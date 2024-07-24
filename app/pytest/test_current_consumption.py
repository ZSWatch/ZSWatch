import utils
import pytest
import logging

log = logging.getLogger()

def test_a(port):
    if((port == None) or (utils.ppk_connected() == 0)):
        pytest.skip("No PPK available")