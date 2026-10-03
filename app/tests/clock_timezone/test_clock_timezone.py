"""Host-only UTC/fractional timezone regression; no hardware or serial access."""
import subprocess
import tempfile
import unittest
from pathlib import Path


class ClockTimezoneTests(unittest.TestCase):
    def test_posix_timezone_and_local_clock_offsets(self):
        here = Path(__file__).resolve().parent
        with tempfile.TemporaryDirectory() as directory:
            binary = Path(directory) / 'clock-timezone-test'
            result = subprocess.run(['cc', '-std=c11', '-Wall', '-Wextra', '-Werror',
                            '-I', str(here.parents[1] / 'src'),
                            str(here / 'test_clock_timezone.c'), '-o', str(binary)],
                           capture_output=True, text=True, timeout=30)
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(binary)], check=True, capture_output=True, timeout=10)


if __name__ == '__main__':
    unittest.main()
