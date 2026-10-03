"""Real Find controller/driver guard with deterministic time; no flash or BLE."""
import subprocess
import tempfile
import unittest
from pathlib import Path


class FindDeviceTests(unittest.TestCase):
    def test_timeout_disconnect_late_callbacks_and_motor_ownership(self):
        here = Path(__file__).resolve().parent
        source = here.parents[1] / 'src'
        driver = (source / 'drivers/zsw_vibration_motor.c').read_text()
        begin = driver.index('bool zsw_vibration_stop_find(void)\n{')
        end = driver.index('\nstatic void run_next_motor_state(', begin)
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / 'stop_find_impl.inc').write_text(driver[begin:end])
            binary = root / 'find-device-test'
            result = subprocess.run(['cc', '-std=c11', '-Wall', '-Wextra', '-Werror',
                                     '-I', str(here / 'stubs'), '-I', str(source),
                                     '-I', directory, str(here / 'test_find_device.c'),
                                     '-o', str(binary)], capture_output=True, text=True, timeout=30)
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(binary)], check=True, capture_output=True, timeout=10)


if __name__ == '__main__':
    unittest.main()
