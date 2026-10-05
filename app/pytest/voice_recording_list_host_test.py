"""Compile the firmware encoder itself and validate its complete wire output."""
import ctypes
import json
from pathlib import Path
import subprocess
import tempfile
import unittest


class Entry(ctypes.Structure):
    _fields_ = [('filename', ctypes.c_char * 32), ('timestamp', ctypes.c_uint32),
                ('duration_ms', ctypes.c_uint32), ('size_bytes', ctypes.c_uint32)]


class RecordingListTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scratch = tempfile.TemporaryDirectory()
        source = Path(__file__).resolve().parents[1] / 'src'
        library = Path(cls.scratch.name) / 'encoder.so'
        subprocess.run(['cc', '-std=c11', '-Wall', '-Wextra', '-Werror', '-shared', '-fPIC',
                        '-I', str(source), str(source / 'ble/gadgetbridge/ble_voice_recording_list.c'),
                        '-o', str(library)], check=True)
        cls.lib = ctypes.CDLL(str(library))
        cls.lib.ble_voice_recording_list_encode.argtypes = [ctypes.POINTER(Entry), ctypes.c_size_t,
                                                         ctypes.c_char_p, ctypes.c_size_t]

    @classmethod
    def tearDownClass(cls):
        cls.scratch.cleanup()

    def encode(self, entries, capacity=6144):
        output = ctypes.create_string_buffer(capacity)
        result = self.lib.ble_voice_recording_list_encode(entries, len(entries), output, capacity)
        return result, output.value

    def test_maximum_list_is_complete_and_keeps_unsigned_values(self):
        entries = (Entry * 50)()
        for i, entry in enumerate(entries):
            entry.filename = (f'{i:02}_' + 'x' * 28).encode()
            entry.timestamp = entry.duration_ms = entry.size_bytes = 0xffffffff
        length, output = self.encode(entries)
        self.assertEqual(length, len(output))
        decoded = json.loads(output)
        self.assertEqual(decoded['result_ack_version'], 1)
        self.assertEqual(len(decoded['recordings']), 50)
        for i, entry in enumerate(decoded['recordings']):
            self.assertEqual(entry['filename'], entries[i].filename.decode())
            self.assertEqual(entry['duration_ms'], 0xffffffff)
            self.assertEqual(entry['size_bytes'], 0xffffffff)
            self.assertEqual(entry['timestamp'], 0xffffffff)

    def test_empty_list_and_escaping(self):
        self.assertEqual(json.loads(self.encode((Entry * 0)())[1])['recordings'], [])
        entries = (Entry * 1)()
        entries[0].filename = b'quote"back\\line\n\x01'
        self.assertEqual(json.loads(self.encode(entries)[1])['recordings'][0]['filename'],
                         entries[0].filename.decode())

    def test_truncated_output_and_unterminated_names_are_rejected(self):
        entries = (Entry * 1)()
        entries[0].filename = b'valid'
        self.assertLess(self.encode(entries, 90)[0], 0)
        entries[0].filename = b'x' * 32
        self.assertLess(self.encode(entries)[0], 0)
        self.assertLess(self.encode((Entry * 51)())[0], 0)


if __name__ == '__main__':
    unittest.main()
