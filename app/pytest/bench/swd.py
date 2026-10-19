"""Read firmware variables from a running watch over SWD, by name from the ELF.

Attaching the debugger powers the nRF53 debug domain, which raises the current
until the next power cycle. Read after a capture, never before one.

From app/pytest:

    python -m bench.swd --elf ../build/app/zephyr/zephyr.elf is_stationary motion_state
"""

import argparse
import json

import pylink
from elftools.elf.elffile import ELFFile
from elftools.elf.sections import SymbolTableSection

from bench.devices import get_device, require

APP_CORE = "nRF5340_xxAA_APP"


def elf_symbols(elf_path: str, names: list[str]) -> dict[str, tuple[int, int]]:
    """Return {name: (address, size)} for data symbols in the ELF."""
    wanted = set(names)
    found = {}
    with open(elf_path, "rb") as f:
        for section in ELFFile(f).iter_sections():
            if not isinstance(section, SymbolTableSection):
                continue
            for symbol in section.iter_symbols():
                if symbol.name in wanted and symbol["st_info"]["type"] == "STT_OBJECT":
                    found[symbol.name] = (symbol["st_value"], symbol["st_size"])
    missing = wanted - found.keys()
    if missing:
        raise KeyError(f"Symbols not in {elf_path}: {', '.join(sorted(missing))}")
    return found


def read_symbols(elf_path: str, names: list[str], jlink_serial: str, device: str = APP_CORE) -> dict:
    """Read each symbol's bytes without halting the CPU; integers up to 8 bytes are decoded."""
    symbols = elf_symbols(elf_path, names)
    jlink = pylink.JLink()
    jlink.open(serial_no=int(jlink_serial))
    try:
        jlink.set_tif(pylink.enums.JLinkInterfaces.SWD)
        jlink.connect(device)
        result = {}
        for name in names:
            address, size = symbols[name]
            data = bytes(jlink.memory_read8(address, size))
            entry = {"address": hex(address), "size": size, "hex": data.hex()}
            if size <= 8:
                entry["value"] = int.from_bytes(data, "little")
            result[name] = entry
        return result
    finally:
        jlink.close()


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--device", help="Entry in devices.yaml (default: ZSW_DEVICE or the only bench entry)")
    parser.add_argument("--elf", required=True, help="zephyr.elf of the running firmware")
    parser.add_argument("symbols", nargs="+")
    args = parser.parse_args()
    serial = require(get_device(args.device), "jlink_serial")
    print(json.dumps(read_symbols(args.elf, args.symbols, serial), indent=2))


if __name__ == "__main__":
    main()
