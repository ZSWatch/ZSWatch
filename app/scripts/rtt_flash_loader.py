import pylink
import argparse
import sys
import time
import os
from binascii import crc32
from struct import *

from threading import Thread

RAM_ADDR = 0x2007FFFC
RTT_FLASH_LOAD_BOOT_MODE = 0x0A0A0A0A
RTT_FLASH_ERASE_EXTERNAL_FLASH = 0xFFFFFFFF
RTT_HEADER_MAGIC = 0x0A0A0A0A


def read_rtt(jlink):
    """Reads the JLink RTT buffer #0 at 10Hz and prints to stdout.

    This method is a polling loop against the connected JLink unit. If
    the JLink is disconnected, it will exit. Additionally, if any exceptions
    are raised, they will be caught and re-raised after interrupting the
    main thread.

    sys.stdout.write and sys.stdout.flush are used since target terminals
    are expected to transmit newlines, which may or may not line up with the
    arbitrarily-chosen 1024-byte buffer that this loop uses to read.

    Args:
      jlink (pylink.JLink): The JLink to read.

    Raises:
      Exception on error.
    """
    try:
        while jlink.connected():
            terminal_bytes = jlink.rtt_read(0, 1024)
            if terminal_bytes and terminal_bytes != "":
                sys.stdout.write("".join(map(chr, terminal_bytes)))
                sys.stdout.flush()
            time.sleep(0.1)
    except Exception:
        print("IO read thread exception, exiting...")
        raise


def dump_flash(jlink, file, partition):
    print(jlink, file)
    with open(file, "wb") as file:
        try:
            bytes = list(bytearray(f"DUMP_START:{partition}", "utf-8")) + [0x0]
            jlink.rtt_write(2, bytes)
            time.sleep(2)

            block_number = 0
            read_data_len = 0
            start_ms = round(time.time() * 1000)
            while jlink.connected():
                bytes = jlink.rtt_read(2, 4096 * 2)
                if len(bytes) == 8:
                    data = "".join(map(chr, bytes))
                    if data == "DUMP_END":
                        print("Received DUMP_END, storing...", read_data_len, "bytes")
                        end_ms = round(time.time() * 1000)
                        print(
                            "Took",
                            end_ms - start_ms,
                            "ms",
                            "Througput: ",
                            (read_data_len * 8 / (end_ms - start_ms)) * 1000,
                            "kbps",
                        )
                        return
                if len(bytes) > 0:
                    read_data_len = read_data_len + len(bytes)
                    file.write(bytearray(bytes))
                    block_number = block_number + 1
                    if block_number % 10 == 0:
                        print("Received", read_data_len, "bytes", end="\r")
        except Exception:
            print("IO read thread exception, exiting...")
            raise


def load_data(jlink, file, partition):
    try:
        buffer_size = 4096
        block_number = 0
        print("FILENAME", file)
        bytes = list(bytearray(f"LOADER_START:{partition}", "utf-8")) + [0x0]
        jlink.rtt_write(2, bytes)
        time.sleep(2)

        with open(file, mode="rb") as f:
            num_sent = 0
            chunk_index = 0
            file_size = os.fstat(f.fileno()).st_size
            print("Filesize:", file_size)
            chunk = f.read(buffer_size)
            start_ms = round(time.time() * 1000)
            while chunk and chunk != "" and jlink.connected():
                to_send = list(bytearray(chunk[chunk_index:]))
                if chunk_index == 0:
                    to_send = (
                        list(
                            bytearray(
                                pack(
                                    "<III",
                                    RTT_HEADER_MAGIC,
                                    block_number * buffer_size,
                                    crc32(bytearray(chunk[chunk_index:])),
                                )
                            )
                        )
                        + to_send
                    )
                sent = jlink.rtt_write(2, to_send)
                if sent == 0:
                    continue
                if sent != len(to_send):
                    chunk_index = chunk_index + sent
                    continue
                num_sent = num_sent + len(chunk)
                print(
                    block_number, num_sent, "/", file_size, "len:", len(chunk), end="\r"
                )

                # Find next none empty block
                chunk = f.read(buffer_size)
                chunk_index = 0
                while chunk and chunk != "":
                    block_number = block_number + 1
                    if bytearray(chunk).count(0xFF) != len(chunk):
                        break
                    chunk = f.read(buffer_size)

            end_ms = round(time.time() * 1000)
            print("Time taken:", end_ms - start_ms, "ms")
            print("Sent", num_sent, "bytes")
            print(
                "Took",
                end_ms - start_ms,
                "ms",
                "Througput: ",
                (num_sent * 8 / (end_ms - start_ms)) * 1000,
                "kbps",
            )
            time.sleep(2)
            print("Sending LOADER_END")
            bytes = list(bytearray("LOADER_END", "utf-8"))
            jlink.rtt_write(2, bytes)
            return

    except Exception:
        print("IO write thread exception, exiting...")
        raise


