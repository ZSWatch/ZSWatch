"""Run a command inside the nRF Connect SDK toolchain without sourcing anything.

The toolchain bundle describes its own environment in environment.json; this
applies it, sets ZEPHYR_BASE for this checkout and drops any active virtualenv
so the toolchain's Python and west are used.

From app/pytest:

    python -m bench.toolchain west build -b watchdk@1/nrf5340/cpuapp ../ -d ../build
    python -m bench.toolchain nrfjprog --ids
    eval "$(python -m bench.toolchain --print)"     # export into the current shell

The bundle is ZSW_NCS_TOOLCHAIN, or the newest one in ~/ncs/toolchains.
"""

import argparse
import json
import os
import shlex
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[3]


def _version_key(version: str) -> tuple:
    return tuple(int(part) if part.isdigit() else 0 for part in version.lstrip("v").split("."))


def find_toolchain() -> Path:
    if os.environ.get("ZSW_NCS_TOOLCHAIN"):
        return Path(os.environ["ZSW_NCS_TOOLCHAIN"])
    root = Path.home() / "ncs/toolchains"
    index = root / "toolchains.json"
    if not index.exists():
        raise SystemExit(f"No {index}; set ZSW_NCS_TOOLCHAIN to a toolchain bundle")
    bundles = []
    for group in json.loads(index.read_text()):
        for entry in group["toolchains"]:
            path = root / entry["identifier"]["bundle_id"]
            if (path / "environment.json").exists():
                bundles.append((max(_version_key(v) for v in entry["ncs_versions"]), path))
    if not bundles:
        raise SystemExit(f"No usable toolchain bundle in {root}")
    return max(bundles)[1]


def toolchain_env(base: dict | None = None) -> dict:
    """Return a copy of base (default os.environ) with the toolchain applied."""
    bundle = find_toolchain()
    env = dict(os.environ if base is None else base)
    venv = env.pop("VIRTUAL_ENV", None)
    if venv:
        env["PATH"] = os.pathsep.join(p for p in env.get("PATH", "").split(os.pathsep)
                                      if not p.startswith(venv))
    for var in json.loads((bundle / "environment.json").read_text())["env_vars"]:
        if var["type"] == "string":
            env[var["key"]] = var["value"]
            continue
        value = os.pathsep.join(str(bundle / p) for p in var["values"])
        if var.get("existing_value_treatment") == "prepend_to" and env.get(var["key"]):
            value += os.pathsep + env[var["key"]]
        env[var["key"]] = value
    if (REPO_ROOT / "zephyr").is_dir():
        env["ZEPHYR_BASE"] = str(REPO_ROOT / "zephyr")
    return env


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--print", action="store_true", help="Print export lines instead of running")
    parser.add_argument("command", nargs=argparse.REMAINDER)
    args = parser.parse_args()
    env = toolchain_env()
    if args.print:
        for key in ("PATH", "LD_LIBRARY_PATH", "PYTHONHOME", "PYTHONPATH", "GIT_EXEC_PATH",
                    "GIT_TEMPLATE_DIR", "ZEPHYR_TOOLCHAIN_VARIANT", "ZEPHYR_SDK_INSTALL_DIR", "ZEPHYR_BASE"):
            if key in env:
                print(f"export {key}={shlex.quote(env[key])}")
        print("unset VIRTUAL_ENV")
        return
    if not args.command:
        parser.error("Give a command to run, or --print")
    command = args.command
    if command[0] == "west":
        # The bundle's west script needs the bundle's interpreter.
        command = ["python3", "-m", "west", *command[1:]]
    os.execvpe(command[0], command, env)


if __name__ == "__main__":
    sys.exit(main())
