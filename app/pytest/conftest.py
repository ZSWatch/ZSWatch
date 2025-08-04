import pytest
import utils
import logging
import yaml
import serial
import time

logging.basicConfig(level=logging.INFO)


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
    """
    if "device_param" not in metafunc.fixturenames:
        return  # Only handle tests requesting device_param

    board_option = metafunc.config.getoption("--board")
    devices = get_all_devices()

    # Detect if running with '-m "not linux_only"'
    skip_native_sim = (
        metafunc.config.option.markexpr
        and "not linux_only" in metafunc.config.option.markexpr
    )

    # Build list of devices to parametrize
    param_devices = []
    ids = []

    for board_name, device_cfg in devices.items():
        if board_option and board_name != board_option:
            continue  # Skip non-selected boards

        if skip_native_sim and board_name == "native_sim":
            continue  # Skip native_sim if requested

        device = dict(device_cfg)
        param_devices.append(device)
        ids.append(board_name)

    if not param_devices:
        raise ValueError("No devices match the selection criteria")

    metafunc.parametrize("device_param", param_devices, ids=ids)


# Fixture that wraps the param from pytest_generate_tests
@pytest.fixture(scope="function")
def device_config(device_param):
    device = device_param  # use the parameter from pytest_generate_tests
    board = device.get("board", None) or next(
        (k for k, v in get_all_devices().items() if v == device), None
    )
    logging.info(f"Using device configuration for board: {board}")

    serial_device = None
    if "serial_port" in device:
        serial_device = serial.Serial(device["serial_port"], baudrate=115200, timeout=5)
        print(f"Connected to serial port: {device['serial_port']}")

    cfg = dict(device)
    cfg["board"] = str(board)
    cfg["serial"] = serial_device

    yield cfg

    if serial_device and serial_device.is_open:
        logging.info("Closing serial connection")
        serial_device.close()


def pytest_addoption(parser):
    parser.addoption("--board", action="store", help="Board name from devices.yaml")


# Flash once per board before all tests for that board
_flashed_boards = set()


@pytest.fixture(autouse=True)
def prepare_device(device_config):
    board = device_config["board"]
    if board in _flashed_boards:
        return
    if "jlink_serial" in device_config:
        try:
            print(f"\n Recovering device: {board}")
            utils.recover(device_config)
            print(f"\n Flashing device: {board}")
            utils.flash(device_config)
            time.sleep(1)
            _flashed_boards.add(board)
        except Exception as e:
            logging.error(f"Error preparing device {board}: {e}")
            pytest.skip(
                "Skipping tests: flashing failed (device not ready or connected?)"
            )
            raise
    else:
        logging.info(f"No jlink_serial for {board}, skipping recover/flash.")


# Reset before each test
@pytest.fixture(autouse=True)
def reset_device(device_config):
    if "jlink_serial" in device_config:
        print(f"\n Resetting device: {device_config['board']}")
        utils.reset(device_config)
        time.sleep(1)
    else:
        logging.info(f"No jlink_serial for {device_config['board']}, skipping reset.")
