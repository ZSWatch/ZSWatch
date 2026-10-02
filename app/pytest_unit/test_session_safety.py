"""Execute the real HIL fixtures with fake hardware, outside their autouse tree."""

import os
from pathlib import Path
import shutil
import subprocess
import sys

import pytest


@pytest.fixture
def lab(tmp_path):
    source = Path(__file__).parents[1] / "pytest"
    shutil.copy(source / "conftest.py", tmp_path)
    shutil.copy(source / "pytest.ini", tmp_path)
    (tmp_path / "devices.yaml").write_text("devices:\n  watchdk@1:\n    jlink_serial: 'fake'\n    ppk2_port: 'fake'\n")
    shutil.copy(tmp_path / "devices.yaml", tmp_path / "local.yaml")
    (tmp_path / "utils.py").write_text('''
from pathlib import Path
def note(text):
    with Path("calls.log").open("a") as out: out.write(text + "\\n")
def reset(cfg): note("reset")
def recover(cfg): note("recover")
def flash(cfg):
    note("flash")
    if Path("fail-flash").exists(): raise RuntimeError("probe disconnected")
class LogCollector:
    def __init__(self, cfg, show_uart=False): pass
    def start(self): note("collector_start")
    def stop(self): note("collector_stop")
''')
    (tmp_path / "ppk2_helper.py").write_text('''
from utils import note
def setup_ppk2(cfg):
    note("power_setup")
    return None
''')
    (tmp_path / "serial.py").write_text("class SerialException(Exception): pass\n")

    def invoke(body, *options):
        (tmp_path / "test_probe.py").write_text("import pytest\n" + body)
        result = subprocess.run(
            [sys.executable, "-m", "pytest", "test_probe.py", "--board", "watchdk@1", *options],
            cwd=tmp_path, env={**os.environ, "PYTEST_DISABLE_PLUGIN_AUTOLOAD": "1"},
            capture_output=True, text=True, timeout=20,
        )
        calls = (tmp_path / "calls.log").read_text().splitlines() if (tmp_path / "calls.log").exists() else []
        return result, calls
    return invoke


SAFE = "@pytest.mark.companion_session\ndef test_status(device_config): pass\n"
SESSION = ("--preserve-session", "--device-config", "local.yaml")


def test_session_does_not_deploy_reset_or_switch_power(lab):
    result, calls = lab(SAFE, *SESSION)
    assert result.returncode == 0, result.stdout + result.stderr
    assert calls == ["collector_start", "collector_stop"]


@pytest.mark.parametrize("body", [
    "def test_boot(device_config): raise AssertionError('must never run')\n",
    "@pytest.mark.companion_session\ndef test_update(device_config, restore_firmware): pass\n",
    "@pytest.mark.companion_session\n@pytest.mark.ppk2\ndef test_power(device_config): pass\n",
])
def test_unsafe_suites_rejected_before_hardware_setup(lab, body):
    result, calls = lab(body, *SESSION)
    assert result.returncode != 0
    assert "--preserve-session refuses" in result.stderr
    assert calls == []


def test_companion_probe_without_session_flag_cannot_flash(lab):
    result, calls = lab(SAFE)
    assert result.returncode != 0
    assert "require --preserve-session" in result.stderr
    assert calls == []


def test_unknown_board_cannot_look_like_success(lab):
    result, calls = lab(SAFE, *SESSION, "--board", "missing")
    assert result.returncode != 0
    assert "No matching device configuration" in result.stdout + result.stderr
    assert calls == []


def test_dynamic_firmware_fixture_also_guarded(lab):
    result, calls = lab(
        "@pytest.mark.companion_session\ndef test_status(device_config, request):\n"
        "    request.getfixturevalue('restore_firmware')\n", *SESSION,
    )
    assert result.returncode == 1
    assert "Firmware restoration is forbidden" in result.stdout
    assert "recover" not in calls and "flash" not in calls and "reset" not in calls


def test_missing_explicit_lab_config_is_error(lab):
    result, calls = lab(SAFE, "--preserve-session")
    assert result.returncode != 0
    assert "requires --device-config" in result.stderr
    assert calls == []


def test_deployment_failure_is_error_not_skip(lab, tmp_path):
    (tmp_path / "fail-flash").touch()
    result, calls = lab("def test_boot(device_config): pass\n")
    assert result.returncode == 1, result.stdout + result.stderr
    assert "Device deployment failed" in result.stdout
    assert "recover" in calls and "flash" in calls
    assert "skipped" not in result.stdout
