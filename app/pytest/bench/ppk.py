"""PPK2 as the watch's battery: keep one session open and measure on request.

Opening the PPK2 again toggles the supply, which reboots the watch and ruins a
matched comparison. `serve` holds the session so flashing, BLE commands and
captures can run in between without power changing.

From app/pytest:

    python -m bench.ppk serve &                     # supply on, keeps running
    python -m bench.ppk measure --seconds 60 --label idle
    python -m bench.ppk stream --seconds 900 --label background --out results/
    python -m bench.ppk cycle --off-seconds 3       # power cycle the watch

Library use: `ppk_request("measure", seconds=30)` talks to a running server.
"""

import argparse
import json
import logging
import os
import signal
import socket
import sys
import time
from pathlib import Path

from bench.devices import get_device, require
from ppk2_helper import AlignedDecoder, PPK2Manager, analyze_current_samples

log = logging.getLogger(__name__)

SOCKET = Path(os.environ.get("ZSW_PPK_SOCKET", Path(os.environ.get("XDG_RUNTIME_DIR", "/tmp")) / "zsw-ppk2.sock"))


def open_supply(port: str, voltage_mv: int) -> PPK2Manager:
    """Open the PPK2 in source mode with the output on and calibration loaded."""
    manager = PPK2Manager(port, voltage_mv)
    manager.setup()
    manager.ensure_calibrated()
    if not manager._modifiers_loaded:
        raise RuntimeError("PPK2 calibration unavailable")
    return manager


def stream_measure(manager: PPK2Manager, seconds: float, label: str, out: Path | None = None) -> dict:
    """Average full-rate current in one-second bins without keeping every sample.

    With out set, the bins are rewritten to <out>/<label>-stream.json every second,
    so a long capture keeps its data if it is interrupted.
    """
    ppk = manager.ppk2
    manager.ensure_calibrated()
    ppk.start_measuring()
    time.sleep(0.1)
    decoder = AlignedDecoder(ppk)
    decoder.feed(ppk.get_data() or b"")  # settle; discard
    started = time.time()
    start = bin_start = time.monotonic()
    bins = []
    total = count = partial = partial_n = 0
    minimum, maximum = float("inf"), 0.0
    path = out / f"{label}-stream.json" if out else None

    def close_bin(now):
        bins.append({"end_seconds": now - start, "average_ma": partial / partial_n / 1000,
                     "sample_count": partial_n})

    try:
        while time.monotonic() - start < seconds:
            raw = ppk.get_data()
            if raw:
                values = decoder.feed(raw)
                if values:
                    total += sum(values)
                    count += len(values)
                    partial += sum(values)
                    partial_n += len(values)
                    minimum = min(minimum, min(values))
                    maximum = max(maximum, max(values))
            now = time.monotonic()
            if now - bin_start >= 1 and partial_n:
                close_bin(now)
                partial = partial_n = 0
                bin_start = now
                if path:
                    path.write_text(json.dumps({"label": label, "started": started, "bins": bins}))
            time.sleep(0.005)
    finally:
        ppk.stop_measuring()
    if partial_n:
        close_bin(time.monotonic())
    if not count:
        raise RuntimeError("No PPK2 samples")
    elapsed = time.monotonic() - start
    result = {"label": label, "voltage_mv": manager.voltage_mv, "started": started,
              "elapsed_seconds": elapsed, "average_ma": total / count / 1000,
              "min_ma": minimum / 1000, "max_ma": maximum / 1000, "sample_count": count,
              "charge_mah": total / count / 1000 * elapsed / 3600,
              "realigned": decoder.realigned}
    if path:
        path.write_text(json.dumps(dict(result, bins=bins), indent=2))
    return result


