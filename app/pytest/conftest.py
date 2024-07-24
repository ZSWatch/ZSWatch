import utils
import pytest
import logging
import subprocess

logging.basicConfig(level=logging.INFO)

def pytest_addoption(parser):
    parser.addoption("--jlink", action="store", help="Serial number of target J-Link")
    parser.addoption("--hw", action="store", help="ZSWatch hardware revision")
    parser.addoption("--ppk", action="store", default=None, help="Port used by an optional Power Profiling Kit")

def pytest_configure(config):
    log = logging.getLogger()

    jlink = config.getoption("--jlink")
    hw = config.getoption("--hw")
    ppk = config.getoption("--ppk")

    output = subprocess.run(
        [
            "pylink",
            "emulator",
            "-l"
        ],
        capture_output=True
    ).stdout.decode().split("\n")

    if(not("Serial Number: {}".format(jlink) in output)):
        # We can skip the test when the J-Link is not connected. Use the return code 0 to avoid the canceling of the pipeline
        pytest.exit(reason="Target J-Link not connected. Skip test!", returncode=0)

    log.info("Using J-Link {}".format(jlink))
    log.info("Using ZSWatch revision {}".format(hw))

    if(ppk != None):
        log.info("Using PPK2 on {}".format(ppk))

    if(utils.is_locked(serial=jlink)):
        log.info("Device locked. Unlocking...")
        if(utils.unlock(serial=jlink)):
            log.info("Unlocking")

    utils.flash(serial=jlink, hw=hw)

@pytest.fixture(autouse=True)
def reset(jlink):
    utils.reset(serial=jlink)
    yield

@pytest.fixture(autouse=True)
def jlink(request):
    '''Serial number of the used J-Link debugger'''
    return request.config.getoption("--jlink")

@pytest.fixture(autouse=True)
def hw(request):
    '''Hardwawre version used'''
    return request.config.getoption("--hw")

@pytest.fixture
def ppk(request):
    '''COM port for an optional Power Profiling Kit'''
    return request.config.getoption("--ppk")
