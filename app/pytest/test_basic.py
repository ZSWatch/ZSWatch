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
    log.info("Wait for boot")
    time.sleep(60)
    assert utils.read_rtt(timeout_ms=20000).find("Enter inactive") != -1
