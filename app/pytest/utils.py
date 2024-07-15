import subprocess
import logging
import time
import pylink

SERIAL_NUMBER = "760208490"

log = logging.getLogger()


def current_milli_time():
    """Get times in milliseconds"""
    return round(time.time() * 1000)


def reset():
    """Reset DUT"""
    log.info("Reset DUT")
    subprocess.run(
        [
            "nrfjprog",
            "--family",
            "nrf53",
            "--snr",
            SERIAL_NUMBER,
            "--reset",
        ],
        shell=False,
        stderr=subprocess.STDOUT,
        text=True,
        check=True,
    )


def flash():
    """Flash firmware"""
    log.info("Flashing CP_APPLICATION.")
    subprocess.run(
        [
            "nrfjprog",
            "--family",
            "nrf53",
            "--snr",
            SERIAL_NUMBER,
            "--program",
            "./zswatch_nrf5340_cpuapp@3_debug.hex",
            "--recover",
            "--verify",
        ],
        shell=False,
        stderr=subprocess.STDOUT,
        text=True,
        check=True,
    )

    log.info("Flashing CP_NETWORK.")
    subprocess.run(
        [
            "nrfjprog",
            "--family",
            "nrf53",
            "--snr",
            SERIAL_NUMBER,
            "--program",
            "./zswatch_nrf5340_CPUNET.hex",
            "--coprocessor",
            "CP_NETWORK",
            "--verify",
            "--reset",
        ],
        shell=False,
        stderr=subprocess.STDOUT,
        text=True,
        check=True,
    )


def read_rtt(target_device="nRF5340_XXAA", timeout_ms=10000):
    """Read Segger RTT output"""
    jlink = pylink.JLink()
    log.info("Connecting to JLink...")
    jlink.open(serial_no=SERIAL_NUMBER)
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
