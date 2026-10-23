"""One BLE connection to the watch with both SMP (shell) and NUS (companion protocol).

Library use:

    async with WatchLink(watch_target(device)) as watch:
        print(await watch.shell("power status"))
        await watch.send_gb({"t": "voice_memo", "action": "list"})
        reply = await watch.wait_for_record(lambda r: r.get("action") == "list_result")

Command line, from app/pytest:

    python -m bench.ble shell "hr status" "battery"
    python -m bench.ble smp off
    python -m bench.ble gb '{"t":"find","n":true}'
    python -m bench.ble scan
    python -m bench.ble pair
    python -m bench.ble logs

Linux finds the watch by MAC. macOS hides MAC addresses, so it looks for the
advertised name ZSWatch-XX instead, XX being the last byte of the MAC. Set
ble_name in devices.yaml to override. The watch takes one connection, so
disconnect the phone first. It only talks over an encrypted link: run pair
once and open the pairing window on the watch (Settings) while it waits.
"""

import argparse
import asyncio
import json
import logging
import re
import shlex
import subprocess
import sys
from typing import Callable

from bleak import BleakClient, BleakScanner
from bleak.backends.device import BLEDevice
from bleak.exc import BleakGATTProtocolError
from smpclient import SMPClient
from smpclient.requests.shell_management import Execute
from smpclient.transport.ble import SMP_CHARACTERISTIC_UUID, SMPBLETransport

from bench.devices import get_device, require

log = logging.getLogger(__name__)

NUS_RX = "6e400002-b5a3-f393-e0a9-e50e24dcca9e"
NUS_TX = "6e400003-b5a3-f393-e0a9-e50e24dcca9e"
MAC_RE = re.compile(r"^([0-9A-Fa-f]{2}:){5}[0-9A-Fa-f]{2}$")
LOG_FRAME_RE = re.compile(rb"<BLELOG>(.*?)</BLELOG>", re.S)


def watch_target(device: dict) -> str:
    """The MAC to connect to on Linux, or the advertised name on macOS."""
    if device.get("ble_name"):
        return device["ble_name"]
    mac = require(device, "mac")
    if sys.platform == "darwin":
        return f"ZSWatch-{mac[-2:].upper()}"
    return mac


async def find_watch(target: str, timeout_s: float = 20.0) -> BLEDevice:
    """Scan for the watch by MAC or by advertised name."""
    if MAC_RE.match(target):
        device = await BleakScanner.find_device_by_address(target, timeout=timeout_s)
    else:
        device = await BleakScanner.find_device_by_name(target, timeout=timeout_s)
    if device is None:
        raise RuntimeError(f"No advertisement from {target}")
    return device


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
        device = await find_watch(address)
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
    if sys.platform != "linux" or not MAC_RE.match(address):
        return
    try:
        info = subprocess.run(["bluetoothctl", "info", address], capture_output=True, text=True).stdout
        if "Paired: no" in info:
            subprocess.run(["bluetoothctl", "remove", address], capture_output=True)
    except FileNotFoundError:
        pass  # Not BlueZ


class _SmpAndNusTransport(SMPBLETransport):
    """SMPBLETransport that discovers all services, so NUS is usable on the same link."""

    async def _connect(self, address: str, timeout_s: float) -> None:
        device = await find_watch(address, timeout_s)
        self._client = BleakClient(device, disconnected_callback=self._set_disconnected_event)
        await self._client.connect()
        self._disconnected_event.clear()
        self._smp_characteristic = self._client.services.get_characteristic(SMP_CHARACTERISTIC_UUID)
        if self._smp_characteristic is None:
            _forget_cached_services(address)
            raise RuntimeError("SMP characteristic missing; is SMP enabled on the watch?")
        if self._bluez_backend(self._client._backend):
            await self._client._backend._acquire_mtu()
            self._max_write_without_response_size = self._client.mtu_size - 3
        else:
            self._max_write_without_response_size = self._smp_characteristic.max_write_without_response_size
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
            try:
                await self._smp.connect(self.timeout_s)
                await self.nus.start_notify(NUS_TX, lambda _sender, data: self.incoming.extend(data))
            except BaseException:
                await self._transport._best_effort_disconnect()
                raise

        await _retry(connect)
        return self

    async def __aexit__(self, *exc) -> None:
        try:
            await self._smp.disconnect()
        finally:
            if self.disable_smp_on_exit:
                await set_smp(self.address, False)

    @property
    def smp(self) -> SMPClient:
        return self._smp

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


