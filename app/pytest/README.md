# ZSWatch Test Suite

Hardware-in-the-loop testing for ZSWatch firmware using pytest.

## BLE pairing window regression

`check_ble_pairing.py` exercises the real Bluetooth Settings menu and Zephyr SMP
over a local TCP connection using Bumble. It requires Linux and Xvfb, but no
physical Bluetooth adapter. Install its optional dependencies separately:

The firmware source must include pairing-window enforcement from commit
`8872c102` (the `jakkra_security_audit` branch). The companion branch without
that fix, or an older simulator executable, is intentionally rejected by the
generated-configuration check. The test is not a workaround for missing
firmware enforcement. Build the matching source with unauthenticated bond
replacement disabled before running:

```bash
python3 -m venv /tmp/zswatch-pairing-venv
/tmp/zswatch-pairing-venv/bin/pip install -r app/pytest/requirements-pairing.txt

source .nrf_env.sh
west build --build-dir app/build_pairing_audit app --board native_sim/native/64 -- \
  -DSB_CONF_FILE=sysbuild_no_mcuboot_no_xip.conf \
  -DEXTRA_CONF_FILE=boards/test_mode.conf \
  -DCONFIG_BLE_DISABLE_PAIRING_REQUIRED=n \
  -DCONFIG_BT_SMP_ALLOW_UNAUTH_OVERWRITE=n

/tmp/zswatch-pairing-venv/bin/python app/pytest/check_ble_pairing.py \
  --exe app/build_pairing_audit/app/zephyr/zephyr.exe \
  --output-dir /tmp/zswatch-pairing-results
```

The test checks closed, open and expired windows with both bonding modes,
plaintext write rejection, protected writes after authorized pairing, existing
bond reconnection, and rejection of unauthenticated bond replacement. The
expiry tests wait for the real 60-second timeout. Use `--scenarios closed`,
`--scenarios open` or `--scenarios expired` for individual groups.

The script validates the generated `.config` before starting. The explicit
`BLE_DISABLE_PAIRING_REQUIRED=n` is necessary because simulator and debug
fragments otherwise permit unencrypted transport writes. Firmware and shell
logs plus a screenshot are saved per case in the output directory.

## Setup

### Device Configuration
1. Connect your devices and check persistent USB paths:
   ```bash
   ls -la /dev/serial/by-id/
   ```

2. Update `devices.yaml` with persistent device paths like this:
   ```yaml
   devices:
     watchdk@1:
       jlink_serial: "1057713387"
       serial_port: "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_B0031VW0-if00-port0"
       ppk2_port: "/dev/serial/by-id/usb-Nordic_Semiconductor_PPK2_E01C7386A791-if01"
       usb_cdc_port: "/dev/serial/by-id/usb-ZEPHYR_ZSWatch_D476C294A8135102-if00"
       mcuboot_usb_cdc_port: "/dev/serial/by-id/usb-ZEPHYR_ZSWatch_BOOT_D476C294A8135102-if00"
     zswatch@5:
       jlink_serial: "760208515"
       serial_port: "/dev/serial/by-id/usb-SEGGER_J-Link_000760208515-if00"
   ```

### Running Tests
```bash
# Single test file
pytest test_basic.py

# Hardware tests only
pytest -m "not linux_only"

# Linux simulation tests only
pytest -m "linux_only"

# Power consumption tests
pytest -m "ppk2"
```

## Test Types
- **Basic tests**: Boot verification, serial communication
- **BLE tests**: Bluetooth functionality
- **Power tests**: Current consumption measurement (requires PPK2)
- **Linux tests**: Native simulation testing
