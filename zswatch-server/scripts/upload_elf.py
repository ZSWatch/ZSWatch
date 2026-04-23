#!/usr/bin/env python3
"""
Upload a local zephyr.elf to the coredump analysis server.

Usage:
    # Auto-detect commit SHA from git (token from COREDUMP_API_KEY env var):
    python upload_elf.py build/app/zephyr/zephyr.elf

    # Explicit commit SHA and token:
    python upload_elf.py build/app/zephyr/zephyr.elf --commit abc1234 --token mytoken

    # Custom server URL:
    python upload_elf.py build/app/zephyr/zephyr.elf --server http://myserver:8000

The server computes a content hash (SHA256, 12 chars) of the ELF and caches it.
If a commit SHA is provided, the server maps commit → hash so that the companion
app can resolve the ELF automatically when analyzing a crash.

Authentication: requires a valid API token, passed via --token or COREDUMP_API_KEY env var.
"""

import argparse
import os
import subprocess
import sys
from pathlib import Path

import requests

DEFAULT_SERVER = "https://zswatch-production.up.railway.app"


def _load_dotenv():
    """Load .env file from the repo root (walk up from this script)."""
    d = Path(__file__).resolve().parent
    for _ in range(5):
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


def get_git_commit() -> str:
    """Get the current git commit SHA."""
    try:
        result = subprocess.run(
            ["git", "rev-parse", "HEAD"],
            capture_output=True, text=True, check=True
        )
        return result.stdout.strip()
    except Exception:
        return ""


def main():
    _load_dotenv()
    parser = argparse.ArgumentParser(description="Upload zephyr.elf to coredump server")
    parser.add_argument("elf_path", type=Path, help="Path to zephyr.elf")
    parser.add_argument("--commit", default="", help="Git commit SHA (auto-detected if not set)")
    parser.add_argument("--server", default=DEFAULT_SERVER, help=f"Server URL (default: {DEFAULT_SERVER})")
    parser.add_argument("--token", default="", help="API token (default: COREDUMP_API_KEY env var)")
    args = parser.parse_args()

    if not args.elf_path.exists():
        print(f"Error: {args.elf_path} not found")
        sys.exit(1)

    token = args.token or os.environ.get("COREDUMP_API_KEY", "")
    if not token:
        print("Error: No API token provided. Use --token or set COREDUMP_API_KEY env var.")
        sys.exit(1)

    commit_sha = args.commit or get_git_commit()

    url = f"{args.server}/api/coredump/upload-elf"
    print(f"Uploading {args.elf_path} ({args.elf_path.stat().st_size / 1024 / 1024:.1f} MB)")
    print(f"  Server:  {url}")
    print(f"  Commit:  {commit_sha or '(none)'}")

    with open(args.elf_path, "rb") as f:
        resp = requests.post(
            url,
            headers={"Authorization": f"Bearer {token}"},
            data={"commit_sha": commit_sha},
            files={"elf_gz": (args.elf_path.name, f)},
            timeout=120,
        )

    if resp.status_code == 200:
        data = resp.json()
        print(f"  ELF hash: {data['elf_hash']}")
        print("Upload successful.")
    else:
        print(f"Upload failed ({resp.status_code}): {resp.text}")
        sys.exit(1)


if __name__ == "__main__":
    main()
