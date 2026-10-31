import logging
import asyncio
import json
import time
from typing import Final

from smpclient.generics import error, success
from smpclient.requests.os_management import EchoWrite

import pytest
from bleak import BleakScanner, BleakClient

from bench.ble import NUS_RX as _NUS_RX, NUS_TX as _NUS_TX, gb_packet as _gb_packet, json_records as _json_records

from mcumgr_utils import (
    find_ble_address as _find_ble_address,
    with_ble_client as _with_ble_client,
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
    # SMP is always registered; no need to enable it first.
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


# Open-Meteo shaped body. The watch's Weather app needs every field below, and the
# non-ASCII city name checks that escaped \uXXXX and \" survive the in-place decode.
WEATHER_BODY: Final = {
    "city": "Malmö \"test\"",
    "current": {
        "temperature_2m": 12.5,
        "weather_code": 3,
        "wind_speed_10m": 4.2,
        "relative_humidity_2m": 81,
        "wind_direction_10m": 220,
        "apparent_temperature": 10.1,
    },
    "daily": {
        "weather_code": [3, 61, 0, 2],
        "temperature_2m_max": [13.0, 11.2, 9.8, 10.5],
        "temperature_2m_min": [7.1, 6.0, 4.4, 5.2],
        "precipitation_probability_max": [10, 80, 0, 20],
    },
}


async def _send_gb_chunked(client: BleakClient, obj: dict):
    packet = _gb_packet(obj)
    chunk = client.mtu_size - 3
    for i in range(0, len(packet), chunk):
        await client.write_gatt_char(_NUS_RX, packet[i:i + chunk], response=True)


async def _wait_for_record(incoming: bytearray, since: int, predicate, timeout_s: float) -> dict:
    deadline = asyncio.get_running_loop().time() + timeout_s
    while asyncio.get_running_loop().time() < deadline:
        found = [r for r in _json_records(bytes(incoming[since:])) if predicate(r)]
        if found:
            return found[-1]
        await asyncio.sleep(0.1)
    raise TimeoutError("No matching NUS record from the watch")


def _is_gps_power(status: bool):
    return lambda r: r.get("t") == "gps_power" and r.get("status") is status


def _is_weather_request(r: dict) -> bool:
    # The Altimeter app asks the same host for GPS and HTTP; only Weather asks for daily data.
    return r.get("t") == "http" and "daily=" in r.get("url", "")


async def _answer_gps_and_get_weather_request(client, incoming: bytearray, timeout_s: float) -> dict:
    """Act as the phone: answer the watch's GPS request and return the Weather app's HTTP request."""
    await _wait_for_record(incoming, 0, _is_gps_power(True), timeout_s)
    since = len(incoming)
    await _send_gb_chunked(client, {"t": "gps", "lat": 55.6, "lon": 13.0})
    return await _wait_for_record(incoming, since, _is_weather_request, 10.0)


@pytest.mark.asyncio
async def test_http_request_over_nus(device_config):
    """Play the phone's HTTP relay for the Weather app: an error reply, then the retry with a body."""
    mac = device_config["mac"]
    incoming = bytearray()
    # Scan first: right after a reset BlueZ does not find a bonded watch by address alone.
    device = await BleakScanner.find_device_by_address(mac, timeout=30.0)
    assert device, f"Device with MAC {mac} not found in BLE scan!"
    async with BleakClient(device, timeout=60.0) as client:
        await client.start_notify(_NUS_TX, lambda _sender, data: incoming.extend(data))
        # NUS RX needs an encrypted link. The watch asks a bonded peer for it 3 s after
        # connecting; a write before that can leave BlueZ's write queue stuck.
        await asyncio.sleep(6)
        # BlueZ does not report the exchanged MTU by itself, and 20-byte writes at the
        # watch's idle interval are too slow to answer within the 10 s HTTP timeout.
        await client._backend._acquire_mtu()
        # The fetch that starts 2 s after connecting can beat the subscription. A time
        # sync, as the phone sends after connecting, starts a new one while weather is stale.
        await client.write_gatt_char(_NUS_RX, f"setTime({int(time.time())});\n".encode(), response=True)

        request = await _answer_gps_and_get_weather_request(client, incoming, 20.0)
        assert "api.open-meteo.com" in request["url"]
        await _send_gb_chunked(client, {"t": "http", "id": request["id"], "err": "Internet access not allowed"})

        # Weather retries 30 s after a failed request, so a new request proves the error path.
        del incoming[:]
        request = await _answer_gps_and_get_weather_request(client, incoming, 45.0)
        since = len(incoming)
        await _send_gb_chunked(client, {"t": "http", "id": request["id"], "resp": json.dumps(WEATHER_BODY)})

        collector = device_config.get("uart_logs")
        if collector is not None and collector.has_source():
            await asyncio.sleep(2)
            log = collector.get_text()
            assert "Invalid weather response" not in log
            assert "Weather received and cached" in log
        else:
            # Without logs: a parsed body stops the 30 s retries.
            await asyncio.sleep(35)
            assert not [r for r in _json_records(bytes(incoming[since:])) if _is_weather_request(r)], \
                "Weather retried after a valid body"
