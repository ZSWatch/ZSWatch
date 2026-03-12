"""
Native simulator tests — boot verification, app testing, and BLE integration.

Tests are organized into two classes:

  TestNativeSim     — Core tests (boot, app launch/close, screenshot). No BLE.
  TestNativeSimBLE  — BLE boot verification. Requires BLEAK_ADAPTER env var.

Usage examples::

    # Boot verification (no BLE)
    pytest test_native_app.py::TestNativeSim::test_boots -s

    # Launch a specific app
    pytest test_native_app.py::TestNativeSim::test_app_launches -s --app Calc

    # Take a screenshot
    pytest test_native_app.py::TestNativeSim::test_app_screenshot -s --app Calc

    # All non-BLE tests
    pytest test_native_app.py::TestNativeSim -s --app Calc

    # BLE boot verification (requires BLEAK_ADAPTER env var)
    BLEAK_ADAPTER=hci0 pytest test_native_app.py::TestNativeSimBLE -s

Options:
    --app NAME          Application name to launch (e.g. "Calculator")
    --exe-path PATH     Path to zephyr.exe (auto-detected if not provided)
    --screenshot-dir DIR  Directory for screenshots (default: /tmp)
"""

import os
import subprocess
import time

import pytest
from native_sim_runner import NativeSimDevice

BOOT_TIMEOUT = 15  # seconds
# Boot marker: fired by zsw_ui_controller after the watchface is up.
BOOT_MARKER = "UI Controller initialized"


# ── Override conftest autouse fixtures ────────────────────────
# The global conftest.py has autouse fixtures (prepare_device, reset_device,
# uart_logs) that depend on device_config, which triggers device parametrization.
# Native_sim tests manage their own process — override these to be no-ops.

@pytest.fixture(autouse=True)
def prepare_device():
    yield


@pytest.fixture(autouse=True)
def reset_device():
    yield


@pytest.fixture(scope="function", autouse=True)
def uart_logs():
    yield None


# ── Helpers ───────────────────────────────────────────────────

def _find_exe(request):
    """Find the native_sim executable (build output or CI artifact)."""
    custom = request.config.getoption("--exe-path")
    if custom:
        return os.path.abspath(custom)

    repo_root = os.path.normpath(os.path.join(os.path.dirname(__file__), "..", ".."))

    # Default build output
    build_exe = os.path.join(repo_root, "app", "build", "app", "zephyr", "zephyr.exe")
    if os.path.isfile(build_exe):
        return build_exe

    # CI artifact (downloaded into app/pytest/ by GitHub Actions)
    ci_exe = os.path.join(os.path.dirname(__file__), "zswatch_native_sim_64.exe")
    if os.path.isfile(ci_exe):
        return os.path.abspath(ci_exe)

    return None


# ── Non-BLE tests ────────────────────────────────────────────