def rtt_run_flush_loader(
    target_device, file, partition, jlink_speed="auto", read_data_only=False
):
    """Creates connection to target via RTT and either writes a file or reads from flash.

    Args:
      target_device (string): The target CPU to connect to.
      file (string): The binary file to write to target or dump target flash content in.
      read_data_only (bool): optional bool indication if flash should be read instead of written to.

    Returns:
      Always returns ``0`` or a JLinkException.

    Raises:
      JLinkException on error.
    """
    jlink = pylink.JLink()
    print("Connecting to JLink...")
    jlink.open()
    print("Connecting to %s..." % target_device)
    jlink.set_tif(pylink.enums.JLinkInterfaces.SWD)

    if jlink_speed != "auto" and jlink_speed != "adaptive":
        jlink_speed = int(jlink_speed)
    jlink.connect(target_device, speed=jlink_speed)
    print("Connected, send RTT_FLASH_LOAD_BOOT_MODE to RAM_ADDR...")
    jlink.reset(0, False)
    jlink.memory_write32(RAM_ADDR, [RTT_FLASH_LOAD_BOOT_MODE])
    if jlink.memory_read32(RAM_ADDR, 1)[0] != RTT_FLASH_LOAD_BOOT_MODE:
        print("Could not write to RAM_ADDR, exiting...")
        sys.exit(1)
    print("Done. Reset into flash loader mode...")
    jlink.reset(0, False)
    print("Done. Starting RTT transfer...")
    time.sleep(10)
    jlink.rtt_start(None)

    while True:
        try:
            num_up = jlink.rtt_get_num_up_buffers()
            num_down = jlink.rtt_get_num_down_buffers()
            print("RTT started, %d up bufs, %d down bufs." % (num_up, num_down))
            break
        except pylink.errors.JLinkRTTException:
            time.sleep(0.1)

    print("up channels:")
    for buf_index in range(jlink.rtt_get_num_up_buffers()):
        buf = jlink.rtt_get_buf_descriptor(buf_index, True)
        print(
            "    %d: name = %r, size = %d bytes, flags = %d"
            % (buf.BufferIndex, buf.name, buf.SizeOfBuffer, buf.Flags)
        )

    print("down channels:")
    for buf_index in range(jlink.rtt_get_num_down_buffers()):
        buf = jlink.rtt_get_buf_descriptor(buf_index, False)
        print(
            "    %d: name = %r, size = %d bytes, flags = %d"
            % (buf.BufferIndex, buf.name, buf.SizeOfBuffer, buf.Flags)
        )

    try:
        read_thread = Thread(target=read_rtt, args=(jlink,))
        read_thread.daemon = True
        read_thread.start()

        work_thread = None
        if read_data_only:
            work_thread = Thread(target=dump_flash, args=(jlink, file, partition))
        else:
            work_thread = Thread(target=load_data, args=(jlink, file, partition))
        work_thread.daemon = True
        work_thread.start()
        work_thread.join()

        # Restart to get back to normal application
        jlink.reset(0, False)
        jlink.close()
        print("JLink disconnected, exiting...")
    except KeyboardInterrupt:
        print("ctrl-c detected, exiting...")
        jlink.close()
        pass

def erase_external_flash(target_device, jlink_speed="auto"):
    jlink = pylink.JLink()
    print("Connecting to JLink...")
    jlink.open()
    print("Connecting to %s..." % target_device)
    jlink.set_tif(pylink.enums.JLinkInterfaces.SWD)

    if jlink_speed != "auto" and jlink_speed != "adaptive":
        jlink_speed = int(jlink_speed)
    jlink.connect(target_device, speed=jlink_speed)
    print("Connected, send RTT_FLASH_ERASE_EXTERNAL_FLASH to RAM_ADDR...")
    jlink.reset(0, False)
    jlink.memory_write32(RAM_ADDR, [RTT_FLASH_ERASE_EXTERNAL_FLASH])
    if jlink.memory_read32(RAM_ADDR, 1)[0] != RTT_FLASH_ERASE_EXTERNAL_FLASH:
        print("Could not write to RAM_ADDR, exiting...")
        sys.exit(1)
    print("Done. Reset into flash loader mode...")
    jlink.reset(0, False)
    print("Done. Wait for watch ti reboot...")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Open RTT console.")
    parser.add_argument(
        "--target_cpu",
        default="nRF5340_XXAA",
        help="Device Name (see https://www.segger.com/supported-devices/jlink/)",
    )
    parser.add_argument(
        "--file",
        help="Binary file to send to target or in case of read the filename to store the data in.",
    )
    parser.add_argument(
        "-p",
        "--partition",
        type=str,
        help="Label of partition in DTS to write to.",
        default="lvgl_raw_partition",
    )
    parser.add_argument(
        "--read_data", help="Read data from flash block", action="store_true"
    )
    parser.add_argument(
        "-s", "--serial", type=int, help="Serial number of ZSWatch attached debugger"
    )
    parser.add_argument(
        "--speed",
        help="JLink speed (int): connection speed in kHz, one of {5-12000, 'auto', 'adaptive'}",
        default="auto",
        required=False,
    )

    args = parser.parse_args()

    sys.exit(
        rtt_run_flush_loader(
            args.target_cpu, args.file, args.partition, args.speed, args.read_data
        )
    )
