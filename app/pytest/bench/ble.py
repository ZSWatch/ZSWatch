"""One BLE connection to the watch with both SMP (shell) and NUS (companion protocol).

Library use:

    async with WatchLink(device["mac"]) as watch:
        print(await watch.shell("power status"))
        await watch.send_gb({"t": "voice_memo", "action": "list"})
        reply = await watch.wait_for_record(lambda r: r.get("action") == "list_result")

Command line, from app/pytest:

    python -m bench.ble shell "hr status" "battery"
    python -m bench.ble smp off
    python -m bench.ble gb '{"t":"find","n":true}'
"""

import argparse
import asyncio
import json
import logging
import re
import shlex
import subprocess
from typing import Callable

from bleak import BleakClient, BleakScanner
from smpclient import SMPClient
from smpclient.requests.shell_management import Execute
from smpclient.transport.ble import SMP_CHARACTERISTIC_UUID, SMPBLETransport

from bench.devices import get_device, require

log = logging.getLogger(__name__)

NUS_RX = "6e400002-b5a3-f393-e0a9-e50e24dcca9e"
NUS_TX = "6e400003-b5a3-f393-e0a9-e50e24dcca9e"


def gb_packet(obj: dict) -> bytes:
    """Encode a Gadgetbridge style command as the watch expects it on NUS."""
    return ("GB(" + json.dumps(obj, separators=(",", ":")) + ")\n").encode()


def json_records(data: bytes) -> list[dict]:
    """Extract every JSON object from a NUS byte stream, ignoring framing and logs."""
    text = data.decode(errors="replace")
    decoder = json.JSONDecoder()
    records = []
    for match in re.finditer(r"\{", text):
        try:
            obj, _ = decoder.raw_decode(text[match.start():])
        except ValueError:
            continue
        if isinstance(obj, dict):
            records.append(obj)
    return records


async def _retry(action, attempts: int = 3, delay_s: float = 1.0):
    for attempt in range(attempts):
        try:
            return await action()
        except Exception as exc:
            if attempt == attempts - 1:
                raise
            log.warning("BLE attempt %d failed: %s", attempt + 1, exc)
            await asyncio.sleep(delay_s * (attempt + 1))


async def set_smp(address: str, enabled: bool) -> None:
    """Enable or disable the SMP service over a short NUS-only connection.

    SMP is registered at runtime, so the GATT database changes; a new connection
    is needed afterwards to see the SMP characteristic.
    """
    async def once():
        # BlueZ only connects to devices it has seen; scan first.
        device = await BleakScanner.find_device_by_address(address, timeout=20)
        if device is None:
            raise RuntimeError(f"No advertisement from {address}")
        async with BleakClient(device, timeout=45) as client:
            await client.write_gatt_char(NUS_RX, gb_packet({"t": "smp", "status": enabled}), response=True)
            await asyncio.sleep(0.5)

    await _retry(once)
    log.info("SMP %s", "enabled" if enabled else "disabled")


def _forget_cached_services(address: str) -> None:
    """Drop BlueZ's cached GATT database for an unpaired watch.

    BlueZ keeps the services of an unpaired device for a while, so a
    reconnect right after enabling SMP can still miss the SMP service.
    Paired devices are left alone; removing them would drop the bond.
    """
    try:
        info = subprocess.run(["bluetoothctl", "info", address], capture_output=True, text=True).stdout
        if "Paired: no" in info:
            subprocess.run(["bluetoothctl", "remove", address], capture_output=True)
    except FileNotFoundError:
        pass  # Not BlueZ


class _SmpAndNusTransport(SMPBLETransport):
    """SMPBLETransport that discovers all services, so NUS is usable on the same link."""

    async def connect(self, address: str, timeout_s: float) -> None:
        device = await BleakScanner.find_device_by_address(address, timeout=timeout_s)
        if device is None:
            raise RuntimeError(f"No advertisement from {address}")
        self._client = BleakClient(device, disconnected_callback=self._set_disconnected_event)
        await self._client.connect()
        self._disconnected_event.clear()
        self._smp_characteristic = self._client.services.get_characteristic(SMP_CHARACTERISTIC_UUID)
        if self._smp_characteristic is None:
            await self._client.disconnect()
            _forget_cached_services(address)
            raise RuntimeError("SMP characteristic missing; is SMP enabled on the watch?")
        await self._client._backend._acquire_mtu()
        self._max_write_without_response_size = self._client.mtu_size - 3
        await self._client.start_notify(SMP_CHARACTERISTIC_UUID, self._notify_callback)


