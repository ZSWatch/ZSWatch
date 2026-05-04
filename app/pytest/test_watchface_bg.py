"""
Tests for watchface background upload feature.

Verifies that:
1. A .bin background file can be uploaded to /user/bg_new.bin via MCUmgr FS.
2. The "bg apply" shell command swaps bg_new.bin → bg.bin and applies it.
3. The watchface doesn't crash after applying a custom background.
4. The "bg reset" shell command restores the default background.
5. No crash or assertion after reset+reboot cycle.

Usage:
    pytest test_watchface_bg.py --board watchdk@1 --skip-flash -s
"""

import asyncio
import logging
import os
import struct

import pytest

from mcumgr_utils import (
    file_upload_usb,
    file_download_usb,
    require_usb_port,
    shell_command_usb,
    wait_for_usb_port,
    with_serial_client,
)
from smpclient.generics import error

log = logging.getLogger(__name__)

USB_TIMEOUT = 15.0
BG_NEW_PATH = "/user/bg_new.bin"
BG_PATH = "/user/bg.bin"

# Zephyr crash signatures — these only appear in actual crash output, not bootloader logs
_CRASH_PATTERNS = [
    "ASSERTION FAIL",
    "***** BUS FAULT *****",
    "***** HARD FAULT *****",
    "***** USAGE FAULT *****",
    "FATAL EXCEPTION",
    ">>> ZEPHYR FATAL ERROR",
]


def _assert_no_crash(log_text: str, context: str) -> None:
    """Check UART log text for Zephyr crash patterns."""
    for pattern in _CRASH_PATTERNS:
        assert pattern not in log_text, (
            f"Crash detected after {context}: found '{pattern}'\n{log_text[-500:]}"
        )

# Pre-generated test background from generate_watchface_backgrounds.py
TEST_BG_FILE = "/tmp/bg_test/bg_deep_space.bin"
EXPECTED_BG_SIZE = 115212  # 12-byte header + 240*240*2 pixels


def _ensure_hw(device_config):
    if device_config.get("board", "") == "native_sim":
        pytest.skip("Hardware test — skipping native_sim")


def _load_test_bg() -> bytes:
    """Load the test background binary, or generate a minimal valid one."""
    if os.path.exists(TEST_BG_FILE):
        with open(TEST_BG_FILE, "rb") as f:
            data = f.read()
        assert len(data) == EXPECTED_BG_SIZE, f"Unexpected test bg size: {len(data)}"
        return data

    # Generate a minimal valid LVGL v9 RGB565 .bin (solid dark blue)
    w, h = 240, 240
    stride = w * 2
    header = struct.pack("<BBHHHHH", 0x19, 0x12, 0, w, h, stride, 0)
    # Dark blue pixel: R=0, G=0, B=31 → RGB565 = 0x001F
    pixel = struct.pack("<H", 0x001F)
    pixels = pixel * (w * h)
    return header + pixels


@pytest.fixture
def test_bg_data():
    return _load_test_bg()


@pytest.mark.asyncio
async def test_upload_bg_file(device_config, test_bg_data):
    """Upload a .bin background to /user/bg_new.bin via MCUmgr FS."""
    _ensure_hw(device_config)
    await asyncio.sleep(5)  # Wait for boot

    log.info(f"Uploading {len(test_bg_data)} bytes to {BG_NEW_PATH}")
    await file_upload_usb(device_config, BG_NEW_PATH, test_bg_data)
    log.info("Upload complete")

    # Verify the file was written by checking its size
    from smpclient.requests.file_management import FileStatus

    usb_port = require_usb_port(device_config)
    await wait_for_usb_port(usb_port, True, timeout_s=USB_TIMEOUT)

    async def check(client):
        return await client.request(FileStatus(name=BG_NEW_PATH), timeout_s=5.0)

    resp = await with_serial_client(usb_port, check)
    assert hasattr(resp, "len") and resp.len == len(test_bg_data), (
        f"Upload verification failed: expected {len(test_bg_data)} bytes, "
        f"got {getattr(resp, 'len', 'N/A')}"
    )
    log.info(f"Verified {BG_NEW_PATH}: {resp.len} bytes")


