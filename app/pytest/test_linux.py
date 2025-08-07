import os
import subprocess
import pytest
import logging
import time

log = logging.getLogger()


@pytest.fixture(scope="module", autouse=True)
def disable_bt(request):
    log.info("Disabling Bluetooth...")
    try:
        with os.popen("yes | sudo btmgmt --index 0 power off") as stream:
            output = stream.read()
        log.info(output)
    except Exception as e:
        log.warning("btmgmt command failed: %s", e)

    def enable_bt():
        log.info("Re-enabling Bluetooth...")
        try:
            with os.popen("yes | sudo btmgmt --index 0 power on") as stream:
                output = stream.read()
            log.info(output)
        except Exception as e:
            log.warning("btmgmt command failed: %s", e)

    request.addfinalizer(enable_bt)


@pytest.mark.linux_only
def test_native_sim_boots():
    # Start Xvfb on display :99
    # This creates a virtual framebuffer for GUI applications
    xvfb_proc = subprocess.Popen(
        ["Xvfb", ":99", "-screen", "0", "800x600x24"],
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
    )

    found = False

    try:
        env = os.environ.copy()
        env["DISPLAY"] = ":99"
        # Print current working directory and its contents for debugging
        log.info("Current working directory: %s", os.getcwd())
        log.info("Directory contents: %s", os.listdir(os.getcwd()))
        # Ensure the executable has the correct permissions
        os.chmod("./zswatch_native_sim_64.exe", 0o755)
        # Run your SDL app
        proc = subprocess.Popen(
            ["sudo", "./zswatch_native_sim_64.exe", "--bt-dev=hci0"],
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            env=env,
        )
        search_strings = ["Start Apple Media Service", "Disable Pairable"]
        found_strings = set()
        log.info("Wait for boot log...")
        timeout_sec = 5
        start_time = time.time()
        try:
            while time.time() - start_time < timeout_sec:
                line = proc.stdout.readline()
                log.info(line.strip())
                if not line:
                    break
                for s in search_strings:
                    if s in line:
                        found_strings.add(s)
                if len(found_strings) == len(search_strings):
                    found = True
                    break
            else:
                log.warning(
                    f"Timeout ({timeout_sec}s) reached without finding all expected strings."
                )
        finally:
            proc.kill()
            proc.wait()

        # Take screenshot of the virtual display
        subprocess.run(["import", "-window", "root", "sdl_screenshot.png"], env=env)
        log.info("Screenshot saved as sdl_screenshot.png")
    finally:
        xvfb_proc.kill()
        xvfb_proc.wait()

    assert found, f"Expected strings not found: {search_strings} in {found_strings}"
