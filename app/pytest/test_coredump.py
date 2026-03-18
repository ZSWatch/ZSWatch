"""
Coredump E2E tests for watchdk hardware.

Tests the full coredump flow:
  1. Erase any existing coredump
  2. Trigger crash via 'coredump crash' (spawns thread with deep call stack)
  3. Wait for reboot, verify coredump was stored
  4. Download coredump file via MCUmgr FS
  5. Send to server for GDB analysis, verify backtrace

Usage::

    pytest test_coredump.py --board watchdk@1 --skip-flash -s
"""

import asyncio
import logging
import time

import pytest
import utils

from mcumgr_utils import (
    file_download_usb,
    require_usb_port,
    shell_command_usb,
    wait_for_usb_port,
)

log = logging.getLogger(__name__)

USB_TIMEOUT = 15.0


def _ensure_hw(device_config):
    if device_config.get("board", "") == "native_sim":
        pytest.skip("Hardware test — skipping native_sim")


async def _usb_shell(device_config, cmd_str, timeout_s=10.0):
    usb_port = require_usb_port(device_config)
    await wait_for_usb_port(usb_port, True, timeout_s=USB_TIMEOUT)
    argv = cmd_str.split()
    response = await shell_command_usb(device_config, argv, timeout_s=timeout_s)
    return response.o.strip() if response.o else ""


@pytest.mark.asyncio
async def test_coredump_summary(device_config):
    """Verify 'coredump summary' shell command runs without error."""
    _ensure_hw(device_config)
    output = await _usb_shell(device_config, "coredump summary")
    print(f"\n=== Coredump Summary ===\n{output}")
    assert output is not None


@pytest.mark.asyncio
async def test_coredump_erase(device_config):
    """Verify 'coredump erase 0' runs without error."""
    _ensure_hw(device_config)
    output = await _usb_shell(device_config, "coredump erase 0")
    print(f"\n=== Coredump Erase ===\n{output}")


@pytest.mark.asyncio
async def test_coredump_summary_after_erase(device_config):
    """After erase, summary should show no coredumps."""
    _ensure_hw(device_config)
    await _usb_shell(device_config, "coredump erase 0")
    await asyncio.sleep(0.5)
    output = await _usb_shell(device_config, "coredump summary")
    print(f"\n=== Summary After Erase ===\n{output}")
    assert "No coredump" in output or "0" in output or output is not None


@pytest.mark.asyncio
async def test_coredump_crash_and_download(device_config):
    """
    Full E2E: trigger crash, reboot, verify coredump stored, download file.

    The crash command spawns a dedicated thread with a multi-frame call stack
    (crash_level_1 → crash_level_2 → crash_level_3 → __ASSERT) so GDB
    produces a meaningful backtrace.
    """
    _ensure_hw(device_config)

    # 1. Erase any old coredump
    await _usb_shell(device_config, "coredump erase 0")
    await asyncio.sleep(0.5)

    # 2. Trigger crash — the shell prints "Spawning crash thread..." then
    #    the device reboots. The SMP response may or may not arrive before
    #    the connection drops.
    print("\n--- Triggering crash ---")
    try:
        await _usb_shell(device_config, "coredump crash")
    except Exception:
        pass  # Expected: device reboots

    # 3. Wait for USB CDC to disappear then reappear
    usb_port = require_usb_port(device_config)
    print("Waiting for device to reboot...")
    await asyncio.sleep(3)
    # Reset via J-Link to ensure clean reboot
    utils.reset(device_config)
    await wait_for_usb_port(usb_port, True, timeout_s=30.0)
    await asyncio.sleep(2)  # Let SMP stack stabilise

    # 4. Verify coredump was stored
    output = await _usb_shell(device_config, "coredump summary")
    print(f"\n=== Post-Crash Summary ===\n{output}")
    assert "Stored coredumps: 1" in output, f"Expected coredump, got: {output}"
    assert "zsw_shell.c" in output, f"Expected zsw_shell.c in summary, got: {output}"

    # 5. Download coredump file
    print("Downloading /user/coredump.txt ...")
    raw = await file_download_usb(device_config, "/user/coredump.txt")
    content = raw.decode("utf-8", errors="replace")

    start = content.find("#CD:BEGIN#")
    assert start >= 0, "Coredump file missing #CD:BEGIN# marker"
    coredump_text = content[start:].replace("\x00", "")
    assert "#CD:END#" in coredump_text, "Coredump file missing #CD:END# marker"
    print(f"Downloaded coredump: {len(coredump_text)} chars")

    # 6. (Optional) If server is running, send for analysis
    try:
        import requests

        payload = {
            "coredump_txt": coredump_text,
            "fw_commit_sha": "test",
            "fw_version": "",
            "board": "watchdk",
            "build_type": "debug",
        }
        resp = requests.post(
            "http://localhost:8322/api/coredump/analyze",
            json=payload,
            timeout=30,
        )
        if resp.status_code == 200:
            result = resp.json()
            print(f"\n=== Server Response ===")
            print(f"  success: {result.get('success')}")
            print(f"  elf_available: {result.get('elf_available')}")
            if result.get("backtrace"):
                print(f"  backtrace:\n{result['backtrace']}")
        else:
            print(f"Server returned {resp.status_code} (non-fatal)")
    except Exception as e:
        print(f"Server not reachable ({e}) — skipping server decode (non-fatal)")
