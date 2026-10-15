"""Exercise the production HTTP handoff/parser with the firmware's cJSON."""
import os
from pathlib import Path
import subprocess
import tempfile
import unittest

class WeatherResponseTests(unittest.TestCase):
    def test_borrowed_response_is_deferred_and_validated(self):
        here = Path(__file__).resolve().parent
        app = here.parents[1]
        source = (app / 'src/applications/weather/weather_app.c').read_text()
        code = source[source.index('static void apply_http_result('):source.index('static void publish_weather_data(struct k_work *work)\n{')]
        sdk = Path(os.environ.get('CJSON_DIR', '/opt/nordic/ncs/v3.3.0-preview1/modules/lib/cjson'))
        self.assertTrue((sdk / 'cJSON.c').is_file(), 'Set CJSON_DIR to the firmware cJSON source directory')
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / 'weather_response.inc').write_text(code)
            (root / 'lvgl.h').write_text('typedef struct {} lv_obj_t; typedef unsigned lv_color_t;\n')
            build = subprocess.run(['cc', '-std=c11', '-Wall', '-Wextra', '-Werror',
                '-Wno-unused-function', '-fsanitize=address,undefined', '-I', str(root), '-I', str(sdk),
                '-I', str(app / 'src/ble'), '-I', str(app / 'src/applications/weather'),
                str(here / 'test_weather_response.c'), str(sdk / 'cJSON.c'), '-o', str(root / 'test')], capture_output=True, text=True)
            self.assertEqual(build.returncode, 0, build.stderr)
            run = subprocess.run([str(root / 'test')], capture_output=True, text=True)
            self.assertEqual(run.returncode, 0, run.stdout + run.stderr)

if __name__ == '__main__':
    unittest.main()
