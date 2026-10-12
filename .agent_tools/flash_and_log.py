#!/usr/bin/env python3
"""Explicitly flash/reset one selected probe and capture UART logs.

No hardware access on --help or invalid arguments. Mac USB bootstrap should use
the app's watch_usb.py with SEGGER Commander; this helper requires nrfjprog.
"""
import argparse
import math
from pathlib import Path
import re
import shutil
import serial
import subprocess
import sys
import threading
import time

def uart_reader(ser, duration, output_lines, errors):
    """Read from UART port for duration seconds, storing lines."""
    try:
        end_time = time.monotonic() + duration
        while time.monotonic() < end_time:
            line = ser.readline()
            if line:
                decoded = line.decode('utf-8', errors='replace').strip()
                if decoded:
                    output_lines.append(decoded)
                    print(f"[UART] {decoded}", flush=True)
    except Exception as e:
        errors.append(str(e))
        print(f"[UART ERROR] {e}")

def positive_seconds(value):
    seconds = float(value)
    if not math.isfinite(seconds) or seconds <= 0:
        raise argparse.ArgumentTypeError("seconds must be positive and finite")
    return seconds

def arguments(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--port", required=True, help="Explicit UART port, not USB SMP")
    parser.add_argument("--probe", required=True, help="Explicit debugger serial number")
    parser.add_argument("--family", choices=("NRF51", "NRF52", "NRF53", "NRF91"), default="NRF53")
    parser.add_argument("--seconds", type=positive_seconds, default=15)
    action = parser.add_mutually_exclusive_group(required=True)
    action.add_argument("--flash", type=Path, metavar="HEX", help="Existing matching firmware .hex")
    action.add_argument("--reset", action="store_true", help="Reset selected target without flashing")
    args = parser.parse_args(argv)
    if not re.fullmatch(r"[0-9]+", args.probe):
        parser.error("--probe must be a numeric debugger serial number")
    if args.flash and (not args.flash.is_file() or args.flash.suffix.lower() != ".hex"):
        parser.error("--flash requires an existing .hex file; no hardware was accessed")
    return args

def command_for(args, executable):
    action = ["--program", str(args.flash.resolve()), "--sectorerase", "--reset"] if args.flash else ["--reset"]
    return [executable, *action, "-f", args.family, "--snr", args.probe]

def main(argv=None):
    args = arguments(argv)
    executable = shutil.which("nrfjprog")
    if not executable:
        print("ERROR: nrfjprog is not installed; no hardware was accessed. "
              "For Mac USB bootstrap, use watch_usb.py with SEGGER JLinkExe.", file=sys.stderr)
        return 1

    lines = []
    errors = []
    # Verify the passive log port before mutating hardware; leave modem lines off.
    ser = serial.Serial(port=None, baudrate=115200, timeout=0.2, exclusive=True)
    ser.dtr = False
    ser.rts = False
    ser.port = args.port
    try:
        ser.open()
    except (OSError, serial.SerialException) as error:
        print(f"ERROR: UART unavailable; no action performed: {error}", file=sys.stderr)
        return 1
    reader = threading.Thread(target=uart_reader, args=(ser, args.seconds, lines, errors))
    reader.start()
    try:
        print(f"[ACTION] {'Flash' if args.flash else 'Reset'} selected probe {args.probe}", flush=True)
        result = subprocess.run(
            command_for(args, executable), capture_output=True, text=True, timeout=120
        )
        print(f"[ACTION] Exit code: {result.returncode}")
        if result.stdout.strip():
            print(result.stdout.strip())
        if result.returncode != 0 and result.stderr.strip():
            print(result.stderr.strip(), file=sys.stderr)
    except (OSError, subprocess.TimeoutExpired) as error:
        print(f"ERROR: {error}", file=sys.stderr)
        return 1
    finally:
        reader.join()
        ser.close()
    print(f"\n[SUMMARY] Captured {len(lines)} log lines; not firmware/BLE acceptance")
    return result.returncode or (0 if lines and not errors else 1)

if __name__ == "__main__":
    sys.exit(main())
