import pytest
import utils
import logging
import yaml
import serial
import time
from ppk2_helper import setup_ppk2

logging.basicConfig(level=logging.INFO)


@pytest.fixture(scope="function")
def ppk2_instance(device_config, prepare_device):
    """Fixture to provide PPK2 instance for power measurement tests"""
    if "ppk2" not in device_config:
        pytest.skip("PPK2 not available for this device")

    return device_config["ppk2"]


def get_all_devices():
    with open("devices.yaml") as f:
        config = yaml.safe_load(f)
    return config["devices"]


def pytest_generate_tests(metafunc):
    """
    Dynamically parameterizes tests that use the 'device_config' fixture.

    WHY:
    Pytest normally runs each test once. This hook allows a single test
    (e.g., test_boot) to run multiple times — once for each hardware device
    defined in devices.yaml — without duplicating code.

    - Automatically injects test cases for multiple boards.
    - Supports filtering via the --board CLI option.
    - Skips devices like 'native_sim' based on test markers (e.g., -m 'not linux_only').
    - Skips devices without PPK2 for tests marked with @pytest.mark.ppk2.
    """
    if (
        "device_param" not in metafunc.fixturenames
        and "device_config" not in metafunc.fixturenames
    ):
        return  # Only handle tests requesting device_param or device_config

    board_option = metafunc.config.getoption("--board")
    devices = get_all_devices()

    # Detect if running with '-m "not linux_only"'
    skip_native_sim = (
        metafunc.config.option.markexpr
        and "not linux_only" in metafunc.config.option.markexpr
    )

    # Check if this test requires PPK2
    requires_ppk2 = False
    for mark in metafunc.definition.iter_markers():
        if mark.name == "ppk2":
            requires_ppk2 = True
            break

    # Build list of devices to parametrize
    param_devices = []
    ids = []

    for board_name, device_cfg in devices.items():
        if board_option and board_name != board_option:
            continue  # Skip non-selected boards

        if skip_native_sim and board_name == "native_sim":
            continue  # Skip native_sim if requested

        # Skip devices without PPK2 if test requires it
        if requires_ppk2 and "ppk2_port" not in device_cfg:
            continue

        device = dict(device_cfg)
        param_devices.append(device)
        ids.append(board_name)

    if not param_devices:
        return

    metafunc.parametrize("device_param", param_devices, ids=ids)


# Fixture that wraps the param from pytest_generate_tests
@pytest.fixture(scope="function")
def device_config(device_param):
    device = device_param  # use the parameter from pytest_generate_tests
    board = device.get("board", None) or next(
        (k for k, v in get_all_devices().items() if v == device), None
    )

    serial_device = None
    if "serial_port" in device:
        try:
            serial_device = serial.Serial(device["serial_port"], baudrate=115200, timeout=5)
        except serial.SerialException as e:
            logging.error(f"Failed to open serial port {device['serial_port']}: {e}")
            pytest.skip(f"Skipping tests: cannot open serial port {device['serial_port']}")

    cfg = dict(device)
    cfg["board"] = str(board)
    cfg["serial"] = serial_device

    yield cfg

    if serial_device and serial_device.is_open:
        serial_device.close()


# Track PPK2 instances for session cleanup
_ppk2_instances = []


@pytest.fixture(scope="session", autouse=True)
def ppk2_session_cleanup():
    """Session-scoped fixture to cleanup all PPK2 instances at the end of the session"""
    yield  # Let all tests run first

    # Cleanup all PPK2 instances
    for ppk2_manager in _ppk2_instances:
        ppk2_manager.cleanup()
    _ppk2_instances.clear()


def pytest_addoption(parser):
    parser.addoption("--board", action="store", help="Board name from devices.yaml")


# Flash once per board before all tests for that board
_flashed_boards = set()


@pytest.fixture(autouse=True)
def prepare_device(device_config):
    board = device_config["board"]

    # Setup PPK2 power if available (always needed for PPK2 tests)
    if "ppk2_port" in device_config and "ppk2" not in device_config:
        ppk2_manager = setup_ppk2(device_config)
        # Store PPK2 manager in device_config for access by ppk2_instance fixture
        if ppk2_manager:
            device_config["ppk2"] = ppk2_manager
            _ppk2_instances.append(ppk2_manager)
            time.sleep(2)  # Allow PPK2 to settle after setup

    if "jlink_serial" in device_config:
        # Flash only once per board
        if board in _flashed_boards:
            return
        try:
            utils.recover(device_config)
            utils.flash(device_config)
            _flashed_boards.add(board)
        except Exception as e:
            logging.error(f"Error preparing device {board}: {e}")
            if "ppk2" in device_config:
                device_config["ppk2"].cleanup()
            pytest.skip(
                "Skipping tests: flashing failed (device not ready or connected?)"
            )
            raise


# Reset before each test
@pytest.fixture(autouse=True)
def reset_device(device_config):
    if "jlink_serial" in device_config:
        utils.reset(device_config)
        time.sleep(1)