@pytest.mark.linux_only
class TestNativeSim:
    """Core native_sim tests — no BLE required."""

    @pytest.fixture(scope="class")
    def sim(self, request):
        exe = _find_exe(request)
        if not exe:
            pytest.skip("No native_sim executable found (build or provide --exe-path)")

        device = NativeSimDevice(exe_path=exe)
        device.start()

        booted = device.wait_for_log(BOOT_MARKER, timeout=BOOT_TIMEOUT)
        if not booted:
            logs = device.get_logs()
            device.stop()
            pytest.fail(
                f"native_sim failed to boot within {BOOT_TIMEOUT}s.\n"
                f"Last 30 log lines:\n" + "\n".join(logs.splitlines()[-30:])
            )

        yield device
        device.stop()

    def test_boots(self, sim):
        """Verify native_sim boots successfully."""
        assert BOOT_MARKER in sim.get_logs()
        crash = sim.has_crash()
        if crash:
            logs = sim.get_logs()
            last_lines = "\n".join(logs.splitlines()[-30:])
            assert False, (
                f"Crash detected during boot: {crash}\n"
                f"Last 30 log lines:\n{last_lines}"
            )

    def test_app_launches(self, sim, request):
        """Launch an application and verify it does not crash."""
        app_name = request.config.getoption("--app") or "Calc"

        sim.shell_command(f"app launch {app_name}")
        time.sleep(2)

        crash = sim.has_crash()
        if crash:
            last_lines = "\n".join(sim.get_logs().splitlines()[-30:])
            assert False, (
                f"Crash detected after launching {app_name}: {crash}\n"
                f"Last 30 log lines:\n{last_lines}"
            )

        sim.shell_command("app state")
        time.sleep(0.5)
        print(f"\n=== Shell output ===\n{sim.get_shell_output()}")

    def test_app_screenshot(self, sim, request):
        """Launch an app, take a screenshot, and print the path."""
        app_name = request.config.getoption("--app") or "Calc"
        screenshot_dir = request.config.getoption("--screenshot-dir") or "/tmp"

        sim.shell_command(f"app launch {app_name}")
        time.sleep(2)
        crash = sim.has_crash()
        assert not crash, f"Crash detected after launching {app_name}: {crash}"

        safe_name = app_name.replace(" ", "_").lower()
        path = os.path.join(screenshot_dir, f"zswatch_{safe_name}.png")
        sim.screenshot(path)

        assert os.path.isfile(path), f"Screenshot not created at {path}"
        print(f"\nScreenshot saved: {path}")

    def test_app_close(self, sim, request):
        """Launch an application, close it, and verify clean shutdown."""
        app_name = request.config.getoption("--app") or "Calc"

        sim.shell_command(f"app launch {app_name}")
        time.sleep(2)
        crash = sim.has_crash()
        assert not crash, f"Crash after launching {app_name}: {crash}"

        sim.shell_command("app close")
        time.sleep(1)
        crash = sim.has_crash()
        assert not crash, f"Crash after closing {app_name}: {crash}"

        sim.shell_command("app state")


# ── BLE tests ────────────────────────────────────────────────

@pytest.mark.linux_only
class TestNativeSimBLE:
    """BLE boot verification — requires BLEAK_ADAPTER env var."""

    @pytest.fixture(scope="class", autouse=True)
    def setup_ble(self, request):
        """Check for BLE adapter, disable it for Zephyr, re-enable on teardown."""
        adapter = os.environ.get("BLEAK_ADAPTER")
        if not adapter:
            pytest.skip("BLE tests require BLEAK_ADAPTER env var")

        subprocess.run(
            ["sudo", "hciconfig", adapter, "down"],
            capture_output=True,
        )
        time.sleep(2)

        yield

        # Re-enable adapter
        subprocess.run(["sudo", "hciconfig", adapter, "down"], capture_output=True)
        time.sleep(2)
        subprocess.run(["sudo", "hciconfig", adapter, "up"], capture_output=True)

    @pytest.fixture(scope="class")
    def sim(self, request):
        adapter = os.environ.get("BLEAK_ADAPTER", "hci0")
        exe = _find_exe(request)
        if not exe:
            pytest.skip("No native_sim executable found")

        device = NativeSimDevice(
            exe_path=exe,
            sudo=True,
            extra_args=[f"--bt-dev={adapter}"],
        )
        device.start()

        booted = device.wait_for_log("Disable Pairable", timeout=BOOT_TIMEOUT)
        if not booted:
            logs = device.get_logs()
            device.stop()
            pytest.fail(
                f"BLE boot failed within {BOOT_TIMEOUT}s.\n"
                f"Last 30 log lines:\n" + "\n".join(logs.splitlines()[-30:])
            )

        yield device
        device.stop()

    def test_boots_with_ble(self, sim):
        """Verify native_sim boots with BLE stack initialized."""
        logs = sim.get_logs()
        assert "Start Apple Media Service" in logs
        assert "Disable Pairable" in logs
        assert not sim.has_crash()
