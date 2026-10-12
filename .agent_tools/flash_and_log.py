#!/usr/bin/env python3
"""Flash a hex file and capture UART logs for verification."""
import serial
import subprocess
import sys
import threading
import time

def uart_reader(port, baud, duration, output_lines):
    """Read from UART port for duration seconds, storing lines."""
    try:
        ser = serial.Serial(port, baud, timeout=1)
        end_time = time.time() + duration
        while time.time() < end_time:
            line = ser.readline()
            if line:
                decoded = line.decode('utf-8', errors='replace').strip()
                if decoded:
                    output_lines.append(decoded)
                    print(f"[UART] {decoded}")
        ser.close()
    except Exception as e:
        print(f"[UART ERROR] {e}")

def main():
    hex_file = sys.argv[1] if len(sys.argv) > 1 else None
    port = sys.argv[2] if len(sys.argv) > 2 else "/dev/ttyUSB0"
    duration = int(sys.argv[3]) if len(sys.argv) > 3 else 15

    lines = []
    reader = threading.Thread(target=uart_reader, args=(port, 115200, duration, lines))
    reader.start()

    # Give UART reader a moment to open
    time.sleep(0.5)

    if hex_file:
        print(f"[FLASH] Flashing {hex_file}...")
        result = subprocess.run(
            ["nrfjprog", "--program", hex_file, "--sectorerase", "--reset"],
            capture_output=True, text=True
        )
        print(f"[FLASH] Exit code: {result.returncode}")
        if result.stdout.strip():
            print(f"[FLASH] {result.stdout.strip()}")
        if result.returncode != 0 and result.stderr.strip():
            print(f"[FLASH ERR] {result.stderr.strip()}")
    else:
        print("[RESET] Resetting device...")
        subprocess.run(["nrfjprog", "--reset"], capture_output=True)

    reader.join()
    print(f"\n[SUMMARY] Captured {len(lines)} log lines")
    return 0 if lines else 1

if __name__ == "__main__":
    sys.exit(main())
