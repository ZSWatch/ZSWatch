# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

"""
West command to upload a zephyr.elf to the ZSWatch coredump analysis server.

Usage:
    west upload_elf                        # interactive: pick from detected builds
    west upload_elf --build_dir app/build  # explicit build directory
    west upload_elf --commit abc1234       # override git commit SHA
    west upload_elf --server http://...    # override server URL
"""

import gzip
import os
import subprocess
import sys
from pathlib import Path

from west.commands import WestCommand
from west import log

DEFAULT_SERVER = "https://zswatch-production.up.railway.app"
APP_DIR = Path(__file__).parent.parent  # app/


def _load_dotenv():
    """Load .env from the repo root (walk up from this script)."""
    d = Path(__file__).resolve().parent
    for _ in range(6):
        env_file = d / ".env"
        if env_file.is_file():
            for line in env_file.read_text().splitlines():
                line = line.strip()
                if not line or line.startswith("#"):
                    continue
                key, _, value = line.partition("=")
                if key and value and key not in os.environ:
                    os.environ[key] = value
            return
        d = d.parent


def _get_git_commit() -> str:
    try:
        result = subprocess.run(
            ["git", "rev-parse", "HEAD"],
            capture_output=True, text=True, check=True
        )
        return result.stdout.strip()
    except Exception:
        return ""


def _find_build_dirs() -> list[tuple[str, Path]]:
    """
    Find all build directories under app/ that contain a valid zephyr.elf.
    Returns a list of (label, elf_path) tuples sorted by modification time (newest first).
    """
    results = []
    for d in sorted(APP_DIR.iterdir()):
        if not d.is_dir() or not d.name.startswith("build"):
            continue
        elf = d / "app" / "zephyr" / "zephyr.elf"
        if elf.exists():
            results.append((d.name, elf))

    # Sort newest first by ELF mtime
    results.sort(key=lambda x: x[1].stat().st_mtime, reverse=True)
    return results


def _prompt_build(builds: list[tuple[str, Path]]) -> Path:
    """Interactively prompt the user to pick a build."""
    print("\nAvailable builds (newest first):")
    for i, (label, elf) in enumerate(builds):
        size_mb = elf.stat().st_size / 1024 / 1024
        from datetime import datetime
        mtime = datetime.fromtimestamp(elf.stat().st_mtime).strftime("%Y-%m-%d %H:%M")
        print(f"  {i + 1}: {label:<30}  {size_mb:.1f} MB  ({mtime})")

    while True:
        raw = input(f"\nSelect build [1-{len(builds)}]: ").strip()
        try:
            idx = int(raw) - 1
            if 0 <= idx < len(builds):
                return builds[idx][1]
        except ValueError:
            pass
        print("Invalid selection, try again.")


def _upload_elf(elf_path: Path, commit_sha: str, server: str, token: str) -> bool:
    try:
        import requests
    except ImportError:
        log.err("requests library not found. Install it: pip install requests")
        return False

    url = f"{server}/api/coredump/upload-elf"
    print(f"Uploading {elf_path.name} ({elf_path.stat().st_size / 1024 / 1024:.1f} MB)")
    print(f"  Server:  {url}")
    print(f"  Commit:  {commit_sha or '(none)'}")

    gz_data = gzip.compress(elf_path.read_bytes())

    resp = requests.post(
        url,
        headers={"Authorization": f"Bearer {token}"},
        data={"commit_sha": commit_sha},
        files={"elf_gz": (elf_path.name + ".gz", gz_data, "application/gzip")},
        timeout=120,
    )

    if resp.status_code == 200:
        data = resp.json()
        print(f"  ELF hash: {data['elf_hash']}")
        print("Upload successful.")
        return True
    else:
        log.err(f"Upload failed ({resp.status_code}): {resp.text}")
        return False


class UploadElfWestCommand(WestCommand):
    def __init__(self):
        super().__init__(
            "upload_elf",
            "Upload zephyr.elf to the coredump analysis server",
            "Upload a firmware ELF to the ZSWatch coredump server for crash analysis.",
        )

    def do_add_parser(self, parser_adder):
        parser = parser_adder.add_parser(
            self.name, help=self.help, description=self.description
        )

        parser.add_argument(
            "--build_dir",
            type=str,
            default="",
            help="Build directory containing app/zephyr/zephyr.elf. "
                 "If omitted, available builds are listed for interactive selection.",
        )

        parser.add_argument(
            "--commit",
            type=str,
            default="",
            help="Git commit SHA to associate with the ELF (auto-detected from HEAD if omitted).",
        )

        parser.add_argument(
            "--server",
            type=str,
            default="",
            help=f"Server base URL (default: {DEFAULT_SERVER}, or COREDUMP_SERVER_URL env var).",
        )

        parser.add_argument(
            "--token",
            type=str,
            default="",
            help="API token (default: COREDUMP_API_KEY env var).",
        )

        return parser

    def do_run(self, args, unknown_args):
        _load_dotenv()

        token = args.token or os.environ.get("COREDUMP_API_KEY", "")
        if not token:
            log.err("No API token provided. Use --token or set COREDUMP_API_KEY env var.")
            sys.exit(1)

        server = args.server or os.environ.get("COREDUMP_SERVER_URL", DEFAULT_SERVER)
        commit_sha = args.commit or _get_git_commit()

        if args.build_dir:
            elf_path = Path(args.build_dir) / "app" / "zephyr" / "zephyr.elf"
            if not elf_path.exists():
                log.err(f"ELF not found: {elf_path}")
                sys.exit(1)
        else:
            builds = _find_build_dirs()
            if not builds:
                log.err(f"No build directories with zephyr.elf found under {APP_DIR}")
                sys.exit(1)

            if len(builds) == 1:
                elf_path = builds[0][1]
                print(f"Using only available build: {builds[0][0]}")
            else:
                elf_path = _prompt_build(builds)

        if not _upload_elf(elf_path, commit_sha, server, token):
            sys.exit(1)
