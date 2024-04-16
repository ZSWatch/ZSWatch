from west.commands import WestCommand
from west import log
import sys
import os
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
            required=True,
            help="The coredump text file to analyse.",
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

        return parser

    def do_run(self, args, unknown_args):
        log.inf("Running coredump analyzis")
        toolchain_path = ""
        elf_file = ""

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
            toolchain_path = self.find_toolchain_path(args.build_dir)
            elf_file = f"{args.build_dir}/zephyr/zephyr.elf"
        else:
            toolchain_path = args.toolchain
            elf_file = args.elf

        gdb_path = toolchain_path + "/arm-zephyr-eabi/bin/arm-zephyr-eabi-gdb"

        if toolchain_path == "" or not Path(gdb_path).exists():
            log.err("Toolchain not found", gdb_path)
            sys.exit(1)

        if elf_file == "" or not Path(elf_file).exists():
            log.err("Elf file not found", elf_file)
            sys.exit(1)

        coredump_bin_file_path = f"{ZEPHYR_BASE.parent.absolute()}/coredump.bin"
        self.convert_coredump_to_bin(args.coredump_file, coredump_bin_file_path)
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
            f'{gdb_path} {elf_file} quiet -ex "set confirm off" -ex "set target-charset ASCII" -ex "target remote localhost:1234" -ex "bt" -ex "info registers" -ex quit'
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
        with open(build_folder + "/CMakeCache.txt") as f:
            for line in f:
                if "ZEPHYR_SDK_INSTALL_DIR:INTERNAL=" in line:
                    return line.split("=")[1].strip()
