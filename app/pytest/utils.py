import asyncio
import subprocess
import logging
import sys
import threading
import time

import serial

log = logging.getLogger()


def current_milli_time():
    """Get times in milliseconds"""
    return round(time.time() * 1000)


def reset(device_config):
    """Reset DUT"""
    log.info("Reset DUT")
    subprocess.run(
        [
            "nrfjprog",
            "--family",
            "nrf53",
            "--snr",
            device_config["jlink_serial"],
            "--reset",
        ],
        shell=False,
        stderr=subprocess.STDOUT,
        text=True,
        check=True,
    )


# QSPI XIP base for nRF53.
_QSPI_XIP_BASE = 0x10000000

# MCUboot secondary slot offsets in external flash for watchdk.
# From pm_static_watchdk_nrf5340_cpuapp_1.yml:
#   mcuboot_secondary   (image 0): 0x000000 – 0x0D4000
#   mcuboot_secondary_1 (image 1): 0x0D4000 – 0x114000
#   mcuboot_secondary_2 (image 2): 0x220000 – 0x320000
_QSPI_SECONDARY_SLOTS = [
    (0x000000, 0x0D4000),
    (0x0D4000, 0x114000),
    (0x220000, 0x320000),
]


def erase_secondary_slots(device_config):
    """Erase MCUboot secondary slots in QSPI to clear stale DFU images."""
    log.info("Erasing MCUboot secondary slots in QSPI.")
    for start_off, end_off in _QSPI_SECONDARY_SLOTS:
        start = _QSPI_XIP_BASE + start_off
        end = _QSPI_XIP_BASE + end_off
        try:
            subprocess.run(
                [
                    "nrfjprog",
                    "--family",
                    "nrf53",
                    "--snr",
                    device_config["jlink_serial"],
                    "--qspiini",
                    "../boards/zswatch/watchdk/support/qspi_mx25u51245.ini",
                    "--erasepage",
                    f"0x{start:X}-0x{end:X}",
                ],
                shell=False,
                stderr=subprocess.STDOUT,
                stdout=subprocess.PIPE,
                text=True,
                check=True,
            )
        except subprocess.CalledProcessError as e:
            log.error(f"Error erasing QSPI region 0x{start:X}-0x{end:X}: {e.stdout}")
            raise


def flash(device_config):
    """Flash firmware"""
    # Erase MCUboot secondary slots first to prevent MCUboot from swapping
    # stale DFU images into the primary slots after a debugger re-flash.
    erase_secondary_slots(device_config)

    log.info("Flashing CP_APPLICATION.")
    try:
        subprocess.run(
            [
                "nrfjprog",
                "--family",
                "nrf53",
                "--snr",
                device_config["jlink_serial"],
                "--program",
                f"./{device_config['board']}_nrf5340_cpuapp_debug.hex",
                "--chiperase",
                "--qspisectorerase",
                "--verify",
                "--qspiini",
                "../boards/zswatch/watchdk/support/qspi_mx25u51245.ini",
            ],
            shell=False,
            stderr=subprocess.STDOUT,
            stdout=subprocess.PIPE,
            text=True,
            check=True,
        )
    except subprocess.CalledProcessError as e:
        log.error(f"Error flashing CP_APPLICATION: {e.stdout}")
        raise

    log.info("Flashing CP_NETWORK.")
    try:
        subprocess.run(
            [
                "nrfjprog",
                "--family",
                "nrf53",
                "--snr",
                device_config["jlink_serial"],
                "--program",
                "zswatch_nrf5340_CPUNET.hex",
                "--coprocessor",
                "CP_NETWORK",
                "--sectorerase",
                "--verify",
                "--reset",
            ],
            shell=False,
            stderr=subprocess.STDOUT,
            stdout=subprocess.PIPE,
            text=True,
            check=True,
        )
    except subprocess.CalledProcessError as e:
        log.error(f"Error flashing CP_NETWORK: {e.stdout}")
        raise


