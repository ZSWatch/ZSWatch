"""Keep offline bench tests independent of the parent HIL fixtures."""

import pytest


@pytest.fixture(autouse=True)
def prepare_device():
    pass


@pytest.fixture(autouse=True)
def reset_device():
    pass


@pytest.fixture(autouse=True)
def uart_logs():
    pass
