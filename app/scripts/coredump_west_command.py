# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

from west.commands import WestCommand
from west import log
import sys
import os
import time
from pathlib import Path
from west.configuration import config
import subprocess

THIS_ZEPHYR = Path(__file__).parent.parent.parent / "zephyr"
ZEPHYR_BASE = Path(os.environ.get("ZEPHYR_BASE", THIS_ZEPHYR))


class CoredumpWestCommand(WestCommand):
    def __init__(self):
        super().__init__(
            "coredump",
            "Analyze the coredump",
            """Use to analyse a coredump.""",
        )

        log.inf("Zephyr Base", ZEPHYR_BASE)

    def do_add_parser(self, parser_adder):
        parser = parser_adder.add_parser(
            self.name, help=self.help, description=self.description
        )

        parser.add_argument(
            "--coredump_file",
            type=str,
            default="",
            required=False,
            help="The coredump text file to analyse, of not provided logs will be retreived over Debugger and RTT.",
        )

        parser.add_argument(
            "--build_dir",
            type=str,
            default="",
            required=False,
            help="Build directory of ZSWatch firmware. Optional, if not privided must specify elf file. By specifying build dir, toolchain and elf can be extracted.",
        )

        parser.add_argument(
            "--elf",
            type=str,
            default="",
            required=False,
            help="Zephyr .elf file to use for symbol resolution if not build_dir is provided.",
        )

        parser.add_argument(
            "--toolchain",
            type=str,
            default="",
            required=False,
            help="Toolchain directory needed if --build_dir is not provided. For example /home/user/ncs/toolchains/7795df4459/opt/zephyr-sdk",
        )

        parser.add_argument(
            "--serial_port",
            type=str,
            default="",
            required=False,
            help="Serial port to read coredump from UART instead of RTT. For example /dev/ttyACM0 or /dev/ttyUSB0.",
        )

        parser.add_argument(
            "--baudrate",
            type=int,
            default=115200,
            required=False,
            help="Baudrate for serial port (default: 115200).",
        )

        return parser

    def do_run(self, args, unknown_args):
        log.inf("Running coredump analyzis")
        toolchain_path = ""
        elf_file = ""
        coredump_file = ""

        if (args.build_dir == "" and args.elf == "") or (
            args.build_dir != "" and args.elf != ""
        ):
            log.err("Either --build_dir or --elf must be provided")
            sys.exit(1)

        if (args.toolchain == "" and args.build_dir == "") or (
            args.toolchain != "" and args.build_dir != ""
        ):
            log.err("Either --toolchain or --build_dir must be provided")
            sys.exit(1)

        if args.build_dir != "":
            toolchain_path = self.find_toolchain_path(args.build_dir + "/app")
            elf_file = f"{args.build_dir}/app/zephyr/zephyr.elf"
        else:
            toolchain_path = args.toolchain
            elf_file = args.elf

        if toolchain_path is None:
            log.err(
                "Toolchain not found (ZEPHYR_SDK_INSTALL_DIR not found in CMakeCache.txt)"
            )
            sys.exit(1)

        gdb_path = toolchain_path + "/arm-zephyr-eabi/bin/arm-zephyr-eabi-gdb"

        if not Path(gdb_path).exists():
            log.err("GDB not found at", gdb_path)
            sys.exit(1)

        if not Path(elf_file).exists():
            log.err("Elf file not found", elf_file)
            sys.exit(1)

        if args.coredump_file == "":
            if args.serial_port != "":
                coredump_file = self.extract_coredump_over_uart(args.serial_port, args.baudrate)
                print("Coredump extracted from UART, saving to coredump_uart.txt")
                with open("coredump_uart.txt", "w") as f:
                    f.write(coredump_file)
                    coredump_file = "coredump_uart.txt"
            else:
                coredump_file = self.extract_coredump_over_rtt()
                print("Coredump extracted from RTT, saving to coredump_rtt.txt")
                with open("coredump_rtt.txt", "w") as f:
                    f.write(coredump_file)
                    coredump_file = "coredump_rtt.txt"
        else:
            coredump_file = args.coredump_file

        coredump_bin_file_path = f"{ZEPHYR_BASE.parent.absolute()}/coredump.bin"
        self.convert_coredump_to_bin(coredump_file, coredump_bin_file_path)
        proc = self.create_gdb_server(coredump_bin_file_path, elf_file)
        self.gdb_get_bt(gdb_path, elf_file, coredump_bin_file_path)
        proc.terminate()

    def convert_coredump_to_bin(self, coredump_txt_file, output_file_path):
        os.system(
            f"{ZEPHYR_BASE.absolute()}/scripts/coredump/coredump_serial_log_parser.py {coredump_txt_file} {output_file_path}"
        )

    def create_gdb_server(self, coredump_bin_file, elf_file):
        return subprocess.Popen(
            [
                f"{ZEPHYR_BASE.absolute()}/scripts/coredump/coredump_gdbserver.py",
                elf_file,
                coredump_bin_file,
            ]
        )

    def gdb_get_bt(self, gdb_path, elf_file, coredump_bin_file):
        """
        Runs GDB to retrieve (print out) the backtrace (bt) and register information from a coredump.

        Args:
            gdb_path (str): The path to the GDB executable.
            elf_file (str): The path to the ELF file.
            coredump_bin_file (str): The path to the coredump binary file.

        Returns:
            None
        """
        os.system(
            f'{gdb_path} -q {elf_file} -ex "set confirm off" -ex "set target-charset ASCII" -ex "target remote localhost:1234" -ex "bt" -ex "info registers" -ex quit'
        )

    def find_toolchain_path(self, build_folder):
        """
        Finds the path of the Zephyr SDK toolchain installation directory.

        Looks for the ZEPHYR_SDK_INSTALL_DIR variable in the CMakeCache.txt file

        Args:
            build_folder (str): The path to the build folder.

        Returns:
            str: The path of the Zephyr SDK toolchain installation directory.

        """
        cmake_cache_path = build_folder + "/CMakeCache.txt"
        if not Path(cmake_cache_path).exists():
            return None
        with open(cmake_cache_path) as f:
            for line in f:
                if "ZEPHYR_SDK_INSTALL_DIR:INTERNAL=" in line:
                    return line.split("=")[1].strip()
        return None

    def extract_coredump_over_rtt(self, target_device="nRF5340_XXAA"):
        try:
            import pylink
        except ImportError as e:
            log.err(f"Failed to import pylink for RTT coredump extraction: {e}")
            sys.exit(1)

        jlink = pylink.JLink()
        print("Connecting to JLink...")
        jlink.open()
        print("Connecting to %s..." % target_device)
        jlink.set_tif(pylink.enums.JLinkInterfaces.SWD)
        jlink.connect(target_device)
        jlink.rtt_start(None)
        print("This only works for ZSWatch debug builds with RTT as log backend")
        print("Connected, go to settings -> other -> Dump coredump over log")

        def read_data():
            read_bytes = jlink.rtt_read(0, 1024)
            if read_bytes and read_bytes != "":
                return "".join(map(chr, read_bytes))
            return None

        def is_connected():
            return jlink.connected()

        return self._parse_coredump_stream(read_data, is_connected)

    def extract_coredump_over_uart(self, serial_port, baudrate=115200):
        try:
            import serial
        except ImportError as e:
            log.err(f"Failed to import pyserial for UART coredump extraction: {e}")
            sys.exit(1)

        print(f"Opening serial port {serial_port} at {baudrate} baud...")
        try:
            ser = serial.Serial(serial_port, baudrate, timeout=1)
        except serial.SerialException as e:
            log.err(f"Failed to open serial port {serial_port}: {e}")
            sys.exit(1)

        print("Connected, go to settings -> other -> Dump coredump over log")

        def read_data():
            read_bytes = ser.read(1024)
            if read_bytes and read_bytes != b"":
                return read_bytes.decode("utf-8", errors="ignore")
            return None

        def is_connected():
            return True

        try:
            return self._parse_coredump_stream(read_data, is_connected)
        finally:
            ser.close()

    def _parse_coredump_stream(self, read_data_func, is_connected_func):
        coredump_content = ""
        found_start = False

        cb_start = "#CD:BEGIN#"
        cb_end = "#CD:END#"

        try:
            while is_connected_func():
                data = read_data_func()
                if data:
                    print(data, end="")
                    start_index = data.find(cb_start)
                    end_index = data.find(cb_end)

                    if start_index != -1 and end_index != -1:
                        print("\nFull coredump in same buffer")
                        coredump_content += data[start_index : (end_index + len(cb_end))]
                        break
                    elif start_index >= 0:
                        print("\nFound start")
                        coredump_content = data[start_index:]
                        found_start = True
                    elif found_start and end_index >= 0:
                        print("\nDone got both start and end")
                        coredump_content += data[: (end_index + len(cb_end))]
                        break
                    elif found_start and end_index == -1:
                        coredump_content += data
                    elif not found_start and end_index != -1:
                        print("\nFound end before start, skipping")
                        coredump_content = ""
                        found_start = False

                time.sleep(0.1)
        except KeyboardInterrupt:
            print("\nInterrupted by user")

        return coredump_content