def _handle(manager: PPK2Manager, request: dict) -> dict:
    action = request["action"]
    if action in ("stream", "measure"):
        manager.flush()
    out = Path(request["out"]) if request.get("out") else None
    if out:
        out.mkdir(parents=True, exist_ok=True)
    if action == "cycle":
        off_seconds = float(request.get("off_seconds", 2))
        if not 1 <= off_seconds <= 30:
            raise ValueError("Power-off time must be 1-30 seconds")
        manager.power_cycle_device(off_seconds)
        return {"voltage_mv": manager.voltage_mv, "output": "ON", "off_seconds": off_seconds}
    if action == "stream":
        return stream_measure(manager, float(request.get("seconds", 900)),
                              request.get("label", "stream"), out)
    if action == "measure":
        seconds = float(request.get("seconds", 30))
        samples = manager.measure_current(seconds)
        if not samples:
            raise RuntimeError("No PPK2 samples")
        result = dict(label=request.get("label", "measurement"), voltage_mv=manager.voltage_mv,
                      requested_seconds=seconds, timestamp=time.time(), **analyze_current_samples(samples))
        if out:
            (out / (time.strftime("%Y%m%d-%H%M%S-") + result["label"] + ".json")).write_text(
                json.dumps(result, indent=2))
        return result
    return {"voltage_mv": manager.voltage_mv, "output": "ON"}


def serve(port: str, voltage_mv: int) -> None:
    manager = open_supply(port, voltage_mv)
    # Clean up on kill as well as Ctrl-C.
    signal.signal(signal.SIGTERM, lambda *_: sys.exit(0))
    SOCKET.unlink(missing_ok=True)
    server = socket.socket(socket.AF_UNIX)
    server.bind(str(SOCKET))
    server.listen(4)
    print(f"PPK2 ready: {voltage_mv} mV, output on, socket {SOCKET}", flush=True)
    try:
        while True:
            conn, _ = server.accept()
            with conn:
                data = b""
                while not data.endswith(b"\n"):
                    chunk = conn.recv(4096)
                    if not chunk:
                        break
                    data += chunk
                try:
                    result = _handle(manager, json.loads(data))
                except Exception as exc:
                    result = {"error": str(exc)}
                print(json.dumps(result), flush=True)
                try:
                    conn.sendall(json.dumps(result).encode() + b"\n")
                except OSError:
                    pass  # The client gave up; keep serving.
    finally:
        # Leave the output on: the PPK2 is the watch's battery.
        manager.ppk2.stop_measuring()
        manager.ppk2.ser.close()
        server.close()
        SOCKET.unlink(missing_ok=True)


def ppk_request(action: str, timeout_s: float | None = None, **params) -> dict:
    """Send one request to a running `serve` and return its result."""
    seconds = float(params.get("seconds", 0))
    with socket.socket(socket.AF_UNIX) as client:
        client.settimeout(timeout_s or seconds + 60)
        client.connect(str(SOCKET))
        client.sendall(json.dumps(dict(params, action=action)).encode() + b"\n")
        data = b""
        while not data.endswith(b"\n"):
            chunk = client.recv(65536)
            if not chunk:
                break
            data += chunk
    result = json.loads(data)
    if "error" in result:
        raise RuntimeError(result["error"])
    return result


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("action", choices=["serve", "status", "measure", "stream", "cycle"])
    parser.add_argument("--device", help="Entry in devices.yaml (default: ZSW_DEVICE or the only bench entry)")
    parser.add_argument("--seconds", type=float, default=30)
    parser.add_argument("--label", default="measurement")
    parser.add_argument("--off-seconds", type=float, default=2)
    parser.add_argument("--out", help="Directory for result JSON files")
    args = parser.parse_args()
    logging.basicConfig(level=logging.INFO, format="%(message)s")
    if args.action == "serve":
        device = get_device(args.device)
        serve(require(device, "ppk2_port"), int(device.get("ppk2_voltage_mv", 4100)))
        return
    params = {"seconds": args.seconds, "label": args.label, "off_seconds": args.off_seconds}
    if args.out:
        params["out"] = str(Path(args.out).resolve())
    print(json.dumps(ppk_request(args.action, **params), indent=2))


if __name__ == "__main__":
    main()
