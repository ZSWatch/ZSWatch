"""
Hardware application tests for watchdk (and other physical boards).

These tests use the existing pytest fixtures from conftest.py (device_config,
uart_logs, prepare_device) and send shell commands over mcumgr SMP over USB CDC.

Usage examples::

    # Boot verification (uses test_basic.py pattern)
    pytest test_hw_app.py::test_boots --board watchdk@1 --skip-flash -s

    # List registered apps
    pytest test_hw_app.py::test_app_list --board watchdk@1 --skip-flash -s

    # Launch a specific app
    pytest test_hw_app.py::test_app_launches --board watchdk@1 --skip-flash --app Calculator -s

    # Run a shell command
    pytest test_hw_app.py::test_shell_cmd --board watchdk@1 --skip-flash --shell-cmd "battery" -s
"""

import asyncio
import logging
import time

import pytest

from mcumgr_utils import (
    require_usb_port,
    shell_command_usb,
    wait_for_usb_port,
)

log = logging.getLogger(__name__)

USB_TIMEOUT = 15.0


def _ensure_hw(device_config):
    """Skip test if running on native_sim."""
    if device_config.get("board", "") == "native_sim":
        pytest.skip("Hardware test — skipping native_sim")


async def _usb_shell(device_config, cmd_str, timeout_s=10.0):
    """Send a shell command over USB SMP and return the response text."""
    usb_port = require_usb_port(device_config)
    await wait_for_usb_port(usb_port, True, timeout_s=USB_TIMEOUT)
    argv = cmd_str.split()
    response = await shell_command_usb(device_config, argv, timeout_s=timeout_s)
    return response.o.strip() if response.o else ""


# ── Tests ─────────────────────────────────────────────────────


@pytest.mark.asyncio
async def test_app_list(device_config):
    """List all registered apps via shell."""
    _ensure_hw(device_config)
    output = await _usb_shell(device_config, "app list")
    print(f"\n=== Registered Apps ===\n{output}")
    assert output, "No output from 'app list'"


@pytest.mark.asyncio
async def test_app_state(device_config):
    """Show the current UI state."""
    _ensure_hw(device_config)
    output = await _usb_shell(device_config, "app state")
    print(f"\n=== App State ===\n{output}")


@pytest.mark.asyncio
async def test_app_launches(device_config, request):
    """Launch an application on hardware and verify no crash."""
    _ensure_hw(device_config)
    app_name = request.config.getoption("--app") or "Calculator"

    output = await _usb_shell(device_config, f"app launch {app_name}")
    print(f"Launch response: {output}")

    await asyncio.sleep(2)

    # Check for crashes in UART logs
    collector = device_config.get("uart_logs")
    if collector and collector.has_source():
        logs = collector.get_text()
        assert "ASSERTION FAIL" not in logs, "Assertion failure after app launch"
        assert "FATAL" not in logs, "Fatal error after app launch"

    # Check app state
    state_output = await _usb_shell(device_config, "app state")
    print(f"App state: {state_output}")


@pytest.mark.asyncio
async def test_app_close(device_config, request):
    """Launch an application, close it, verify clean shutdown."""
    _ensure_hw(device_config)
    app_name = request.config.getoption("--app") or "Calculator"

    await _usb_shell(device_config, f"app launch {app_name}")
    await asyncio.sleep(2)

    close_output = await _usb_shell(device_config, "app close")
    print(f"Close response: {close_output}")

    await asyncio.sleep(1)

    collector = device_config.get("uart_logs")
    if collector and collector.has_source():
        logs = collector.get_text()
        assert "ASSERTION FAIL" not in logs, "Assertion failure after app close"
        assert "FATAL" not in logs, "Fatal error after app close"


@pytest.mark.asyncio
async def test_shell_cmd(device_config, request):
    """Run an arbitrary shell command on hardware and print the output."""
    _ensure_hw(device_config)
    cmd = request.config.getoption("--shell-cmd") or "app list"

    output = await _usb_shell(device_config, cmd)
    print(f"\n=== Shell: {cmd} ===\n{output}")
    assert output is not None  # command executed without SMP error
