"""Explicit USB status probe; excluded from the ordinary destructive HIL suite."""

from pathlib import Path

import pytest


@pytest.mark.companion_session
@pytest.mark.asyncio
async def test_companion_control_status(device_config, request, tmp_path):
    # Prevent accidental invocation with the ordinary destructive HIL defaults.
    assert request.config.getoption("--preserve-session"), "Use --preserve-session"
    uart = device_config.get("serial_port")
    smp = device_config.get("usb_cdc_port")
    assert uart and Path(uart).exists(), "Configure an existing UART log port"
    assert smp and Path(smp).exists(), "Configure an existing USB SMP control port"
    assert Path(uart).resolve() != Path(smp).resolve(), "UART and SMP must be separate"
    from mcumgr_utils import shell_command_usb

    for command, expected in (
        (["app", "state"], "UI state:"),
        (["voice_memo", "status"], "Recording:"),
    ):
        response = await shell_command_usb(device_config, command, timeout_s=10.0)
        output = response.o or ""
        (tmp_path / f"{command[0]}-status.txt").write_text(output)
        assert response.ret == 0, f"Shell command failed: {output}"
        assert expected in output, f"Unexpected response to {command}: {output}"
