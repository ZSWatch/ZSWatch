"""
NativeSimDevice — manages a native_sim zephyr.exe process for testing.

Provides:
  - Headless display via Xvfb
  - Stdout/stderr log capture
  - Shell command execution via PTY
  - Screenshot capture
  - Boot verification

Usage as a module::

    from native_sim_runner import NativeSimDevice

    device = NativeSimDevice()
    device.start()
    assert device.wait_for_log("UI Controller initialized", timeout=15)
    device.shell_command("app launch Calculator")
    device.screenshot("/tmp/calc.png")
    device.stop()
"""

import os
import re
import select
import signal
import subprocess
import threading
import time


class NativeSimDevice:
    """Manage a native_sim zephyr.exe process with Xvfb and PTY shell access."""

    def __init__(self, exe_path=None, display=":98", workdir=None, sudo=False, extra_args=None):
        self.exe_path = exe_path or "app/build/app/zephyr/zephyr.exe"
        self.display = display
        self.workdir = workdir
        self.sudo = sudo
        self.extra_args = extra_args or []
        self._xvfb = None
        self._proc = None
        self._pty_path = None
        self._pty_fd = None
        self._stdout_chunks = []
        self._pty_chunks = []
        self._stop = threading.Event()
        self._threads = []

    # ── lifecycle ──────────────────────────────────────────────

    def start(self):
        """Start Xvfb and zephyr.exe, find the shell PTY."""
        # Kill leftovers
        if self.sudo:
            subprocess.run(["sudo", "pkill", "-f", "zephyr.exe"], capture_output=True)
        else:
            subprocess.run(["pkill", "-f", "zephyr.exe"], capture_output=True)
        subprocess.run(["pkill", "-f", f"Xvfb {self.display}"], capture_output=True)
        time.sleep(0.3)

        # Start Xvfb. The SDL window is 480x480 (240x240 display at 200% zoom
        # per CONFIG_SDL_DISPLAY_ZOOM_PCT=200). The virtual framebuffer must be
        # at least that large or the window gets clipped and screenshots are gray.
        self._xvfb = subprocess.Popen(
            ["Xvfb", self.display, "-screen", "0", "480x480x24"],
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
        )
        time.sleep(0.5)
        if self._xvfb.poll() is not None:
            raise RuntimeError("Xvfb failed to start")

        # Everything after Xvfb is wrapped so that a failure mid-startup
        # still tears down the processes we already spawned.  Without this,
        # an exception before the pytest yield skips teardown and leaves
        # Xvfb / zephyr.exe behind, poisoning the next run.
        try:
            self._start_firmware()
        except Exception:
            self.stop()
            raise

    def _start_firmware(self):
        """Resolve the executable path, launch zephyr.exe, and open the PTY."""
        # Resolve exe path
        exe = self.exe_path
        if self.workdir and not os.path.isabs(exe):
            exe = os.path.join(self.workdir, exe)
        if not os.path.isfile(exe):
            raise FileNotFoundError(f"Executable not found: {exe}")

        # Ensure executable permission (CI artifacts may lack it)
        try:
            os.chmod(exe, os.stat(exe).st_mode | 0o111)
        except OSError:
            pass

        # Remove any stale flash.bin so NVS settings from a previous run
        # don't interfere.  The file is created in the process CWD by the
        # Zephyr flash simulator driver.
        flash_bin = os.path.join(os.path.dirname(exe), "flash.bin")
        for candidate in [flash_bin, "flash.bin"]:
            try:
                os.remove(candidate)
            except FileNotFoundError:
                pass

        env = os.environ.copy()
        env["DISPLAY"] = self.display

        cmd = [exe, "--flash_erase"] + self.extra_args
        if self.sudo:
            cmd = ["sudo"] + cmd

        self._proc = subprocess.Popen(
            cmd,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            env=env,
        )

        # Background readers for stdout and stderr
        self._start_reader(self._proc.stdout.fileno(), self._stdout_chunks)
        self._start_reader(self._proc.stderr.fileno(), self._stdout_chunks)

        # Find PTY printed at boot (e.g. "uart connected to pseudotty: /dev/pts/3")
        self._pty_path = self._find_pty(timeout=5.0)
        if self._pty_path:
            if self.sudo:
                # PTY is owned by root when running with sudo, make it accessible
                subprocess.run(["sudo", "chmod", "666", self._pty_path], capture_output=True)
            try:
                self._pty_fd = os.open(self._pty_path, os.O_RDWR | os.O_NONBLOCK)
                self._start_reader(self._pty_fd, self._pty_chunks)
                time.sleep(0.5)  # let shell init
            except OSError:
                self._pty_fd = None

    def stop(self):
        """Stop the firmware process, Xvfb, and all readers."""
        self._stop.set()

        if self._pty_fd is not None:
            try:
                os.close(self._pty_fd)
            except OSError:
                pass
            self._pty_fd = None

        if self._proc:
            try:
                self._proc.send_signal(signal.SIGTERM)
                self._proc.wait(timeout=3)
            except Exception:
                self._proc.kill()
                self._proc.wait(timeout=2)
            if self.sudo:
                subprocess.run(["sudo", "pkill", "-f", "zephyr.exe"], capture_output=True)
            self._proc = None

        if self._xvfb:
            self._xvfb.terminate()
            try:
                self._xvfb.wait(timeout=2)
            except Exception:
                self._xvfb.kill()
            self._xvfb = None

        for t in self._threads:
            t.join(timeout=1)
        self._threads.clear()

    # ── shell commands ────────────────────────────────────────

    def shell_command(self, cmd):
        """Send a shell command string via the PTY."""
        if self._pty_fd is None:
            raise RuntimeError("No PTY available — was firmware built with test_mode.conf?")
        if not self.is_alive():
            rc = self._proc.returncode if self._proc else "unknown"
            raise RuntimeError(
                f"Firmware process is dead (rc={rc}), cannot send command: {cmd}\n"
                f"Last 20 log lines:\n"
                + "\n".join(self.get_logs().splitlines()[-20:])
            )
        os.write(self._pty_fd, (cmd + "\n").encode())

    # ── log access ────────────────────────────────────────────

    def get_logs(self):
        """Return all captured stdout/stderr text."""
        return "".join(self._stdout_chunks)

    def get_shell_output(self):
        """Return all captured PTY (shell) output."""
        return "".join(self._pty_chunks)

    def wait_for_log(self, pattern, timeout=10.0):
        """Block until *pattern* appears in stdout or timeout expires."""
        deadline = time.time() + timeout
        while time.time() < deadline:
            if pattern in self.get_logs():
                return True
            time.sleep(0.2)
        return pattern in self.get_logs()

    def is_alive(self):
        """Return True if the firmware process is still running."""
        return self._proc is not None and self._proc.poll() is None

    def has_crash(self):
        """Check logs for Zephyr-specific assertion failures or fatal errors.

        Returns the matched pattern string if a crash is detected, or None.
        Truthy when a crash is found, falsy (None) otherwise.
        """
        crash_re = re.compile(
            r"ASSERTION FAIL"
            r"|ZEPHYR FATAL ERROR"
            r"|\*{3,}\s*(?:BUS FAULT|HardFault|STACK OVERFLOW|Usage fault|MPU FAULT|SecureFault)\s*\*{3,}"
            r"|z_fatal_error"
            r"|#CD:",  # Actual Zephyr coredump data prefix
        )
        m = crash_re.search(self.get_logs())
        if m:
            return m.group()
        # Also treat unexpected process death as a crash
        if not self.is_alive():
            rc = self._proc.returncode if self._proc else "unknown"
            return f"process exited unexpectedly (rc={rc})"
        return None

    # ── screenshots ───────────────────────────────────────────

    def screenshot(self, path="/tmp/zswatch_screenshot.png"):
        """Capture the Xvfb display to a PNG file."""
        env = os.environ.copy()
        env["DISPLAY"] = self.display
        subprocess.run(
            ["import", "-window", "root", path],
            env=env,
            capture_output=True,
            timeout=5,
        )
        return path

    # ── internals ─────────────────────────────────────────────

    def _start_reader(self, fd, chunks):
        t = threading.Thread(target=self._reader, args=(fd, chunks), daemon=True)
        t.start()
        self._threads.append(t)

    def _reader(self, fd, chunks):
        while not self._stop.is_set():
            try:
                r, _, _ = select.select([fd], [], [], 0.1)
                if r:
                    data = os.read(fd, 8192)
                    if not data:
                        break
                    chunks.append(data.decode("utf-8", errors="replace"))
            except (OSError, ValueError):
                break

    def _find_pty(self, timeout=5.0):
        pat = re.compile(r"pseudotty:\s*(/dev/pts/\d+)")
        deadline = time.time() + timeout
        while time.time() < deadline:
            text = self.get_logs()
            m = pat.search(text)
            if m:
                return m.group(1)
            time.sleep(0.1)
        return None
