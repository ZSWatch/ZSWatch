#!/usr/bin/env python3
"""Capture serial output to stdout for a fixed duration."""
import sys
import time
import serial
import argparse

p = argparse.ArgumentParser()
p.add_argument("--port", default="/dev/ttyUSB1")
p.add_argument("--baud", type=int, default=115200)
p.add_argument("--seconds", type=float, default=15.0)
args = p.parse_args()

ser = serial.Serial(args.port, args.baud, timeout=0.2)
end = time.time() + args.seconds
buf = b""
while time.time() < end:
    chunk = ser.read(4096)
    if chunk:
        buf += chunk
        sys.stdout.buffer.write(chunk)
        sys.stdout.buffer.flush()
ser.close()
