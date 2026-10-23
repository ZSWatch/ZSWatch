"""Offline DFU checks. Run with python -m unittest discover -s bench/tests."""

import hashlib
import json
import struct
import tempfile
import unittest
import zipfile
from pathlib import Path
from types import SimpleNamespace
from unittest.mock import AsyncMock, patch

from bench import ble, dfu
from smp.os_management import ResetWriteResponse
from smpclient.requests.image_management import ImageStatesWrite


def image(payload=b"firmware"):
    header = bytearray(32)
    struct.pack_into("<I", header, 0, dfu.IMAGE_MAGIC)
    struct.pack_into("<HHI", header, 8, 32, 0, len(payload))
    body = bytes(header) + payload
    digest = hashlib.sha256(body).digest()
    return body + struct.pack("<HHBBH", dfu.TLV_INFO_MAGIC, 40, dfu.TLV_SHA256, 0, 32) + digest


def states(files, active=False, confirmed=False):
    return SimpleNamespace(images=[SimpleNamespace(
        image=i, hash=dfu.image_hash(data), active=active, confirmed=confirmed, pending=False
    ) for i, data in files.items()])


class PackageTests(unittest.TestCase):
    def test_hash_rejects_corrupt_image_and_truncated_tlv(self):
        data = image()
        self.assertEqual(dfu.image_hash(data), hashlib.sha256(data[:40]).digest())
        corrupt = bytearray(data)
        corrupt[32] ^= 1
        with self.assertRaises(ValueError):
            dfu.image_hash(corrupt)
        truncated_table = bytearray(data)
        struct.pack_into("<H", truncated_table, 42, 8)
        with self.assertRaises(ValueError):
            dfu.image_hash(truncated_table)

    def test_manifest_uses_indices_without_board_names(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "dfu.zip"
            with zipfile.ZipFile(path, "w") as archive:
                archive.writestr("manifest.json", json.dumps({"files": [
                    {"image_index": str(i), "file": f"{i}.bin"} for i in (2, 1, 0)
                ]}))
                for i in (0, 1, 2):
                    archive.writestr(f"{i}.bin", image(bytes([i])))
            self.assertEqual(set(dfu.load_images(path, net=False)), {0, 2})
            self.assertEqual(set(dfu.load_images(path, net=True)), {0, 1, 2})


class InstallTests(unittest.IsolatedAsyncioTestCase):
    def setUp(self):
        self.files = {0: image(b"internal"), 2: image(b"external")}
        self.watch = SimpleNamespace(shell=AsyncMock(return_value="Recording: no"),
                                     smp=SimpleNamespace(request=AsyncMock(return_value=ResetWriteResponse())))
        self.connect = self.enter_patch("connect", AsyncMock(return_value=self.watch))
        self.close = self.enter_patch("close", AsyncMock())
        self.upload = self.enter_patch("upload", AsyncMock())
        self.request = self.enter_patch("request", AsyncMock())
        self.enterContext(patch.object(dfu.asyncio, "sleep", new=AsyncMock()))

    def enter_patch(self, name, replacement):
        return self.enterContext(patch.object(dfu, name, new=replacement))

    def writes(self):
        return [call.args[1] for call in self.request.call_args_list
                if isinstance(call.args[1], ImageStatesWrite)]

    async def test_running_images_are_skipped_without_upload_or_reset(self):
        self.request.return_value = states(self.files, active=True, confirmed=True)
        await dfu.install("watch", self.files, confirm=True)
        self.upload.assert_not_awaited()
        self.watch.smp.request.assert_not_awaited()
        self.assertEqual(self.writes(), [])
        self.close.assert_awaited_once()

    async def test_recording_or_unknown_status_blocks_update(self):
        for status in ("Recording: yes", "command not found"):
            self.watch.shell.return_value = status
            with self.assertRaisesRegex(RuntimeError, "not updating"):
                await dfu.install("watch", self.files, confirm=True)
        self.request.assert_not_awaited()
        self.upload.assert_not_awaited()

    async def test_rerun_finishes_confirmation_without_upload_or_reboot(self):
        self.request.side_effect = [states(self.files, active=True), None, None,
                                    states(self.files, active=True, confirmed=True)]
        await dfu.install("watch", self.files, confirm=True)
        self.upload.assert_not_awaited()
        self.watch.smp.request.assert_not_awaited()
        self.assertEqual([write.confirm for write in self.writes()], [True, True])

    async def test_failed_confirmation_is_reported(self):
        self.request.side_effect = [states(self.files, active=True), None, None,
                                    states(self.files, active=True)]
        with self.assertRaisesRegex(RuntimeError, "was not confirmed"):
            await dfu.install("watch", self.files, confirm=True)

    async def test_other_pending_build_blocks_update(self):
        pending = states({0: image(b"other")})
        pending.images[0].pending = True
        self.request.return_value = pending
        with self.assertRaisesRegex(RuntimeError, "already pending"):
            await dfu.install("watch", self.files, confirm=True)
        self.upload.assert_not_awaited()

    async def test_missing_upload_does_not_mark_any_image_pending(self):
        self.request.side_effect = [states({}), states({0: self.files[0]})]
        with self.assertRaisesRegex(RuntimeError, "not found"):
            await dfu.install("watch", self.files, confirm=True)
        self.assertEqual(self.writes(), [])
        self.watch.smp.request.assert_not_awaited()

    async def test_partial_boot_does_not_confirm_any_image(self):
        self.request.side_effect = [states({}), states(self.files), None, None,
                                    states({0: self.files[0]}, active=True)]
        with self.assertRaisesRegex(RuntimeError, "did not boot"):
            await dfu.install("watch", self.files, confirm=True)
        self.assertEqual([write.confirm for write in self.writes()], [False, False])

    async def test_upload_reconnects_and_confirms_all_images_after_boot(self):
        self.upload.side_effect = [TimeoutError("link lost"), None, None]
        self.request.side_effect = [states({}), states(self.files), None, None,
                                    states(self.files, active=True), None, None,
                                    states(self.files, active=True, confirmed=True)]
        await dfu.install("watch", self.files, confirm=True)
        self.assertEqual(self.upload.await_count, 3)
        self.assertEqual(self.connect.await_count, 3)  # initial, retry, after reboot
        self.assertEqual([write.confirm for write in self.writes()], [False, False, True, True])

    async def test_no_confirm_leaves_new_images_unconfirmed(self):
        self.request.side_effect = [states({}), states(self.files), None, None,
                                    states(self.files, active=True)]
        await dfu.install("watch", self.files, confirm=False)
        self.assertEqual([write.confirm for write in self.writes()], [False, False])


class BleTests(unittest.IsolatedAsyncioTestCase):
    async def test_failed_nus_subscription_disconnects_before_retry(self):
        transport = SimpleNamespace(_client=SimpleNamespace(
            start_notify=AsyncMock(side_effect=RuntimeError("subscription failed"))),
            _best_effort_disconnect=AsyncMock())
        with patch.object(ble, "_SmpAndNusTransport", return_value=transport), \
                patch.object(ble, "SMPClient", return_value=SimpleNamespace(connect=AsyncMock())), \
                patch.object(ble.asyncio, "sleep", new=AsyncMock()):
            with self.assertRaisesRegex(RuntimeError, "subscription failed"):
                await ble.WatchLink("watch", enable_smp=False).__aenter__()
        self.assertEqual(transport._best_effort_disconnect.await_count, 3)

    async def test_transport_uses_macos_write_size_and_bluez_mtu(self):
        for platform in ("CoreBluetooth", "BleakClientBlueZDBus"):
            backend = type(platform, (), {"_acquire_mtu": AsyncMock()})()
            characteristic = SimpleNamespace(max_write_without_response_size=244)
            client = SimpleNamespace(_backend=backend, connect=AsyncMock(), start_notify=AsyncMock(),
                                     mtu_size=247, services=SimpleNamespace(
                                         get_characteristic=lambda _: characteristic))
            with patch.object(ble, "find_watch", new=AsyncMock()), \
                    patch.object(ble, "BleakClient", return_value=client):
                transport = ble._SmpAndNusTransport()
                await transport.connect("watch", timeout_s=1)
                self.assertEqual(transport._max_write_without_response_size, 244)
            self.assertEqual(backend._acquire_mtu.await_count, int(platform == "BleakClientBlueZDBus"))


if __name__ == "__main__":
    unittest.main()
