import subprocess
import logging
import time
import pylink
import serial

log = logging.getLogger()


def current_milli_time():
    """Get times in milliseconds"""
    return round(time.time() * 1000)


def reset(device_config):
    """Reset DUT"""
    log.info("Reset DUT")
    subprocess.run(
        [
            "nrfjprog",
            "--family",
            "nrf53",
            "--snr",
            device_config["jlink_serial"],
            "--reset",
        ],
        shell=False,
        stderr=subprocess.STDOUT,
        text=True,
        check=True,
    )


def flash(device_config):
    """Flash firmware"""
    log.info("Flashing CP_APPLICATION.")
    try:
        subprocess.run(
            [
                "nrfjprog",
                "--family",
                "nrf53",
                "--snr",
                device_config["jlink_serial"],
                "--program",
                f"./{device_config['board']}_nrf5340_cpuapp_debug.hex",
                "--chiperase",
                "--qspisectorerase",
                "--verify",
                "--qspiini",
                "../qspi_mx25u51245.ini",
            ],
            shell=False,
            stderr=subprocess.STDOUT,
            stdout=subprocess.PIPE,
            text=True,
            check=True,
        )
    except subprocess.CalledProcessError as e:
        log.error(f"Error flashing CP_APPLICATION: {e.stdout}")
        raise

    log.info("Flashing CP_NETWORK.")
    try:
        subprocess.run(
            [
                "nrfjprog",
                "--family",
                "nrf53",
                "--snr",
                device_config["jlink_serial"],
                "--program",
                "zswatch_nrf5340_CPUNET.hex",
                "--coprocessor",
                "CP_NETWORK",
                "--sectorerase",
                "--verify",
                "--reset",
            ],
            shell=False,
            stderr=subprocess.STDOUT,
            stdout=subprocess.PIPE,
            text=True,
            check=True,
        )
    except subprocess.CalledProcessError as e:
        log.error(f"Error flashing CP_NETWORK: {e.stdout}")
        raise


def recover(device_config):
    """Recover Device"""
    log.info("Recovering CP_APPLICATION.")
    subprocess.run(
        [
            "nrfjprog",
            "--family",
            "nrf53",
            "--snr",
            device_config["jlink_serial"],
            "--recover",
        ],
        shell=False,
        stderr=subprocess.STDOUT,
        text=True,
        check=True,
    )

    log.info("Recovering CP_NETWORK.")
    subprocess.run(
        [
            "nrfjprog",
            "--family",
            "nrf53",
            "--snr",
            device_config["jlink_serial"],
            "--recover",
            "--coprocessor",
            "CP_NETWORK",
        ],
        shell=False,
        stderr=subprocess.STDOUT,
        text=True,
        check=True,
    )


def read_rtt(serial_number, timeout_ms=10000, target_device="nRF5340_XXAA",):
    """Read Segger RTT output"""
    jlink = pylink.JLink()
    log.info("Connecting to JLink...")
    jlink.open(serial_no=serial_number)
    log.info("Connecting to %s..." % target_device)
    jlink.set_tif(pylink.enums.JLinkInterfaces.SWD)
    jlink.connect(target_device)
    jlink.rtt_start(None)
    start_time = current_milli_time()
    read_data = ""

    while jlink.connected() and start_time + timeout_ms > current_milli_time():
        read_bytes = jlink.rtt_read(0, 1024)

        if read_bytes and read_bytes != "":
            data = "".join(map(chr, read_bytes))
            read_data = read_data + data

        time.sleep(0.1)

    jlink.close()
    print(read_data)
    log.debug(read_data)

    return read_data


def read_serial(serial_device, timeout_ms=10000):
    """Read serial output from a given serial handle"""
    start_time = current_milli_time()
    read_data = ""

    while start_time + timeout_ms > current_milli_time():
        if serial_device.in_waiting > 0:
            data = serial_device.read(serial_device.in_waiting).decode(errors="replace")
            read_data += data
        time.sleep(0.1)

    print(read_data)
    log.debug(read_data)
    return read_data

def read_log(device_config, timeout_ms=10000):
    """Read from either RTT or serial based on device configuration"""
    if "serial_port" in device_config:
        return read_serial(device_config["serial"], timeout_ms)
    elif "jlink_serial" in device_config and device_config["jlink_serial"]:
        return read_rtt(device_config["jlink_serial"], timeout_ms)
    else:
        log.error("No valid communication method found in device configuration.")
        return ""