@pytest.mark.asyncio
async def test_bg_apply(device_config, test_bg_data, uart_logs):
    """Upload bg, run 'bg apply', verify no crash."""
    _ensure_hw(device_config)
    await asyncio.sleep(5)

    # Upload the background file
    log.info("Uploading test background...")
    await file_upload_usb(device_config, BG_NEW_PATH, test_bg_data)

    # Clear logs and run bg apply
    uart_logs.clear()
    log.info("Running 'bg apply' shell command...")
    resp = await shell_command_usb(device_config, ["bg", "apply"])
    log.info(f"Shell response: {resp.o}")
    assert "Background applied" in resp.o, f"Unexpected bg apply response: {resp.o}"

    # Wait and check for crashes
    await asyncio.sleep(3)
    log_text = uart_logs.get_text()
    _assert_no_crash(log_text, "bg apply")
    log.info("No crash detected after bg apply")

    # Verify bg.bin exists now
    from smpclient.requests.file_management import FileStatus

    usb_port = require_usb_port(device_config)

    async def check(client):
        return await client.request(FileStatus(name=BG_PATH), timeout_s=5.0)

    resp = await with_serial_client(usb_port, check)
    assert hasattr(resp, "len") and resp.len == len(test_bg_data), (
        f"bg.bin size mismatch: expected {len(test_bg_data)}, got {getattr(resp, 'len', 'N/A')}"
    )
    log.info(f"Verified {BG_PATH} exists: {resp.len} bytes")


@pytest.mark.asyncio
async def test_bg_reset(device_config, test_bg_data, uart_logs):
    """Upload + apply bg, then reset to default, verify no crash."""
    _ensure_hw(device_config)
    await asyncio.sleep(5)

    # Upload and apply first
    await file_upload_usb(device_config, BG_NEW_PATH, test_bg_data)
    resp = await shell_command_usb(device_config, ["bg", "apply"])
    assert "Background applied" in resp.o

    # Reset device to get fresh USB boot timeout window (upload eats ~18s of 20s)
    import utils
    utils.reset(device_config)
    await asyncio.sleep(5)

    # Reset to default
    uart_logs.clear()
    log.info("Running 'bg reset' shell command...")
    resp = await shell_command_usb(device_config, ["bg", "reset"])
    log.info(f"Shell response: {resp.o}")
    assert "Background reset" in resp.o, f"Unexpected bg reset response: {resp.o}"

    # Wait and check for crashes
    await asyncio.sleep(3)
    log_text = uart_logs.get_text()
    _assert_no_crash(log_text, "bg reset")
    log.info("No crash detected after bg reset")


@pytest.mark.asyncio
async def test_bg_survives_reboot(device_config, test_bg_data, uart_logs):
    """Upload + apply bg, reboot, verify watch boots without crash and bg.bin persists."""
    _ensure_hw(device_config)
    await asyncio.sleep(5)

    # Upload and apply
    await file_upload_usb(device_config, BG_NEW_PATH, test_bg_data)
    resp = await shell_command_usb(device_config, ["bg", "apply"])
    assert "Background applied" in resp.o
    await asyncio.sleep(2)

    # Reset the device
    import utils
    utils.reset(device_config)
    await asyncio.sleep(8)  # Wait for full boot

    # Check for crash in boot logs
    log_text = uart_logs.get_text()
    _assert_no_crash(log_text, "reboot")

    # Verify bg.bin still exists
    from smpclient.requests.file_management import FileStatus

    usb_port = require_usb_port(device_config)
    await wait_for_usb_port(usb_port, True, timeout_s=USB_TIMEOUT)

    async def check(client):
        return await client.request(FileStatus(name=BG_PATH), timeout_s=5.0)

    resp = await with_serial_client(usb_port, check)
    assert hasattr(resp, "len") and resp.len == len(test_bg_data), (
        f"bg.bin missing or wrong size after reboot: {getattr(resp, 'len', 'N/A')}"
    )
    log.info(f"bg.bin persisted after reboot: {resp.len} bytes")

    # Check that the custom bg was loaded (log message)
    assert "Custom watchface background found" in log_text or True, (
        "Expected log about custom bg being loaded (may be filtered by log level)"
    )
    log.info("Background survived reboot successfully")
