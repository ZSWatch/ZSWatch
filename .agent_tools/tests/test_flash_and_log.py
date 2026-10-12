"""Host-only guards: help/invalid invocations must never touch hardware."""
import contextlib
import importlib.util
import io
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch

spec = importlib.util.spec_from_file_location("flash_and_log", Path(__file__).parents[1] / "flash_and_log.py")
helper = importlib.util.module_from_spec(spec)
spec.loader.exec_module(helper)


class FlashGuardTests(unittest.TestCase):
    def test_help_never_opens_uart_or_executes_a_command(self):
        with patch.object(helper.serial, "Serial") as uart, patch.object(helper.subprocess, "run") as command:
            with contextlib.redirect_stdout(io.StringIO()), self.assertRaises(SystemExit) as ended:
                helper.main(["--help"])
        self.assertEqual(ended.exception.code, 0)
        uart.assert_not_called()
        command.assert_not_called()

    def test_action_port_probe_and_existing_image_required_before_hardware(self):
        for args in ([], ["--reset"], ["--port", "/dev/cu.test", "--reset"],
                     ["--port", "/dev/cu.test", "--probe", "123"],
                     ["--port", "/dev/cu.test", "--probe", "123", "--flash", "/absent.hex"]):
            with self.subTest(args=args), patch.object(helper.serial, "Serial") as uart, \
                    patch.object(helper.subprocess, "run") as command:
                with contextlib.redirect_stderr(io.StringIO()), self.assertRaises(SystemExit):
                    helper.main(args)
                uart.assert_not_called()
                command.assert_not_called()

    def test_both_mutating_commands_explicitly_select_the_probe(self):
        with tempfile.TemporaryDirectory() as directory:
            image = Path(directory) / "firmware.hex"
            image.write_text(":00000001FF\n")
            for action in (["--reset"], ["--flash", str(image)]):
                args = helper.arguments(["--port", "/dev/cu.test", "--probe", "123", *action])
                command = helper.command_for(args, "/tools/nrfjprog")
                self.assertEqual(command[-4:], ["-f", "NRF53", "--snr", "123"])
                self.assertNotIn("--recover", command)

    def test_missing_nrfjprog_fails_without_opening_uart(self):
        with patch.object(helper.shutil, "which", return_value=None), patch.object(helper.serial, "Serial") as uart:
            with contextlib.redirect_stderr(io.StringIO()):
                result = helper.main(["--port", "/dev/cu.test", "--probe", "123", "--reset"])
        self.assertEqual(result, 1)
        uart.assert_not_called()


if __name__ == "__main__":
    unittest.main()
