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
            "--file",
            type=str,
            default="",
            required=True,
            help="The coredump file to analyse",
        )

        parser.add_argument(
            "--elf",
            type=str,
            default="",
            required=True,
            help="Zephyr .elf file to use for symbol resolution",
        )

        return parser

    def do_run(self, args, unknown_args):
        log.inf("Running coredump analyzis")

        coredump_bin_file_path = f"{ZEPHYR_BASE.parent.absolute()}/coredump.bin"
        self.convert_coredump_to_bin(args.file, coredump_bin_file_path)
        proc = self.create_gdb_server(coredump_bin_file_path, args.elf)
        self.gdb_get_bt(args.elf, coredump_bin_file_path)
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

    def gdb_get_bt(self, elf_file, coredump_bin_file):
        gdb_path = "/home/jakkra/ncs/toolchains/7795df4459/opt/zephyr-sdk/arm-zephyr-eabi/bin/arm-zephyr-eabi-gdb"
        os.system(
            f'{gdb_path} {elf_file} quiet -ex "set confirm off" -ex "set target-charset ASCII" -ex "target remote localhost:1234" -ex "bt" -ex "info registers" -ex quit'
        )
