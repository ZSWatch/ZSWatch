"""Compile the actual input listener and exercise pre/post-init delivery."""
from pathlib import Path
import subprocess
import tempfile
import unittest

class InputStartupTests(unittest.TestCase):
    def test_early_touch_does_not_enter_uninitialized_managers(self):
        here = Path(__file__).resolve().parent
        source = (here.parents[1] / 'src/ui/zsw_ui_controller.c').read_text()
        start = source.index('static void on_input_subsys_callback(struct input_event *evt, void *user_data)\n{')
        callback = source[start:source.index('static void handle_screen_gesture(', start)]
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / 'input_callback.inc').write_text(callback)
            binary = root / 'test'
            build = subprocess.run(['cc', '-std=c11', '-Wall', '-Wextra', '-Werror', '-Wno-unused-parameter',
                '-fsanitize=address,undefined', '-I', str(root), str(here / 'test_input_startup.c'), '-o', str(binary)], capture_output=True, text=True)
            self.assertEqual(build.returncode, 0, build.stderr)
            run = subprocess.run([str(binary)], capture_output=True, text=True)
            self.assertEqual(run.returncode, 0, run.stderr)

if __name__ == '__main__': unittest.main()
