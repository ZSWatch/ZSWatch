"""
Tests for filesystem log backend (spec 015).

Verifies that:
1. Log files are created on the /user/logs/ LittleFS partition after boot.
2. Log files contain valid Zephyr log text (level tags, timestamps).
3. Log files can be downloaded via MCUmgr FS group over USB SMP.

Usage:
    pytest test_fs_log_backend.py --board watchdk@1 --skip-flash -s
"""

import asyncio
import logging
import time

import pytest

from mcumgr_utils import (
    file_download_usb,
    require_usb_port,
    shell_command_usb,
    wait_for_usb_port,
    with_serial_client,
)
from smpclient.generics import error

log = logging.getLogger(__name__)

USB_TIMEOUT = 15.0
# Zephyr FS backend naming: CONFIG_LOG_BACKEND_FS_FILE_PREFIX + 4-digit number
LOG_FILE_PREFIX = "/user/logs/log"


def _ensure_hw(device_config):
    if device_config.get("board", "") == "native_sim":
        pytest.skip("Hardware test — skipping native_sim")


@pytest.mark.asyncio
async def test_log_files_created(device_config):
    """After boot, at least one log file should exist on /user/logs/."""
    _ensure_hw(device_config)

    # Give firmware time to boot and write some logs
    await asyncio.sleep(5)

    from smpclient.requests.file_management import FileStatus

    usb_port = require_usb_port(device_config)
    await wait_for_usb_port(usb_port, True, timeout_s=USB_TIMEOUT)

    async def check_status(client):
        resp = await client.request(
            FileStatus(name=f"{LOG_FILE_PREFIX}0000"), timeout_s=5.0
        )
        return resp

    resp = await with_serial_client(usb_port, check_status)
    assert hasattr(resp, "len") and resp.len > 0, (
        f"Log file {LOG_FILE_PREFIX}0000 not found or empty — "
        "FS log backend may not be enabled or LittleFS not mounted"
    )
    log.info(f"Log file log0000 exists, size: {resp.len} bytes")


@pytest.mark.asyncio
async def test_log_file_download_and_content(device_config):
    """Download log0000 and verify it contains valid Zephyr log entries."""
    _ensure_hw(device_config)

    await asyncio.sleep(5)

    data = await file_download_usb(device_config, f"{LOG_FILE_PREFIX}0000")
    assert len(data) > 0, "Downloaded log file is empty"

    text = data.decode("utf-8", errors="replace")
    log.info(f"Downloaded {len(data)} bytes from log0000")
    log.info(f"First 500 chars:\n{text[:500]}")

    # Zephyr text-mode logs contain level tags like <inf>, <wrn>, <err>, <dbg>
    has_log_tags = any(tag in text for tag in ["<inf>", "<wrn>", "<err>", "<dbg>"])
    assert has_log_tags, (
        "Log file does not contain Zephyr log level tags — "
        "content may be binary/dictionary format or backend not producing output"
    )


@pytest.mark.asyncio
async def test_log_files_survive_reboot(device_config, uart_logs):
    """Log files should persist across a device reset."""
    _ensure_hw(device_config)

    # Let some logs accumulate
    await asyncio.sleep(5)

    # Download pre-reboot content
    data_before = await file_download_usb(device_config, f"{LOG_FILE_PREFIX}0000")
    assert len(data_before) > 0, "No log data before reboot"

    # Reset the device
    import utils
    utils.reset(device_config)
    await asyncio.sleep(5)

    # Download again — with CONFIG_LOG_BACKEND_FS_APPEND_TO_NEWEST_FILE=y,
    # the file should be >= the pre-reboot size (appended to)
    data_after = await file_download_usb(device_config, f"{LOG_FILE_PREFIX}0000")
    assert len(data_after) >= len(data_before), (
        f"Log file shrank after reboot: {len(data_before)} -> {len(data_after)} bytes. "
        "Expected append or new file creation."
    )
    log.info(
        f"Pre-reboot: {len(data_before)} bytes, post-reboot: {len(data_after)} bytes"
    )


@pytest.mark.asyncio
async def test_log_file_probe_pattern(device_config):
    """Verify the probe-by-status pattern the companion app will use.

    The app probes log0000 through log{FILES_LIMIT-1} using FileStatus.
    This test confirms that pattern works over USB SMP.
    """
    _ensure_hw(device_config)
    await asyncio.sleep(5)

    from smpclient.requests.file_management import FileStatus

    usb_port = require_usb_port(device_config)
    await wait_for_usb_port(usb_port, True, timeout_s=USB_TIMEOUT)

    found_files = []

    async def probe(client):
        for i in range(8):  # CONFIG_LOG_BACKEND_FS_FILES_LIMIT = 8
            path = f"{LOG_FILE_PREFIX}{i:04d}"
            try:
                resp = await client.request(
                    FileStatus(name=path), timeout_s=3.0
                )
                if not error(resp) and hasattr(resp, "len") and resp.len > 0:
                    found_files.append((path, resp.len))
                    log.info(f"  Found: {path} ({resp.len} bytes)")
                else:
                    log.info(f"  Missing: {path}")
            except Exception as e:
                log.info(f"  Error probing {path}: {e}")

    await with_serial_client(usb_port, probe)

    assert len(found_files) > 0, (
        "No log files found via probe pattern — "
        "FS log backend may not be creating files"
    )
    log.info(f"Probe found {len(found_files)} log files")
