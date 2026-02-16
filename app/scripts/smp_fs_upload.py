#!/usr/bin/env -S PYTHONPATH= python3
"""Upload files to ZSWatch via BLE SMP (MCUmgr) filesystem commands.

Uses only bleak + cbor2 — no smpclient dependency.

Usage:
    python3 smp_fs_upload.py <local_file> <remote_path> [--device <name_or_addr>]

Example:
    python3 smp_fs_upload.py app/build_dbg_dk/app/llext/about_ext.llext /lvgl_lfs/apps/about_ext/app.llext
"""

import asyncio
import argparse
import os
import struct
import sys

import cbor2
from bleak import BleakClient, BleakScanner

SMP_SERVICE_UUID = "8d53dc1d-1db7-4cd3-868b-8a527460aa84"
SMP_CHAR_UUID = "da2e7828-fbce-4e01-ae9e-261174997c48"

# SMP header: op(1) flags(1) len(2) group(2) seq(1) id(1)
SMP_HDR_FMT = "!BBHHBB"
SMP_HDR_SIZE = 8

# SMP groups and commands
SMP_GROUP_FS = 8
SMP_OP_WRITE = 2
SMP_OP_READ = 0
SMP_ID_FS_UPLOAD = 0  # file upload/download uses id=0

# Chunk size — BLE MTU is typically 252 max for ATT, leave room for SMP header + CBOR overhead
CHUNK_SIZE = 128


def build_smp_packet(op, group, cmd_id, seq, payload_cbor):
    """Build a complete SMP packet with header + CBOR payload."""
    hdr = struct.pack(SMP_HDR_FMT, op, 0, len(payload_cbor), group, seq, cmd_id)
    return hdr + payload_cbor


def parse_smp_response(data):
    """Parse SMP response: header + CBOR body."""
    if len(data) < SMP_HDR_SIZE:
        raise ValueError(f"Response too short: {len(data)} bytes")
    op, flags, length, group, seq, cmd_id = struct.unpack(SMP_HDR_FMT, data[:SMP_HDR_SIZE])
    body = cbor2.loads(data[SMP_HDR_SIZE:SMP_HDR_SIZE + length])
    return {"op": op, "group": group, "seq": seq, "id": cmd_id, "body": body}


class SMPFileUploader:
    def __init__(self, client):
        self.client = client
        self.seq = 0
        self.response_event = asyncio.Event()
        self.response_data = bytearray()

    def _notification_handler(self, sender, data):
        self.response_data.extend(data)
        # Check if we have a complete SMP packet
        if len(self.response_data) >= SMP_HDR_SIZE:
            _, _, length, _, _, _ = struct.unpack(SMP_HDR_FMT, self.response_data[:SMP_HDR_SIZE])
            if len(self.response_data) >= SMP_HDR_SIZE + length:
                self.response_event.set()

    async def start(self):
        await self.client.start_notify(SMP_CHAR_UUID, self._notification_handler)

    async def stop(self):
        try:
            await self.client.stop_notify(SMP_CHAR_UUID)
        except Exception:
            pass

    async def _send_and_receive(self, packet, timeout=10.0):
        self.response_data = bytearray()
        self.response_event.clear()
        await self.client.write_gatt_char(SMP_CHAR_UUID, packet, response=False)
        try:
            await asyncio.wait_for(self.response_event.wait(), timeout=timeout)
        except asyncio.TimeoutError:
            raise TimeoutError("SMP response timeout")
        return parse_smp_response(bytes(self.response_data))

    async def upload_file(self, local_data, remote_path):
        """Upload bytes to a remote filesystem path."""
        total = len(local_data)
        offset = 0
        first = True

        while offset < total:
            chunk_end = min(offset + CHUNK_SIZE, total)
            chunk = local_data[offset:chunk_end]

            payload = {"off": offset, "data": chunk}
            if first:
                payload["name"] = remote_path
                payload["len"] = total
                first = False

            payload_cbor = cbor2.dumps(payload)
            self.seq = (self.seq + 1) % 256
            pkt = build_smp_packet(SMP_OP_WRITE, SMP_GROUP_FS, SMP_ID_FS_UPLOAD, self.seq, payload_cbor)

            resp = await self._send_and_receive(pkt)
            body = resp["body"]

            rc = body.get("rc", -1)
            if rc != 0:
                raise RuntimeError(f"SMP upload error at offset {offset}: rc={rc}")

            new_off = body.get("off", offset + len(chunk))
            offset = new_off

            pct = int(offset * 100 / total) if total > 0 else 100
            print(f"\r  {offset}/{total} bytes ({pct}%)", end="", flush=True)

        print()  # newline after progress


async def find_device(name_filter="ZSWatch"):
    """Scan for a BLE device matching the name. Returns BLEDevice object."""
    print(f"Scanning for '{name_filter}' BLE devices (10s)...")
    devices = await BleakScanner.discover(timeout=10.0, return_adv=True)

    matches = []
    for d, adv in devices.values():
        dev_name = adv.local_name or d.name or ""
        if name_filter.lower() in dev_name.lower():
            print(f"  Found: {d.address} - {dev_name} (RSSI: {adv.rssi})")
            matches.append((d, adv))

    if not matches:
        return None

    # Pick strongest signal
    matches.sort(key=lambda x: x[1].rssi or -999, reverse=True)
    best = matches[0][0]
    print(f"  Using: {best.address}")
    return best  # Return BLEDevice object, not just address


async def main():
    parser = argparse.ArgumentParser(description="Upload file to ZSWatch via BLE SMP")
    parser.add_argument("local_file", help="Local file to upload")
    parser.add_argument("remote_path", help="Remote filesystem path (e.g. /lvgl_lfs/apps/about_ext/app.llext)")
    parser.add_argument("--device", default="ZSWatch", help="BLE device name to scan for (default: ZSWatch)")
    parser.add_argument("--address", default=None, help="BLE device address (skip scan)")
    args = parser.parse_args()

    if not os.path.exists(args.local_file):
        print(f"Error: File not found: {args.local_file}")
        sys.exit(1)

    with open(args.local_file, "rb") as f:
        file_data = f.read()
    print(f"File: {args.local_file} ({len(file_data)} bytes)")
    print(f"Remote: {args.remote_path}")

    # Find device
    device = args.address  # Can be address string or None
    if not device:
        device = await find_device(args.device)
        if not device:
            print("No matching device found!")
            sys.exit(1)

    dev_label = device.address if hasattr(device, 'address') else device
    print(f"\nConnecting to {dev_label}...")
    async with BleakClient(device, timeout=15.0) as client:
        print(f"Connected! MTU={client.mtu_size}")

        # Verify SMP service
        smp_found = any(SMP_SERVICE_UUID.lower() in str(s.uuid).lower() for s in client.services)
        if not smp_found:
            print("ERROR: SMP service not found on device!")
            print("Available services:")
            for s in client.services:
                print(f"  {s.uuid}: {s.description}")
            sys.exit(1)
        print("SMP service found!")

        uploader = SMPFileUploader(client)
        await uploader.start()

        print(f"\nUploading to {args.remote_path}...")
        await uploader.upload_file(file_data, args.remote_path)
        print("Upload complete!")

        await uploader.stop()


if __name__ == "__main__":
    asyncio.run(main())
