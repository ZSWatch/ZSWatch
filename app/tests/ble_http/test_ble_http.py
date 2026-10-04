"""Actual HTTP controller and parser with deterministic Zephyr/BLE adapters."""
import subprocess
import tempfile
import unittest
from pathlib import Path


class BleHttpTests(unittest.TestCase):
    def test_terminal_callbacks_and_json_response_contract(self):
        here = Path(__file__).resolve().parent
        app = here.parents[1]
        cjson = app.parent / 'modules/lib/cjson'
        parser = (app / 'src/ble/gadgetbridge/ble_gadgetbridge.c').read_text()
        begin = parser.index('static int parse_httpstate(char *data, int len)')
        end = parser.index('\nstatic int parse_gps_data', begin)
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / 'http_parser.inc').write_text(parser[begin:end])
            binary = root / 'ble-http-test'
            result = subprocess.run([
                'cc', '-std=c11', '-Wall', '-Wextra', '-Werror',
                '-fsanitize=address,undefined', '-fno-omit-frame-pointer',
                '-I', str(here / 'stubs'), '-I', str(app / 'src'),
                '-I', str(cjson), '-I', directory,
                str(here / 'test_ble_http.c'), str(cjson / 'cJSON.c'),
                '-lm', '-o', str(binary)], capture_output=True, text=True, timeout=30)
            self.assertEqual(result.returncode, 0, result.stderr)
            run = subprocess.run([str(binary)], capture_output=True, text=True, timeout=10)
            self.assertEqual(run.returncode, 0, run.stderr)


if __name__ == '__main__':
    unittest.main()
