#!/usr/bin/env python3
"""Upload an LLEXT app to ZSWatch via BLE MCUmgr filesystem commands.

Usage:
    python3 upload_llext_app.py <llext_file> [app_name]

Example:
    python3 upload_llext_app.py app/build_dbg_dk/app/llext/about_ext.llext about_ext

This will upload:
  - about_ext.llext  -> /lvgl_lfs/apps/about_ext/app.llext
  - manifest.json    -> /lvgl_lfs/apps/about_ext/manifest.json
"""

import asyncio
import json
import os
import sys

from smpclient import SMPClient
from smpclient.transport.ble import SMPBLETransport


APPS_BASE_PATH = "/lvgl_lfs/apps"
SMP_SERVICE_UUID = "8d53dc1d-1db7-4cd3-868b-8a527460aa84"


async def find_zswatch_with_smp():
    """Scan for ZSWatch devices and return the address of one with SMP service."""
    from bleak import BleakScanner, BleakClient

    print("Scanning for ZSWatch devices (10s)...")
    devices = await BleakScanner.discover(timeout=10.0, return_adv=True)

    zswatch_list = []
    for d, adv in devices.values():
        name = adv.local_name or d.name or ""
        if "zswatch" in name.lower():
            print(f"  Found: {d.address} - {name} (RSSI: {adv.rssi})")
            zswatch_list.append((d, adv))

    if not zswatch_list:
        print("No ZSWatch devices found!")
        return None

    # Sort by signal strength (closest first)
    zswatch_list.sort(key=lambda x: x[1].rssi or -999, reverse=True)

    for device, adv in zswatch_list:
        print(f"\nChecking {device.address} for SMP service...")
        try:
            client = BleakClient(device, timeout=15.0)
            await client.connect()
            has_smp = any(
                SMP_SERVICE_UUID in s.uuid.lower()
                for s in client.services
            )
            await client.disconnect()
            if has_smp:
                print(f"  SMP service found! Using {device.address}")
                return device.address
            else:
                print(f"  No SMP service (wrong device or firmware)")
        except Exception as e:
            print(f"  Connection failed: {e}")

    return None


async def upload_file_data(client, data, remote_path, label="file"):
    """Upload bytes to a remote path."""
    print(f"  Uploading {label} -> {remote_path} ({len(data)} bytes)")
    last_pct = -1
    async for offset in client.upload_file(data, remote_path, timeout_s=10.0):
        pct = int((offset / len(data)) * 100) if len(data) > 0 else 100
        if pct != last_pct and pct % 10 == 0:
            print(f"    {pct}%", end=" ", flush=True)
            last_pct = pct
    print(f"\n  Done: {remote_path}")


async def main():
    if len(sys.argv) < 2:
        print("Usage: upload_llext_app.py <llext_file> [app_name]")
        print("Example: upload_llext_app.py app/build_dbg_dk/app/llext/about_ext.llext")
        sys.exit(1)

    llext_path = sys.argv[1]
    app_name = sys.argv[2] if len(sys.argv) > 2 else os.path.splitext(os.path.basename(llext_path))[0]

    if not os.path.exists(llext_path):
        print(f"Error: LLEXT file not found: {llext_path}")
        sys.exit(1)

    # Find the correct device
    address = await find_zswatch_with_smp()
    if address is None:
        print("Could not find a ZSWatch with SMP service. Is the firmware flashed?")
        sys.exit(1)

    # Create manifest
    manifest = {
        "name": app_name,
        "version": "1.0.0",
        "entry_symbol": "app_entry",
    }
    manifest_json = json.dumps(manifest, indent=2)
    manifest_bytes = manifest_json.encode("utf-8")

    remote_dir = f"{APPS_BASE_PATH}/{app_name}"
    remote_llext = f"{remote_dir}/app.llext"
    remote_manifest = f"{remote_dir}/manifest.json"

    print(f"\nConnecting to {address} via SMP BLE...")
    async with SMPClient(SMPBLETransport(), address) as client:
        print("Connected to SMP!")

        print(f"\nUploading LLEXT app '{app_name}' to {remote_dir}/")

        # Upload manifest first (small)
        await upload_file_data(client, manifest_bytes, remote_manifest, "manifest.json")

        # Upload LLEXT binary
        with open(llext_path, "rb") as f:
            llext_data = f.read()
        await upload_file_data(client, llext_data, remote_llext, os.path.basename(llext_path))

        print(f"\nUpload complete! Reboot the watch to load the app:")
        print(f"  nrfjprog --reset")


if __name__ == "__main__":
    asyncio.run(main())
