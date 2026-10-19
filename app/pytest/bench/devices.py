"""Device entries from devices.yaml, shared by pytest and the bench tools."""

import os
from pathlib import Path

import yaml

PYTEST_DIR = Path(__file__).resolve().parents[1]


def load_devices() -> dict:
    """Return all device entries; devices_local.yaml replaces devices.yaml when present."""
    local = PYTEST_DIR / "devices_local.yaml"
    path = local if local.exists() else PYTEST_DIR / "devices.yaml"
    with open(path) as f:
        return yaml.safe_load(f)["devices"]


def get_device(name: str | None = None) -> dict:
    """Return one device entry with its name and board filled in.

    The name comes from the argument, then ZSW_DEVICE. Without either, the only
    bench_only entry is used, so a single desk setup needs no arguments.
    """
    devices = load_devices()
    name = name or os.environ.get("ZSW_DEVICE")
    if not name:
        bench = [key for key, cfg in devices.items() if cfg.get("bench_only")]
        if len(bench) != 1:
            raise SystemExit(f"Pick a device with --device or ZSW_DEVICE: {', '.join(devices)}")
        name = bench[0]
    if name not in devices:
        raise SystemExit(f"Unknown device '{name}', expected one of: {', '.join(devices)}")
    cfg = dict(devices[name])
    cfg["name"] = name
    cfg.setdefault("board", name)
    return cfg


def require(device: dict, key: str) -> str:
    value = device.get(key)
    if not value:
        raise SystemExit(f"Device '{device['name']}' has no '{key}' in devices.yaml")
    return str(value)
