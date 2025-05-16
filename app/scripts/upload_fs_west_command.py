from west.commands import WestCommand
from west import log
from create_custom_resource_image import create_custom_raw_fs_image
from rtt_flash_loader import rtt_run_flush_loader, erase_external_flash
from create_littlefs_resouce_image import create_littlefs_fs_image
import sys
import os
from pathlib import Path
from pynrfjprog import HighLevel
import intelhex


class UploadFsWestCommand(WestCommand):
    def __init__(self):
        super().__init__(
            "upload_fs",
            "Upload the LVGL filesystem to external SPI Flash",
            """Use to upload files to the external SPI flash.""",
        )

    def do_add_parser(self, parser_adder):
        parser = parser_adder.add_parser(
            self.name, help=self.help, description=self.description
        )

        parser.add_argument(
            "--erase",
            action="store_true",
            help="Erase the external (Q)SPI Flash",
        )

        parser.add_argument(
            "--type",
            type=str,
            default="raw",
            help="raw or fs. fs to load littlefs image, raw to load custom binary",
        )

        parser.add_argument(
            "--read_file", type=str, help="If set dump flash to this filename"
        )

        parser.add_argument(
            "-p",
            "--partition",
            type=str,
            help="Label of partition in DTS to write to. Leave blank to use auto guess name.",
        )

        parser.add_argument(
            "--speed",
            help="JLink speed (int): connection speed in kHz, one of {5-12000, 'auto', 'adaptive'}",
            default="auto",
            required=False,
        )

        parser.add_argument(
            "-s",
            "--serial_number",
            type=int,
            help="JLink serial number",
            default=None,
            required=False,
        )

        parser.add_argument(
            "-ini",
            "--ini_file",
            type=str,
            help="nrfjprog qspsi ini file",
            default="app/qspi_mx25u51245.ini",
        )

        parser.add_argument(
            "--use_rtt",
            action="store_true",
            help="Upload using RTT, needed for v3 watches without QSPI flash",
        )

        parser.add_argument(
            "--generate_only",
            action="store_true",
            help="Only generate the image, do not upload it.",
        )

        return parser

    def prompt_for_serial_number(self):
        serial_number = None
        with HighLevel.API() as api:
            probes = api.get_connected_probes()
            if len(probes) > 1:
                print("# More than one device found, please specify which one to use.")
                for i, probe in enumerate(probes):
                    print(f"{i+1}: {probe}")
                probe_index = input("Enter the number of the device to use: ")
                try:
                    probe_index = int(probe_index)
                    if probe_index < 1 or probe_index > len(probes):
                        print("Invalid probe number. Please enter a valid number.")
                    else:
                        serial_number = probes[probe_index - 1]
                except ValueError:
                    print("Invalid probe number. Please enter a valid number.")

            else:
                serial_number = probes[0]

        return serial_number

    def write_to_qspi_flash(self, serial_number, hex_file, ini_file=None, speed=4000):
        if serial_number is None:
            serial_number = self.prompt_for_serial_number()

        if serial_number is None:
            print("No serial number provided and or no probe found.")
            return

        with HighLevel.API() as api:
            with HighLevel.DebugProbe(api, serial_number) as probe:
                print("# Setting up the probe to qspi.")
                probe.setup_qspi_with_ini(ini_file)
                program_options = HighLevel.ProgramOptions(
                    erase_action=HighLevel.EraseAction.ERASE_SECTOR,
                    qspi_erase_action=HighLevel.EraseAction.ERASE_SECTOR,
                    reset=HighLevel.ResetAction.RESET_SYSTEM,
                    verify=HighLevel.VerifyAction.VERIFY_READ,
                )

                print("# Programming:", hex_file)
                probe.program(hex_file, program_options=program_options)
                print("# Programming done.")

    def erase_qspi_flash(self, serial_number, ini_file):
        if serial_number is None:
            serial_number = self.prompt_for_serial_number()

        if serial_number is None:
            print("No serial number provided and or no probe found.")
            return

        print("# Erasing QSPI flash. This may take over a minute...")
        with HighLevel.API() as api:
            with HighLevel.DebugProbe(api, serial_number) as probe:
                print("# Setting up the probe to qspi.")
                probe.setup_qspi_with_ini(ini_file)
                probe.erase(HighLevel.EraseAction.ERASE_ALL, 0x10000000)

                print("# Programming done.")

    def do_run(self, args, unknown_args):
        if args.erase:
            sys.exit(self.erase_qspi_flash(args.serial_number, args.ini_file))
            return
        log.inf("Creating image")
        img_size = 2 * 1024 * 1024
        block_size = 4096
        read_size = 1024
        prog_size = 512
        name_max = 255
        file_max = 0
        attr_max = 0
        disk_version = "2.0"
        filename = "lvgl_resources"
        partition = args.partition
        zephyr_base = Path(os.environ.get("ZEPHYR_BASE"))
        images_path = f"{zephyr_base.parent.absolute()}/app/src/images/binaries"
        qspi_flash_address = 0x10000000
        print(images_path)
        if args.read_file:
            filename = args.read_file
            partition = partition if partition else "littlefs_storage"
        if args.read_file is None:
            if args.type == "raw":
                source_dir = f"{images_path}/S"
                partition = partition if partition else "lvgl_raw_partition"
                create_custom_raw_fs_image(filename, source_dir, block_size)
                qspi_flash_address = qspi_flash_address + 0x3A0000
            elif args.type == "lfs":
                source_dir = f"{images_path}/lvgl_lfs"
                partition = partition if partition else "littlefs_storage"
                qspi_flash_address = qspi_flash_address + 0x1a0000
                create_littlefs_fs_image(
                    filename,
                    img_size,
                    block_size,
                    read_size,
                    prog_size,
                    name_max,
                    file_max,
                    attr_max,
                    source_dir,
                    disk_version,
                )

        # Convert file to Intel Hex file
        hex_file = filename + ".hex"
        ih = intelhex.IntelHex()
        ih.loadbin(filename, qspi_flash_address)
        ih.tofile(hex_file, format="hex")

        log.inf("Uploading image")
        if args.use_rtt:
            sys.exit(
                rtt_run_flush_loader(
                    "nRF5340_XXAA",
                    filename,
                    partition,
                    args.speed,
                    args.read_file,
                    str(args.serial_number),
                )
            )
        else:
            speed = None if args.speed == 'auto' else int(args.speed)
            if (args.generate_only):
                print(f"Generated {hex_file} with size {os.path.getsize(hex_file)}")
                return 0
            else:
                sys.exit(self.write_to_qspi_flash(args.serial_number, hex_file, args.ini_file, speed))
