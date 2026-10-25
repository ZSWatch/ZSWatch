"""
Living Sky watchface on native_sim: weather effects, sky tint and the animation burst.

Injects each weather type over the shell (``event weather``), checks for crashes, that
the animation runs as a short burst and that no LVGL timer is left once it stops, the
display sleeps or the face is removed. Screenshots are saved for manual review.

Usage::

    pytest test_native_living_sky.py -s
    pytest test_native_living_sky.py -s --screenshot-dir /tmp/living_sky
"""

import os
import re
import time

import pytest
from native_sim_runner import NativeSimDevice
from test_native_app import BOOT_MARKER, BOOT_TIMEOUT, _find_exe

FACE_NAME = "Living Sky"
# CONFIG_WATCHFACE_LIVING_SKY_ANIMATION_SECONDS default
BURST_SECONDS = 6

# (name, "hour minute", "code temp_C wind_kmh wind_dir")
WEATHER_CASES = [
    ("clear_day", "13 37", "800 21 10 270"),
    ("clear_night", "23 10", "800 9 5 90"),
    ("clouds", "11 05", "803 15 30 270"),
    ("rain", "15 20", "501 12 25 250"),
    ("snow", "09 15", "601 -3 15 300"),
    ("fog", "07 45", "741 6 3 180"),
    ("thunderstorm", "17 30", "211 19 35 220"),
]


# Override the hardware autouse fixtures from conftest.py, see test_native_app.py.
@pytest.fixture(autouse=True)
def prepare_device():
    yield


@pytest.fixture(autouse=True)
def reset_device():
    yield


@pytest.fixture(scope="function", autouse=True)
def uart_logs():
    yield None


def _shell(sim, cmd, wait=0.5):
    """Run a shell command and return only the output it produced."""
    start = len(sim.get_shell_output())
    sim.shell_command(cmd)
    time.sleep(wait)
    return re.sub(r"\x1b\[[0-9;]*m", "", sim.get_shell_output()[start:])


def _lv_timers(sim):
    out = _shell(sim, "lv_timers")
    m = re.search(r"LVGL timers: (\d+)", out)
    assert m, f"lv_timers gave no count: {out!r}"
    return int(m.group(1))


def _count(sim, marker):
    return sim.get_logs().count(marker)


def _wait_for_count(sim, marker, count, timeout):
    deadline = time.time() + timeout
    while time.time() < deadline:
        if _count(sim, marker) >= count:
            return True
        time.sleep(0.2)
    return _count(sim, marker) >= count


def _assert_no_crash(sim, what):
    crash = sim.has_crash()
    if crash:
        last_lines = "\n".join(sim.get_logs().splitlines()[-30:])
        pytest.fail(f"Crash {what}: {crash}\nLast 30 log lines:\n{last_lines}")