class WatchLink:
    """Async context manager holding SMP and NUS on one connection.

    enable_smp turns SMP on first. disable_smp_on_exit turns it off again so the
    watch returns to its normal connection parameters, e.g. before a current capture.
    """

    def __init__(self, address: str, enable_smp: bool = True, disable_smp_on_exit: bool = False,
                 timeout_s: float = 15.0):
        self.address = address
        self.enable_smp = enable_smp
        self.disable_smp_on_exit = disable_smp_on_exit
        self.timeout_s = timeout_s
        self.incoming = bytearray()
        self._transport: _SmpAndNusTransport | None = None
        self._smp: SMPClient | None = None

    async def __aenter__(self) -> "WatchLink":
        if self.enable_smp:
            await set_smp(self.address, True)

        async def connect():
            self._transport = _SmpAndNusTransport()
            self._smp = SMPClient(self._transport, self.address)
            await self._smp.connect(self.timeout_s)

        await _retry(connect)
        await self.nus.start_notify(NUS_TX, lambda _sender, data: self.incoming.extend(data))
        return self

    async def __aexit__(self, *exc) -> None:
        try:
            await self._smp.disconnect()
        finally:
            if self.disable_smp_on_exit:
                await set_smp(self.address, False)

    @property
    def nus(self) -> BleakClient:
        return self._transport._client

    async def shell(self, command: str | list[str], timeout_s: float = 20.0, check: bool = True) -> str:
        """Run a Zephyr shell command over SMP and return its output."""
        argv = shlex.split(command) if isinstance(command, str) else command
        response = await self._smp.request(Execute(argv=argv), timeout_s=timeout_s)
        ret = getattr(response, "ret", None)
        output = getattr(response, "o", str(response))
        if check and ret != 0:
            raise RuntimeError(f"Shell command {argv} failed ({ret}): {output}")
        return output

    async def send_raw(self, data: bytes | str) -> None:
        await self.nus.write_gatt_char(NUS_RX, data.encode() if isinstance(data, str) else data, response=True)

    async def send_gb(self, obj: dict) -> None:
        await self.send_raw(gb_packet(obj))

    def records(self, since: int = 0) -> list[dict]:
        return json_records(bytes(self.incoming[since:]))

    async def wait_for_record(self, predicate: Callable[[dict], bool], timeout_s: float = 5.0,
                              since: int = 0) -> dict:
        """Wait for a JSON object from the watch on NUS that matches predicate."""
        loop = asyncio.get_running_loop()
        deadline = loop.time() + timeout_s
        while True:
            found = [r for r in self.records(since) if predicate(r)]
            if found:
                return found[-1]
            if loop.time() >= deadline:
                raise TimeoutError("No matching NUS record from the watch")
            await asyncio.sleep(0.1)


async def _main(args) -> None:
    address = require(get_device(args.device), "mac")
    if args.action == "smp":
        await set_smp(address, args.values[0] == "on")
        return
    async with WatchLink(address, disable_smp_on_exit=not args.keep_smp) as watch:
        for value in args.values:
            if args.action == "shell":
                print(f"$ {value}\n{await watch.shell(value, check=False)}", flush=True)
            else:
                await watch.send_gb(json.loads(value))
        if args.action == "gb":
            await asyncio.sleep(args.listen)
            for record in watch.records():
                print(json.dumps(record))


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--device", help="Entry in devices.yaml (default: ZSW_DEVICE or the only bench entry)")
    parser.add_argument("--keep-smp", action="store_true", help="Leave SMP enabled when done")
    parser.add_argument("--listen", type=float, default=2.0, help="Seconds to print NUS replies after gb")
    parser.add_argument("action", choices=["shell", "gb", "smp"])
    parser.add_argument("values", nargs="+", help="Shell commands, GB JSON objects, or on/off for smp")
    args = parser.parse_args()
    if args.action == "smp" and args.values not in (["on"], ["off"]):
        parser.error("smp takes on or off")
    logging.basicConfig(level=logging.INFO, format="%(message)s")
    asyncio.run(_main(args))


if __name__ == "__main__":
    main()
