"""Run a command inside the nRF Connect SDK environment.

The environment comes from .nrf_env.sh in the repository root, which the VS
Code task "Get nRF Connect environment setup" writes from the nRF Connect
shell. Set ZSW_NCS_TOOLCHAIN to a toolchain bundle directory to use its
environment.json instead. Either way ZEPHYR_BASE points at this checkout and
any active virtualenv is dropped so the toolchain's Python and west are used.

From app/pytest:

    python -m bench.toolchain west build -b watchdk@1/nrf5340/cpuapp ../ -d ../build
    python -m bench.toolchain nrfjprog --ids
    eval "$(python -m bench.toolchain --print)"     # export into the current shell
"""

import argparse
import json
import os
import shlex
import subprocess
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[3]
NRF_ENV_FILE = REPO_ROOT / ".nrf_env.sh"


def _without_venv(env: dict) -> dict:
    venv = env.pop("VIRTUAL_ENV", None)
    if venv:
        env["PATH"] = os.pathsep.join(p for p in env.get("PATH", "").split(os.pathsep)
                                      if not p.startswith(venv))
    return env


def _nrf_env_file(env: dict) -> dict:
    result = subprocess.run(["bash", "-c", 'source "$1" && env -0', "_", str(NRF_ENV_FILE)],
                            env=env, capture_output=True, check=True)
    pairs = (item.split("=", 1) for item in result.stdout.decode().split("\0") if "=" in item)
    return _without_venv(dict(pairs))


def _bundle(env: dict, bundle: Path) -> dict:
    for var in json.loads((bundle / "environment.json").read_text())["env_vars"]:
        if var["type"] == "string":
            env[var["key"]] = var["value"]
            continue
        value = os.pathsep.join(str(bundle / p) for p in var["values"])
        if var.get("existing_value_treatment") == "prepend_to" and env.get(var["key"]):
            value += os.pathsep + env[var["key"]]
        env[var["key"]] = value
    return env


def toolchain_env(base: dict | None = None) -> dict:
    """Return a copy of base (default os.environ) with the toolchain applied."""
    env = _without_venv(dict(os.environ if base is None else base))
    if env.get("ZSW_NCS_TOOLCHAIN"):
        env = _bundle(env, Path(env["ZSW_NCS_TOOLCHAIN"]))
    elif NRF_ENV_FILE.exists():
        env = _nrf_env_file(env)
    else:
        raise SystemExit(f"No {NRF_ENV_FILE}. Generate it with the VS Code task "
                         "'Get nRF Connect environment setup', or set ZSW_NCS_TOOLCHAIN "
                         "to a toolchain bundle.")
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
        for key in ("PATH", "LD_LIBRARY_PATH", "PYTHONHOME", "PYTHONPATH", "CMAKE_PREFIX_PATH",
                    "GIT_EXEC_PATH", "GIT_TEMPLATE_DIR", "GNUARMEMB_TOOLCHAIN_PATH",
                    "ZEPHYR_TOOLCHAIN_VARIANT", "ZEPHYR_SDK_INSTALL_DIR", "ZEPHYR_BASE"):
            if key in env:
                print(f"export {key}={shlex.quote(env[key])}")
        print("unset VIRTUAL_ENV")
        return
    if not args.command:
        parser.error("Give a command to run, or --print")
    command = args.command
    if command[0] == "west":
        # The toolchain's west script needs the toolchain's interpreter.
        command = ["python3", "-m", "west", *command[1:]]
    os.execvpe(command[0], command, env)


if __name__ == "__main__":
    sys.exit(main())
