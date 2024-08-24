import time
import logging
import utils

log = logging.getLogger()


def test_flash():
    """Test flashing, don't need to assert as failing to flash throws exception"""
    utils.flash()


def test_reset():
    """Test Reset, don't need to assert as failing to reset throws exception"""
    utils.reset()


def test_boot():
    search_string = "Enter inactive"
    log.info("Booting...")
    log.info("Check for \'{}\' string".format(search_string))
    assert utils.read_rtt(timeout_ms=60000).find(search_string) != -1
