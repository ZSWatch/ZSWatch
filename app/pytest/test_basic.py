import time
import logging
import utils
import yaml

log = logging.getLogger()


def test_boot(device_config):
    search_string = "Disable Pairable"
    log.info("Check for '{}' string".format(search_string))
    timeout_s = 10
    start_time = time.time()
    output = ""
    found = False
    while time.time() - start_time < timeout_s:
        chunk = utils.read_log(device_config, timeout_ms=1000)
        output += chunk
        if search_string in output:
            found = True
            break
    log.info(output)
    assert (
        found
    ), f"'{search_string}' not found in serial output within {timeout_s} seconds"
