import asyncio
from pathlib import Path
import zipfile

import pytest

from smpclient.generics import error, success
from smpclient.requests.os_management import ResetWrite
from smpclient.requests.image_management import ImageStatesRead, ImageStatesWrite

import utils
from mcumgr_utils import (
    enable_ble_fota as _enable_ble_fota,
    enter_serial_recovery as _enter_serial_recovery,
    find_ble_address as _find_ble_address,
    require_usb_port as _require_usb_port,
    require_mcuboot_usb_port as _require_mcuboot_usb_port,
    shell_command_ble as _shell_command_ble,
    shell_command_usb as _shell_command_usb,
    wait_for_usb_port as _wait_for_usb_port,
    with_ble_client as _with_ble_client,
    with_serial_client as _with_serial_client,
)

BOOT_LOG_PATTERN = "Disable Pairable"
FIRMWARE_ZIP = Path(__file__).resolve().parent / "firmware" / "dfu_application_dk.zip"
FIRMWARE_SEQUENCE = [
    "app.internal.bin",
    "app.external.bin",
    "ipc_radio.bin",
]
APPLICATION_IMAGE_IDS = {
    "app.internal.bin": 0,
    "ipc_radio.bin": 1,
    "app.external.bin": 2,
}
SERIAL_RECOVERY_IMAGE_IDS = {
    "app.internal.bin": 1,
    "ipc_radio.bin": 3,
    "app.external.bin": 5,
}


def _ensure_watchdk(device_config):
    board = device_config.get("board", "")
    if not board.startswith("watchdk"):
        pytest.skip("Firmware update tests only run on watchdk targets")


def _load_firmware_bins():
    with zipfile.ZipFile(FIRMWARE_ZIP, "r") as archive:
        return {name: archive.read(name) for name in FIRMWARE_SEQUENCE}

def _clear_uart_logs(device_config):
    collector = device_config.get("uart_logs")
    collector.clear()

async def _reset_device(device_config):
    await asyncio.to_thread(utils.reset, device_config)
    await asyncio.sleep(2)


async def _wait_fw_update_boot_log(device_config, serial_recovery: bool = False, timeout_s: float = 60.0):
    collector = device_config.get("uart_logs")
    if collector is None or not collector.has_source():
        assert False, "No UART log source configured"
    found = True
    if not serial_recovery:
        found = await collector.wait_for_async("Image 0 upgrade secondary slot", timeout_s)
        found = found and await collector.wait_for_async("Image 1 upgrade secondary slot", timeout_s)
        found = found and await collector.wait_for_async("Turned on network core", timeout_s)
    found = found and await collector.wait_for_async(BOOT_LOG_PATTERN, timeout_s)
    if not found:
        pytest.fail(f"Boot log did not contain '{BOOT_LOG_PATTERN}' within {timeout_s}s")


async def _upload_images(client, image_map, serial_recovery_mode: bool = False):
    firmware_bins = _load_firmware_bins()
    for filename in FIRMWARE_SEQUENCE:
        image_id = image_map[filename]
        data = firmware_bins[filename]
        last_offset = 0
        async for offset in client.upload(data, slot=image_id):
            last_offset = offset
        assert last_offset == len(data)
        if serial_recovery_mode and filename == "ipc_radio.bin":
            # In serial recovery mode we need to wait after uploading the radio firmware
            # to allow net core bootloader to copy the image from RAM to net core flash
            # This takes less than 30s according to documentation
            print("Waiting 30s for net core to copy radio firmware...")
            await asyncio.sleep(30)
    if not serial_recovery_mode:
        state_response = await client.request(ImageStatesRead())
        if error(state_response):
            pytest.fail(f"Failed to read image state: {state_response}")
        assert success(state_response)
        for state in state_response.images:
            if state.confirmed == True or state.hash is None:
                continue
            response = await client.request(ImageStatesWrite(hash=state.hash))
            if error(response):
                pytest.fail(f"Failed to set image pending: {response}")
            assert success(response)


async def _upload_images_and_reset(client, image_map, serial_recovery_mode: bool = False):
    await _upload_images(client, image_map, serial_recovery_mode=serial_recovery_mode)
    try:
        reset_response = await client.request(ResetWrite())
        if error(reset_response) and not serial_recovery_mode:
            pytest.fail(f"mcumgr reset failed: {reset_response}")
    except TimeoutError:
        pass


@pytest.mark.asyncio
@pytest.mark.usefixtures("restore_firmware")
async def test_usb_firmware_update(device_config):
    _ensure_watchdk(device_config)
    usb_port = _require_usb_port(device_config)
    await _wait_for_usb_port(usb_port, True, timeout_s=15.0)
    _clear_uart_logs(device_config)
    await _with_serial_client(
        usb_port,
        lambda client: _upload_images_and_reset(client, APPLICATION_IMAGE_IDS),
    )

    await _wait_fw_update_boot_log(device_config)


@pytest.mark.asyncio
@pytest.mark.usefixtures("restore_firmware")
async def test_ble_firmware_update(device_config):
    _ensure_watchdk(device_config)
    usb_port = _require_usb_port(device_config)
    await _wait_for_usb_port(usb_port, True, timeout_s=15.0)

    await _enable_ble_fota(device_config)
    await asyncio.sleep(2)

    address = await _find_ble_address(device_config)
    _clear_uart_logs(device_config)
    await _with_ble_client(
        address,
        lambda client: _upload_images_and_reset(client, APPLICATION_IMAGE_IDS),
    )

    await _wait_fw_update_boot_log(device_config)


@pytest.mark.asyncio
@pytest.mark.usefixtures("restore_firmware")
async def test_serial_recovery_firmware_update(device_config):
    _ensure_watchdk(device_config)
    usb_port = _require_usb_port(device_config)
    usb_mcuboot_port = _require_mcuboot_usb_port(device_config)

    await _enter_serial_recovery(device_config)
    _clear_uart_logs(device_config)
    await _with_serial_client(
        usb_mcuboot_port,
        lambda client: _upload_images_and_reset(client, SERIAL_RECOVERY_IMAGE_IDS, serial_recovery_mode=True),
    )

    await _wait_fw_update_boot_log(device_config, serial_recovery=True)
    await _wait_for_usb_port(usb_port, True, timeout_s=30.0)



@pytest.mark.asyncio
async def test_usb_auto_disable(device_config):
    _ensure_watchdk(device_config)
    usb_port = _require_usb_port(device_config)

    await _reset_device(device_config)
    await _wait_for_usb_port(usb_port, True, timeout_s=15.0)
    await _wait_for_usb_port(usb_port, False, timeout_s=30.0)

    await _reset_device(device_config)
    await _wait_for_usb_port(usb_port, True, timeout_s=15.0)


@pytest.mark.asyncio
async def test_shell_command_over_usb(device_config):
    _ensure_watchdk(device_config)
    usb_port = _require_usb_port(device_config)

    await _wait_for_usb_port(usb_port, True, timeout_s=15.0)
    response = await _shell_command_usb(device_config, ["ble_fota", "status"])
    assert "BLE FOTA is currently" in response.o


@pytest.mark.asyncio
async def test_shell_command_over_ble(device_config):
    _ensure_watchdk(device_config)
    usb_port = _require_usb_port(device_config)

    await _wait_for_usb_port(usb_port, True, timeout_s=15.0)
    await _enable_ble_fota(device_config)
    await asyncio.sleep(2)

    response = await _shell_command_ble(device_config, ["ble_fota", "status"])
    print(f"BLE FOTA status: {response.o}")
    assert "enabled" in response.o.lower()

    await _shell_command_usb(device_config, ["ble_fota", "disable"])
