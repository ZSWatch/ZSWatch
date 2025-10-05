# ZSWatch Test Suite

Hardware-in-the-loop testing for ZSWatch firmware using pytest.

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
