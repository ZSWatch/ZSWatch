"""Compile the actual Weather UI update/error functions against deterministic LVGL adapters."""
import subprocess
import tempfile
import unittest
from pathlib import Path

class WeatherUiTests(unittest.TestCase):
    def test_success_replaces_failed_state(self):
        here = Path(__file__).resolve().parent
        source = (here.parents[1] / 'src/applications/weather/weather_ui.c').read_text()
        update = source[source.index('void weather_ui_set_weather_data('):source.index('void weather_ui_set_time(')]
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / 'weather_update.inc').write_text(update)
            binary = root / 'weather-ui-test'
            build = subprocess.run(['cc', '-std=c11', '-Wall', '-Wextra', '-Werror',
                '-fsanitize=address,undefined', '-I', directory,
                str(here / 'test_weather_ui.c'), '-o', str(binary)], capture_output=True, text=True)
            self.assertEqual(build.returncode, 0, build.stderr)
            run = subprocess.run([str(binary)], capture_output=True, text=True)
            self.assertEqual(run.returncode, 0, run.stderr)

if __name__ == '__main__':
    unittest.main()