@pytest.mark.linux_only
class TestLivingSky:

    @pytest.fixture(scope="class")
    def sim(self, request):
        exe = _find_exe(request)
        if not exe:
            pytest.skip("No native_sim executable found (build or provide --exe-path)")

        device = NativeSimDevice(exe_path=exe)
        device.start()
        if not device.wait_for_log(BOOT_MARKER, timeout=BOOT_TIMEOUT):
            logs = device.get_logs()
            device.stop()
            pytest.fail("native_sim failed to boot:\n" + "\n".join(logs.splitlines()[-30:]))

        time.sleep(1)
        faces = _shell(device, "watchface list")
        m = re.search(r"(\d+): " + FACE_NAME, faces)
        if not m:
            device.stop()
            pytest.skip(f"{FACE_NAME} watchface not in this build")
        device.living_sky_index = int(m.group(1))

        _shell(device, f"watchface set {device.living_sky_index}")
        assert _wait_for_count(device, "Burst start", 1, 5), "Burst did not start when the face was shown"
        assert _wait_for_count(device, "Burst stop", 1, BURST_SECONDS + 3), "First burst never stopped"
        time.sleep(0.5)
        device.idle_timers = _lv_timers(device)

        yield device
        device.stop()

    @pytest.fixture
    def screenshot_dir(self, request):
        path = os.path.join(request.config.getoption("--screenshot-dir") or "/tmp", "living_sky")
        os.makedirs(path, exist_ok=True)
        return path

    def _wait_idle(self, sim):
        """Wait until any running burst has stopped."""
        assert _wait_for_count(sim, "Burst stop", _count(sim, "Burst start"), BURST_SECONDS + 3), \
            "Burst did not stop"
        time.sleep(0.3)

    @pytest.mark.parametrize("name,clock,weather", WEATHER_CASES, ids=[c[0] for c in WEATHER_CASES])
    def test_weather_effect(self, sim, screenshot_dir, name, clock, weather):
        """Each weather type starts a burst without crashing, screenshots mid burst and after."""
        self._wait_idle(sim)
        _shell(sim, "power wake", 0.2)
        _shell(sim, f"event time {clock}", 1.2)
        starts = _count(sim, "Burst start")

        code = weather.split()[0]
        out = _shell(sim, f"event weather {weather}", 0.3)
        assert "Injected weather" in out, out
        assert sim.wait_for_log(f"Weather {code} -> effect", timeout=3), "Face did not get the weather"
        assert _wait_for_count(sim, "Burst start", starts + 1, 3), "New weather did not start a burst"

        time.sleep(1.0)
        sim.screenshot(os.path.join(screenshot_dir, f"{name}_mid.png"))
        assert _lv_timers(sim) == sim.idle_timers + 1, "Expected exactly one extra LVGL timer during the burst"
        _assert_no_crash(sim, f"during {name}")

        self._wait_idle(sim)
        sim.screenshot(os.path.join(screenshot_dir, f"{name}_end.png"))
        assert _lv_timers(sim) == sim.idle_timers, "LVGL timer left after the burst"
        _assert_no_crash(sim, f"after {name}")

    def test_burst_is_short(self, sim):
        """The burst stops by itself after about BURST_SECONDS."""
        self._wait_idle(sim)
        starts = _count(sim, "Burst start")
        _shell(sim, "power wake", 0.2)
        _shell(sim, "touch 120 120", 0.1)
        t0 = time.time()
        assert _wait_for_count(sim, "Burst start", starts + 1, 3), "Tap did not start a burst"
        assert _wait_for_count(sim, "Burst stop", starts + 1, BURST_SECONDS + 3), "Burst did not stop"
        elapsed = time.time() - t0
        assert BURST_SECONDS - 1 < elapsed < BURST_SECONDS + 3, f"Burst took {elapsed:.1f} s"

    def test_display_sleep_stops_burst(self, sim):
        """Display off stops the animation at once, display on starts a new burst."""
        self._wait_idle(sim)
        _shell(sim, "power wake", 0.2)
        starts = _count(sim, "Burst start")
        _shell(sim, "touch 120 120", 0.1)
        assert _wait_for_count(sim, "Burst start", starts + 1, 3)

        stops = _count(sim, "Burst stop")
        _shell(sim, "power sleep", 0.2)
        assert _wait_for_count(sim, "Burst stop", stops + 1, 2), "Burst kept running with the display off"
        assert _lv_timers(sim) == sim.idle_timers, "LVGL timer left while the display is off"

        _shell(sim, "power wake", 0.2)
        assert _wait_for_count(sim, "Burst start", starts + 2, 3), "Display on did not start a burst"
        _assert_no_crash(sim, "around display sleep")

    def test_face_removed_stops_burst(self, sim):
        """Switching away from the face mid burst stops the animation."""
        self._wait_idle(sim)
        _shell(sim, "power wake", 0.2)
        starts = _count(sim, "Burst start")
        _shell(sim, "touch 120 120", 0.1)
        assert _wait_for_count(sim, "Burst start", starts + 1, 3)

        stops = _count(sim, "Burst stop")
        other = 0 if sim.living_sky_index != 0 else 1
        _shell(sim, f"watchface set {other}", 0.2)
        assert _wait_for_count(sim, "Burst stop", stops + 1, 2), "Burst kept running after the face was removed"
        _assert_no_crash(sim, "after removing the face")

        # Back to Living Sky for any later test.
        _shell(sim, f"watchface set {sim.living_sky_index}", 0.2)
        assert _wait_for_count(sim, "Burst start", starts + 2, 3)
        self._wait_idle(sim)
        assert _lv_timers(sim) == sim.idle_timers