async def scan(timeout_s: float = 15.0) -> None:
    """Print every advertising ZSWatch with the address this machine sees."""
    found = await BleakScanner.discover(timeout=timeout_s, return_adv=True)
    for device, adv in sorted(found.values(), key=lambda item: -item[1].rssi):
        name = adv.local_name or device.name or ""
        if name.startswith("ZSWatch"):
            print(f"{name:<14} {device.address}  rssi {adv.rssi}")


async def pair(address: str, timeout_s: float = 180.0) -> None:
    """Bond with the watch, retrying until its pairing window is opened."""
    loop = asyncio.get_running_loop()
    deadline = loop.time() + timeout_s
    log.info("Open the pairing window on the watch (Settings) now")
    while True:
        try:
            device = await find_watch(address)
            async with BleakClient(device, timeout=45) as client:
                try:
                    await client.pair()
                except NotImplementedError:
                    pass  # CoreBluetooth pairs on the first encrypted access instead.
                # Stay connected while macOS asks the user to accept the pairing.
                for _ in range(30):
                    try:
                        await client.write_gatt_char(NUS_RX, b"\n", response=True)
                        break
                    except BleakGATTProtocolError:
                        await asyncio.sleep(2)
                else:
                    raise RuntimeError("The watch still requires encryption")
            log.info("Paired with %s", address)
            return
        except Exception as exc:
            if loop.time() >= deadline:
                raise
            log.info("Not paired yet (%s), retrying", exc)
            await asyncio.sleep(3)


async def stream_logs(address: str) -> None:
    """Print the watch's BLE log backend output, reconnecting when the watch reboots."""
    while True:
        try:
            device = await find_watch(address, timeout_s=30)
        except RuntimeError as exc:
            log.warning("%s, still looking", exc)
            continue
        disconnected = asyncio.Event()
        buffer = bytearray()

        def on_data(_sender, data: bytearray) -> None:
            buffer.extend(data)
            end = 0
            for match in LOG_FRAME_RE.finditer(buffer):
                print(match.group(1).decode(errors="replace"), end="", flush=True)
                end = match.end()
            del buffer[:end]
            if len(buffer) > 4096:
                buffer.clear()

        try:
            async with BleakClient(device, timeout=45,
                                   disconnected_callback=lambda _client: disconnected.set()) as client:
                await client.start_notify(NUS_TX, on_data)
                await client.write_gatt_char(NUS_RX, gb_packet({"t": "log", "status": True}), response=True)
                log.info("Connected to %s, streaming logs (Ctrl+C to stop)", address)
                try:
                    await disconnected.wait()
                finally:
                    if client.is_connected:
                        try:
                            await asyncio.wait_for(client.write_gatt_char(
                                NUS_RX, gb_packet({"t": "log", "status": False}), response=True), timeout=3)
                        except Exception:
                            pass  # The watch may have disconnected during shutdown.
        except Exception as exc:
            log.warning("Connection failed: %s", exc)
            await asyncio.sleep(1)
            continue
        log.info("Disconnected, reconnecting")


async def _main(args) -> None:
    if args.action == "scan":
        await scan()
        return
    address = watch_target(get_device(args.device))
    if args.action == "logs":
        await stream_logs(address)
        return
    if args.action == "pair":
        await pair(address)
        return
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
    parser.add_argument("action", choices=["shell", "gb", "smp", "scan", "pair", "logs"])
    parser.add_argument("values", nargs="*", help="Shell commands, GB JSON objects, or on/off for smp")
    args = parser.parse_args()
    if args.action == "smp" and args.values not in (["on"], ["off"]):
        parser.error("smp takes on or off")
    if args.action in ("shell", "gb") and not args.values:
        parser.error(f"{args.action} needs at least one value")
    logging.basicConfig(level=logging.INFO, format="%(message)s")
    try:
        asyncio.run(_main(args))
    except KeyboardInterrupt:
        pass


if __name__ == "__main__":
    main()
