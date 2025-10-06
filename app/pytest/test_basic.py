import logging

import pytest
import utils

log = logging.getLogger()


def test_boot(device_config):
    search_string = "Disable Pairable"
    log.info("Check for '{}' string".format(search_string))
    timeout_s = 10
    collector = device_config.get("uart_logs")
    if collector is None or not collector.has_source():
        pytest.skip("No log collector available for this device")
    collector.clear()
    utils.reset(device_config)
    found = collector.wait_for(search_string, timeout_s)
    log.info(collector.get_text())
    assert (
        found
    ), f"'{search_string}' not found in serial output within {timeout_s} seconds"
