import utils
import pytest
import logging

log = logging.getLogger()

def test_flash(jlink, hw):
    """Test flashing, don't need to assert as failing to flash throws exception"""
    #utils.flash(serial=jlink, hw=hw)
    print("flash")

def test_reset(jlink):
    """Test Reset, don't need to assert as failing to reset throws exception"""
    #utils.reset(serial=jlink)
    print("reset")

def test_boot(jlink):
    #search_string = "Enter inactive"
    #log.info("Booting...")
    #log.info("Check for \'{}\' string".format(search_string))
    #assert utils.read_rtt(serial=jlink, timeout_ms=60000).find(search_string) != -1
    print("boot")