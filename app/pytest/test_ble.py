import time
import logging
import utils
import yaml

import asyncio
from typing import Final

from smpclient import SMPClient
from smpclient.generics import error, success
from smpclient.requests.os_management import EchoWrite
from smpclient.transport.ble import SMPBLETransport

import pytest
from bleak import BleakScanner, BleakClient

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
    mac = device_config["mac"]
    log.info("Scanning for SMP servers...")
    smp_servers: Final = await SMPBLETransport.scan(timeout=30.0)
    log.info("OK")
    log.info(f"Found {len(smp_servers)} SMP servers: {smp_servers}")
    log.info(smp_servers)
    matched_server = None
    for server in smp_servers:
        if server.address.lower() == mac.lower():
            matched_server = server
            break
    assert (
        matched_server is not None
    ), f"Device with MAC {mac} not found among SMP servers!"

    log.info("Connecting to the first SMP server...")
    async with SMPClient(SMPBLETransport(), matched_server.address) as client:
        log.info("OK")

        log.info("Sending request...")
        response: Final = await client.request(EchoWrite(d="Hello, World!"))
        log.info("OK")

        if success(response):
            log.info(f"Received response: {response}")
        elif error(response):
            log.info(f"Received error: {response}")
        else:
            raise Exception(f"Unknown response: {response}")
