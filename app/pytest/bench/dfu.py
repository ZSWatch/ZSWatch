"""Install a dfu_application.zip on a watch over BLE and confirm it.

Skips images the watch already runs, uploads the rest, marks them pending,
reboots through SMP, reconnects and confirms the images that booted. The
network core image is left out unless --net is given. Disconnect the phone
first: the watch takes one connection at a time.

    python -m bench.dfu ../build/dfu_application.zip
    python -m bench.dfu --net --no-confirm ../build/dfu_application.zip
"""

import argparse
import asyncio
import hashlib
import json
import logging
import struct
import time
import zipfile
from pathlib import Path

from smpclient.generics import error
from smpclient.requests.image_management import ImageStatesRead, ImageStatesWrite
from smpclient.requests.os_management import ResetWrite

from bench.ble import WatchLink, watch_target
from bench.devices import get_device

log = logging.getLogger(__name__)

IMAGE_MAGIC = 0x96F3B83D
TLV_INFO_MAGIC = 0x6907
TLV_SHA256 = 0x10
UPLOAD_ATTEMPTS = 5
REBOOT_TIMEOUT_S = 240


def image_hash(data: bytes) -> bytes:
    """Return the MCUboot SHA-256 TLV after checking that it matches the image."""
    if len(data) < 32 or struct.unpack_from("<I", data)[0] != IMAGE_MAGIC:
        raise ValueError("Not an MCUboot image")
    header, protected, size = struct.unpack_from("<HHI", data, 8)
    end = header + size + protected
    if header < 32 or end + 4 > len(data):
        raise ValueError("Image header points outside the file")
    magic, total = struct.unpack_from("<HH", data, end)
    if magic != TLV_INFO_MAGIC or total < 4 or end + total > len(data):
        raise ValueError("Image TLV table missing or truncated")
    at = end + 4
    while at + 4 <= end + total:
        kind, _, length = struct.unpack_from("<BBH", data, at)
        at += 4
        if at + length > end + total:
            raise ValueError("Image TLV entry points outside the table")
        if kind == TLV_SHA256:
            digest = data[at:at + length]
            if length != 32 or hashlib.sha256(data[:end]).digest() != digest:
                raise ValueError("Image SHA-256 TLV does not match its contents")
            return digest
        at += length
    raise ValueError("Image has no SHA-256 TLV")


def load_images(path: Path, net: bool) -> dict[int, bytes]:
    """Read the images from the zip by manifest image_index."""
    with zipfile.ZipFile(path) as archive:
        manifest = json.loads(archive.read("manifest.json"))
        files = {}
        for entry in manifest["files"]:
            index = int(entry["image_index"])
            if index not in (0, 1, 2):
                raise ValueError(f"Unsupported image_index {index}")
            if index == 1 and not net:
                continue
            if index in files:
                raise ValueError(f"Duplicate image_index {index} in manifest")
            files[index] = archive.read(entry["file"])
        if not files:
            raise ValueError("No firmware images selected from the manifest")
        return files


def has_image(states, index: int, digest: bytes, **flags) -> bool:
    return any(s.image == index and s.hash == digest and
               all(getattr(s, k) == v for k, v in flags.items()) for s in states.images)


async def request(watch: WatchLink, req, timeout_s: float = 10.0):
    response = await watch.smp.request(req, timeout_s=timeout_s)
    if error(response):
        raise RuntimeError(f"{type(req).__name__} failed: {response}")
    return response


async def connect(target: str, timeout_s: float = 60.0) -> WatchLink:
    """Open a WatchLink, retrying until timeout_s, e.g. while MCUboot swaps images."""
    async def retry():
        while True:
            watch = WatchLink(target)
            try:
                return await watch.__aenter__()
            except Exception as exc:
                log.info("Waiting for %s: %s", target, exc)
                await asyncio.sleep(5)

    try:
        return await asyncio.wait_for(retry(), timeout=timeout_s)
    except asyncio.TimeoutError as exc:
        raise TimeoutError(f"No SMP connection to {target} within {timeout_s:.0f} s") from exc


async def close(watch: WatchLink) -> None:
    try:
        await watch.__aexit__(None, None, None)
    except Exception:
        pass  # The link may already be gone.


