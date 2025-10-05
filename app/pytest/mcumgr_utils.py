import asyncio
from pathlib import Path
from typing import Awaitable, Callable, TypeVar

import pytest

from smpclient import SMPClient
from smpclient.exceptions import SMPBadSequence
from smpclient.generics import error, success
from smpclient.requests.shell_management import Execute
from smpclient.transport.ble import SMPBLETransport
from smpclient.transport.serial import SMPSerialTransport


T = TypeVar("T")
ClientHandler = Callable[[SMPClient], Awaitable[T]]


async def drain_pending_frames(client: SMPClient, attempts: int = 3, timeout_s: float = 0.2) -> None:
    """Drain any pending frames left in the transport before starting a new exchange."""
    for _ in range(attempts):
        try:
            await asyncio.wait_for(client._transport.receive(), timeout_s)
        except (asyncio.TimeoutError, TimeoutError):
            break


async def with_serial_client(usb_port: str, handler: ClientHandler[T], attempts: int = 3) -> T:
    """Execute a coroutine with an SMP client over USB, retrying on transient errors."""
    last_exc: Exception | None = None
    for attempt in range(attempts):
        transport = SMPSerialTransport()
        try:
            async with SMPClient(transport, usb_port) as client:
                await drain_pending_frames(client)
                return await handler(client)
        except (TimeoutError, SMPBadSequence) as exc:
            last_exc = exc
            await asyncio.sleep(1.0 * (attempt + 1))
    pytest.fail(f"Failed to communicate with device on {usb_port}: {last_exc}")


async def with_ble_client(address: str, handler: ClientHandler[T], attempts: int = 3) -> T:
    """Execute a coroutine with an SMP client over BLE, retrying on transient errors."""
    last_exc: Exception | None = None
    for attempt in range(attempts):
        transport = SMPBLETransport()
        try:
            async with SMPClient(transport, address) as client:
                return await handler(client)
        except (TimeoutError, SMPBadSequence) as exc:
            last_exc = exc
            await asyncio.sleep(1.0 * (attempt + 1))
    pytest.fail(f"Failed to communicate over BLE ({address}): {last_exc}")


def require_usb_port(device_config) -> str:
    usb_port = device_config.get("usb_cdc_port")
    if not usb_port:
        pytest.skip("usb_cdc_port missing in device configuration")
    return usb_port


async def wait_for_usb_port(usb_port: str, present: bool, timeout_s: float = 30.0, interval_s: float = 0.5) -> None:
    loop = asyncio.get_running_loop()
    deadline = loop.time() + timeout_s
    while True:
        exists = Path(usb_port).exists()
        if exists == present:
            return
        if loop.time() >= deadline:
            state = "present" if present else "absent"
            pytest.fail(f"USB port {usb_port} not {state} within {timeout_s}s")
        await asyncio.sleep(interval_s)


async def find_ble_address(device_config) -> str:
    mac = device_config.get("mac")
    if not mac:
        pytest.skip("Device MAC address not configured")
    return mac


async def shell_command_usb(device_config, argv: list[str], timeout_s: float = 5.0):
    usb_port = require_usb_port(device_config)
    await wait_for_usb_port(usb_port, True, timeout_s=timeout_s)

    async def run(client: SMPClient):
        response = await client.request(Execute(argv=argv), timeout_s=timeout_s)
        if error(response):
            pytest.fail(f"Shell command over USB failed for argv={argv}: {response}")
        assert success(response)
        return response

    return await with_serial_client(usb_port, run)


async def shell_command_ble(device_config, argv: list[str], timeout_s: float = 5.0):
    address = await find_ble_address(device_config)

    async def run(client: SMPClient):
        response = await client.request(Execute(argv=argv), timeout_s=timeout_s)
        if error(response):
            pytest.fail(f"Shell command over BLE failed for argv={argv}: {response}")
        assert success(response)
        return response

    return await with_ble_client(address, run)


async def enable_ble_fota(device_config):
    response = await shell_command_usb(device_config, ["ble_fota", "enable"])
    assert "BLE FOTA" in response.o


async def enter_serial_recovery(device_config):
    usb_port = require_usb_port(device_config)
    await wait_for_usb_port(usb_port, True, timeout_s=15.0)

    async def trigger(client: SMPClient):
        try:
            response = await client.request(Execute(argv=["boot", "start"]), timeout_s=5.0)
            if error(response):
                pytest.fail(f"Failed to enter serial recovery: {response}")
        except TimeoutError:
            pass

    await with_serial_client(usb_port, trigger)
    await wait_for_usb_port(usb_port, False, timeout_s=10.0)
    await wait_for_usb_port(usb_port, True, timeout_s=30.0)

