import logging
import asyncio
from typing import Final

from smpclient.generics import error, success
from smpclient.requests.os_management import EchoWrite

import pytest
from bleak import BleakScanner, BleakClient

from mcumgr_utils import (
    find_ble_address as _find_ble_address,
    with_ble_client as _with_ble_client,
    require_usb_port as _require_usb_port,
    enable_ble_fota as _enable_ble_fota,
    wait_for_usb_port as _wait_for_usb_port,
)

log = logging.getLogger()


@pytest.mark.asyncio
async def test_advertises(device_config):
    mac = device_config["mac"]
    device_name = "ZSWatch"
    found = False
    connected = False
    log.info(f"Scanning for BLE devices to find {device_name} ({mac})...")
    devices = await BleakScanner.discover(timeout=30.0)
    log.info(f"Found device: {len(devices)} devices")
    for d in devices:
        if mac and d.address.lower() == mac.lower():
            found = True
            break
    assert found, f"Device with MAC {mac} not found in BLE scan!"


@pytest.mark.asyncio
async def test_connect(device_config):
    mac = device_config["mac"]
    connected = False
    async with BleakClient(mac, address_type="random", timeout=30.0) as client:
        log.info(f"Connected to {mac}")
        connected = True
    assert connected, f"Failed to connect to device with MAC {mac}"


@pytest.mark.asyncio
async def test_smp_echo(device_config):
    usb_port = _require_usb_port(device_config)
    await _wait_for_usb_port(usb_port, True, timeout_s=15.0)

    await _enable_ble_fota(device_config)
    await asyncio.sleep(2)
    address = await _find_ble_address(device_config)
    log.info(f"Connecting to SMP server at {address} via helper")

    async def run(client):
        log.info("Sending SMP echo request...")
        response: Final = await client.request(EchoWrite(d="Hello, World!"))
        if success(response):
            log.info(f"Received response: {response}")
        elif error(response):
            pytest.fail(f"Received SMP error response: {response}")
        else:
            pytest.fail(f"Unknown SMP response: {response}")

    await _with_ble_client(address, run)
