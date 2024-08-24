from west.commands import WestCommand
from west import log
from create_custom_resource_image import create_custom_raw_fs_image
from rtt_flash_loader import rtt_run_flush_loader, erase_external_flash
from create_littlefs_resouce_image import create_littlefs_fs_image
import sys
import os
from pathlib import Path


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
            type=str,
            help="JLink serial number",
            default=None,
            required=False,
        )

        return parser

    def do_run(self, args, unknown_args):
        if args.erase:
            sys.exit(erase_external_flash("nRF5340_XXAA"))
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
        print(images_path)
        if args.read_file:
            filename = args.read_file
            partition = partition if partition else "littlefs_storage"
        if args.read_file is None:
            if args.type == "raw":
                source_dir = f"{images_path}/S"
                partition = partition if partition else "lvgl_raw_partition"
                create_custom_raw_fs_image(filename, source_dir, block_size)
            elif args.type == "lfs":
                source_dir = f"{images_path}/lvgl_lfs"
                partition = partition if partition else "littlefs_storage"
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
        log.inf("Uploading image")
        sys.exit(
            rtt_run_flush_loader(
                "nRF5340_XXAA",
                filename,
                partition,
                args.speed,
                args.read_file,
                args.serial_number,
            )
        )