async def upload(watch: WatchLink, index: int, data: bytes) -> None:
    start = time.monotonic()
    async for offset in watch.smp.upload(data, slot=index, upgrade=False, subsequent_timeout_s=12):
        rate = offset / 1024 / max(time.monotonic() - start, 0.001)
        print(f"\rImage {index}: {offset * 100 // len(data):3d}% {rate:5.1f} KB/s", end="", flush=True)
    print()


async def confirm_images(watch: WatchLink, hashes: dict[int, bytes], states) -> bool:
    """Confirm only after every selected image is running; verify the result."""
    for index, digest in hashes.items():
        if not has_image(states, index, digest, active=True):
            raise RuntimeError(f"Image {index} did not boot; MCUboot kept the previous image")
    unconfirmed = [i for i, h in hashes.items()
                   if not has_image(states, i, h, active=True, confirmed=True)]
    for index in unconfirmed:
        await request(watch, ImageStatesWrite(hash=hashes[index], confirm=True))
    if unconfirmed:
        states = await request(watch, ImageStatesRead())
        for index, digest in hashes.items():
            if not has_image(states, index, digest, active=True, confirmed=True):
                raise RuntimeError(f"Image {index} was not confirmed")
    return bool(unconfirmed)


async def install(target: str, files: dict[int, bytes], confirm: bool) -> None:
    hashes = {index: image_hash(data) for index, data in files.items()}

    watch = await connect(target)
    try:
        status = await watch.shell("voice_memo status")
        if "Recording: no" not in status:
            raise RuntimeError("Watch is recording or status is unavailable; not updating")
        states = await request(watch, ImageStatesRead())

        running = {i for i, h in hashes.items() if has_image(states, i, h, active=True)}
        todo = [i for i in sorted(files) if i not in running]
        for index in sorted(running):
            print(f"Image {index}: already running, skipped")
        stale = [s.image for s in states.images if s.pending and s.hash != hashes.get(s.image)]
        if stale:
            raise RuntimeError(f"Image(s) {stale} already pending with another build; reboot or confirm first")
        if not todo:
            if confirm and await confirm_images(watch, hashes, states):
                print("Confirmed running images")
            print("Nothing to install")
            return

        for index in todo:
            if has_image(states, index, hashes[index], active=False):
                print(f"Image {index}: already uploaded")
                continue
            for attempt in range(1, UPLOAD_ATTEMPTS + 1):
                try:
                    await upload(watch, index, files[index])
                    break
                except Exception as exc:
                    print()
                    if attempt == UPLOAD_ATTEMPTS:
                        raise
                    # The watch keeps the received part, so the next upload resumes.
                    log.warning("Upload of image %d failed (%s), reconnecting", index, exc)
                    await close(watch)
                    watch = await connect(target)

        states = await request(watch, ImageStatesRead())
        for index in todo:
            if not has_image(states, index, hashes[index]):
                raise RuntimeError(f"Image {index} not found on the watch after upload")
        for index in todo:
            await request(watch, ImageStatesWrite(hash=hashes[index], confirm=False))
        print("Images pending, rebooting")
        try:
            response = await watch.smp.request(ResetWrite(), timeout_s=5)
        except Exception:
            pass  # The watch may drop the link before it answers.
        else:
            if error(response):
                raise RuntimeError(f"Reset failed: {response}")
    finally:
        await close(watch)

    await asyncio.sleep(10)
    watch = await connect(target, REBOOT_TIMEOUT_S)
    try:
        states = await request(watch, ImageStatesRead())
        for index in hashes:
            if not has_image(states, index, hashes[index], active=True):
                raise RuntimeError(f"Image {index} did not boot; MCUboot kept the previous image")
        if confirm:
            await confirm_images(watch, hashes, states)
    finally:
        await close(watch)
    print("Booted and confirmed" if confirm else "Booted; not confirmed, the next reboot reverts it")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--device", help="Entry in devices.yaml (default: ZSW_DEVICE or the only bench entry)")
    parser.add_argument("--net", action="store_true", help="Also install the network core image")
    parser.add_argument("--no-confirm", action="store_true", help="Leave the new images unconfirmed")
    parser.add_argument("zip", type=Path, help="dfu_application.zip from the build directory")
    args = parser.parse_args()
    logging.basicConfig(level=logging.INFO, format="%(message)s")
    files = load_images(args.zip, args.net)
    asyncio.run(install(watch_target(get_device(args.device)), files, not args.no_confirm))


if __name__ == "__main__":
    main()