def recover(device_config):
    """Recover Device"""
    log.info("Recovering CP_APPLICATION.")
    subprocess.run(
        [
            "nrfjprog",
            "--family",
            "nrf53",
            "--snr",
            device_config["jlink_serial"],
            "--recover",
        ],
        shell=False,
        stderr=subprocess.STDOUT,
        text=True,
        check=True,
    )

    log.info("Recovering CP_NETWORK.")
    subprocess.run(
        [
            "nrfjprog",
            "--family",
            "nrf53",
            "--snr",
            device_config["jlink_serial"],
            "--recover",
            "--coprocessor",
            "CP_NETWORK",
        ],
        shell=False,
        stderr=subprocess.STDOUT,
        text=True,
        check=True,
    )


def read_serial(serial_device, timeout_ms=10000):
    """Read serial output from a given serial handle"""
    start_time = current_milli_time()
    read_data = ""

    while start_time + timeout_ms > current_milli_time():
        if serial_device.in_waiting > 0:
            data = serial_device.read(serial_device.in_waiting).decode(errors="replace")
            read_data += data
        time.sleep(0.1)

    print(read_data)
    log.debug(read_data)
    return read_data


def read_log(device_config, timeout_ms=10000):
    """Read from serial when available (legacy helper, prefer LogCollector)."""
    serial_dev = device_config.get("serial")
    if serial_dev is None:
        log.error("Serial logging not configured for this device.")
        return ""
    return read_serial(serial_dev, timeout_ms)


class LogCollector:
    """Simple background collector for UART output."""

    def __init__(self, device_config, show_uart: bool = False):
        self._device_config = device_config
        self._show_uart = show_uart
        self._history: list[str] = []
        self._lock = threading.Lock()
        self._update_event = threading.Event()
        self._stop_event = threading.Event()
        self._thread: threading.Thread | None = None
        self._has_source = False

    def start(self) -> None:
        if self._thread is not None:
            return
        if self._device_config.get("serial") is not None:
            self._thread = threading.Thread(target=self._serial_loop, daemon=True)
            self._has_source = True
        else:
            self._has_source = False
            return
        self._stop_event.clear()
        self._update_event.clear()
        self._thread.start()

    def stop(self) -> None:
        self._stop_event.set()
        self._update_event.set()
        thread = self._thread
        if thread is not None:
            thread.join(timeout=2.0)
        self._thread = None

    def clear(self) -> None:
        with self._lock:
            self._history.clear()
        self._update_event.set()

    def get_text(self) -> str:
        with self._lock:
            return "".join(self._history)

    def wait_for(self, pattern: str, timeout_s: float) -> bool:
        deadline = time.time() + timeout_s
        while time.time() < deadline:
            if pattern in self.get_text():
                return True
            remaining = deadline - time.time()
            if remaining <= 0:
                break
            self._update_event.wait(remaining)
            self._update_event.clear()
        return pattern in self.get_text()

    async def wait_for_async(self, pattern: str, timeout_s: float) -> bool:
        loop = asyncio.get_running_loop()
        return await loop.run_in_executor(None, self.wait_for, pattern, timeout_s)

    def has_source(self) -> bool:
        return self._has_source

    def _append(self, chunk: str) -> None:
        if not chunk:
            return
        with self._lock:
            self._history.append(chunk)
        if self._show_uart:
            print(chunk, end="", file=sys.stdout, flush=True)
        self._update_event.set()

    def _serial_loop(self) -> None:
        serial_dev = self._device_config.get("serial")
        if serial_dev is None:
            return
        try:
            serial_dev.reset_input_buffer()
        except Exception:
            pass
        while not self._stop_event.is_set():
            try:
                waiting = serial_dev.in_waiting
                if waiting:
                    data = serial_dev.read(waiting)
                else:
                    data = serial_dev.read(1)
                if data:
                    chunk = data.decode(errors="replace")
                    self._append(chunk)
                else:
                    time.sleep(0.05)
            except Exception:
                break
