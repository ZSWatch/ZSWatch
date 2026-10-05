"""Real receipt code and Zephyr CRC with POSIX filesystem fault injection."""
import subprocess
import tempfile
import unittest
from pathlib import Path


class VoiceResultTests(unittest.TestCase):
    def test_durable_acceptance_and_deduplication(self):
        here = Path(__file__).resolve().parent
        app = here.parents[1]
        with tempfile.TemporaryDirectory() as directory:
            binary = Path(directory) / 'receipt-test'
            build = subprocess.run([
                'cc', '-std=c11', '-Wall', '-Wextra', '-Werror',
                '-I', str(here / 'stubs'), '-I', str(app / 'src'),
                str(app / 'src/ble/gadgetbridge/ble_voice_result_receipt.c'),
                str(app.parent / 'zephyr/subsys/crc/crc32_sw.c'),
                str(here / 'test_voice_result.c'), '-o', str(binary),
            ], capture_output=True, text=True, timeout=30)
            self.assertEqual(build.returncode, 0, build.stderr)
            subprocess.run([str(binary)], cwd=directory, check=True,
                           capture_output=True, timeout=10)


if __name__ == '__main__':
    unittest.main()
