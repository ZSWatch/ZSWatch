import pytest
import utils
import logging

logging.basicConfig(level=logging.INFO)


def pytest_configure():
    utils.flash()


@pytest.fixture(autouse=True)
def reset():
    utils.reset()
    yield
