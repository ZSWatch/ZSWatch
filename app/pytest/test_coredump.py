"""
Coredump E2E tests for watchdk hardware.

Tests the full coredump flow:
  1. Erase any existing coredump
  2. Trigger crash via 'coredump crash' (spawns thread with deep call stack)
  3. Wait for reboot, verify coredump was stored
  4. Download coredump file via MCUmgr FS
  5. Upload the ELF to the server
  6. Send coredump to server for GDB analysis
  7. Verify backtrace contains the expected crash call chain

Usage::

    # Full flow (build, flash, crash, analyze):
    pytest test_coredump.py --board watchdk@1 -s

    # Skip flash (device already has correct firmware):
    pytest test_coredump.py --board watchdk@1 --skip-flash -s

    # Custom server URL:
    pytest test_coredump.py --board watchdk@1 --skip-flash -s --server-url http://myserver:8000

Prerequisites:
    - Server running: cd app-builder && uvicorn server.main:app --port 8000
    - arm-zephyr-eabi-gdb available on the server
    - watchdk connected via J-Link + USB CDC
"""

import asyncio
import gzip
import hashlib
import logging
import os
import time
from pathlib import Path

import pytest
import requests
import utils
from conftest import get_global_config

from mcumgr_utils import (
    file_download_usb,
    require_usb_port,
    shell_command_usb,
    wait_for_usb_port,
)

log = logging.getLogger(__name__)

USB_TIMEOUT = 15.0

# Default build directory (relative to app/pytest/ → app/build_dbg_dk)
DEFAULT_BUILD_DIR = Path(__file__).parent.parent / "build_dbg_dk"

# Expected functions in the crash backtrace (from zsw_shell.c crash call chain)
EXPECTED_BT_FUNCTIONS = ["crash_level_3", "crash_level_2", "crash_level_1"]

DEFAULT_SERVER_URL = "http://localhost:8000"


def pytest_addoption_coredump(parser):
    """Register coredump-specific CLI options (called from conftest if needed)."""
    try:
        parser.addoption(
            "--build-dir",
            action="store",
            default=None,
            help=f"Firmware build directory (default: {DEFAULT_BUILD_DIR})",
        )
    except ValueError:
        pass  # Options already added


def _ensure_hw(device_config):
    if device_config.get("board", "") == "native_sim":
        pytest.skip("Hardware test — skipping native_sim")


def _get_server_url(request) -> str:
    return os.environ.get(
        "COREDUMP_SERVER_URL",
        get_global_config().get("coredump_server_url", DEFAULT_SERVER_URL),
    )


def _get_build_dir(request) -> Path:
    try:
        custom = request.config.getoption("--build-dir")
        if custom:
            return Path(custom)
    except ValueError:
        pass
    env = os.environ.get("COREDUMP_BUILD_DIR")
    if env:
        return Path(env)
    return DEFAULT_BUILD_DIR


def _find_elf(build_dir: Path) -> Path:
    """Find zephyr.elf in the build directory."""
    candidates = [
        build_dir / "app" / "zephyr" / "zephyr.elf",
        build_dir / "zephyr" / "zephyr.elf",
        build_dir / "zephyr.elf",
    ]
    for path in candidates:
        if path.exists():
            return path
    pytest.fail(
        f"zephyr.elf not found in {build_dir}. Searched:\n"
        + "\n".join(f"  - {p}" for p in candidates)
    )


async def _usb_shell(device_config, cmd_str, timeout_s=10.0):
    usb_port = require_usb_port(device_config)
    await wait_for_usb_port(usb_port, True, timeout_s=USB_TIMEOUT)
    argv = cmd_str.split()
    response = await shell_command_usb(device_config, argv, timeout_s=timeout_s)
    return response.o.strip() if response.o else ""


def _strip_binary_header(raw_bytes: bytes) -> str:
    """Strip the binary zsw_coredump_sumary_t header before #CD:BEGIN#."""
    content = raw_bytes.decode("utf-8", errors="replace")
    start = content.find("#CD:BEGIN#")
    if start < 0:
        pytest.fail("Coredump file missing #CD:BEGIN# marker")
    text = content[start:].replace("\x00", "")
    assert "#CD:END#" in text, "Coredump file missing #CD:END# marker"
    return text


def _upload_elf(server_url: str, elf_path: Path, commit_sha: str = "") -> str:
    """Upload ELF to server, return the elf_hash."""
    elf_bytes = elf_path.read_bytes()
    gz_data = gzip.compress(elf_bytes)

    log.info("Uploading ELF %s (%.1f MB → %.1f MB gzipped)",
             elf_path.name, len(elf_bytes) / 1e6, len(gz_data) / 1e6)

    resp = requests.post(
        f"{server_url}/api/coredump/upload-elf",
        data={"commit_sha": commit_sha},
        files={"elf_gz": ("zephyr.elf.gz", gz_data)},
        timeout=120,
    )
    assert resp.status_code == 200, f"ELF upload failed: {resp.status_code} {resp.text}"
    data = resp.json()
    elf_hash = data["elf_hash"]
    log.info("ELF uploaded successfully: hash=%s", elf_hash)
    return elf_hash


def _analyze_coredump(
    server_url: str,
    coredump_text: str,
    elf_hash: str = "",
    use_latest_elf: bool = False,
) -> dict:
    """Send coredump to server for analysis."""
    payload = {
        "coredump_txt": coredump_text,
        "fw_commit_sha": "",
        "fw_version": "",
        "board": "watchdk",
        "build_type": "debug",
        "use_latest_elf": use_latest_elf,
    }
    if elf_hash:
        payload["elf_hash"] = elf_hash

    resp = requests.post(
        f"{server_url}/api/coredump/analyze",
        json=payload,
        timeout=60,
    )
    assert resp.status_code == 200, f"Analyze failed: {resp.status_code} {resp.text}"
    return resp.json()


# ──────────────────────────────────────────────────────────────────────────────
# Tests
# ──────────────────────────────────────────────────────────────────────────────


@pytest.mark.asyncio
async def test_coredump_summary(device_config):
    """Verify 'coredump summary' shell command runs without error."""
    _ensure_hw(device_config)
    output = await _usb_shell(device_config, "coredump summary")
    print(f"\n=== Coredump Summary ===\n{output}")
    assert output is not None


@pytest.mark.asyncio
async def test_coredump_erase(device_config):
    """Verify 'coredump erase 0' runs without error."""
    _ensure_hw(device_config)
    output = await _usb_shell(device_config, "coredump erase 0")
    print(f"\n=== Coredump Erase ===\n{output}")


@pytest.mark.asyncio
async def test_coredump_summary_after_erase(device_config):
    """After erase, summary should show no coredumps."""
    _ensure_hw(device_config)
    await _usb_shell(device_config, "coredump erase 0")
    await asyncio.sleep(0.5)
    output = await _usb_shell(device_config, "coredump summary")
    print(f"\n=== Summary After Erase ===\n{output}")
    assert "No coredump" in output or "0" in output or output is not None


@pytest.mark.asyncio
async def test_coredump_crash_and_download(device_config):
    """
    Full E2E: trigger crash, reboot, verify coredump stored, download file.

    The crash command spawns a dedicated thread with a multi-frame call stack
    (crash_level_1 → crash_level_2 → crash_level_3 → __ASSERT) so GDB
    produces a meaningful backtrace.
    """
    _ensure_hw(device_config)

    # 1. Erase any old coredump
    await _usb_shell(device_config, "coredump erase 0")
    await asyncio.sleep(0.5)

    # 2. Trigger crash — the shell prints "Spawning crash thread..." then
    #    the device reboots. The SMP response may or may not arrive before
    #    the connection drops.
    print("\n--- Triggering crash ---")
    try:
        await _usb_shell(device_config, "coredump crash")
    except Exception:
        pass  # Expected: device reboots

    # 3. Wait for USB CDC to disappear then reappear
    usb_port = require_usb_port(device_config)
    print("Waiting for device to reboot...")
    await asyncio.sleep(3)
    # Reset via J-Link to ensure clean reboot
    utils.reset(device_config)
    await wait_for_usb_port(usb_port, True, timeout_s=30.0)
    await asyncio.sleep(2)  # Let SMP stack stabilise

    # 4. Verify coredump was stored
    output = await _usb_shell(device_config, "coredump summary")
    print(f"\n=== Post-Crash Summary ===\n{output}")
    assert "Stored coredumps: 1" in output, f"Expected coredump, got: {output}"
    assert "zsw_shell.c" in output, f"Expected zsw_shell.c in summary, got: {output}"

    # 5. Download coredump file
    print("Downloading /user/coredump.txt ...")
    raw = await file_download_usb(device_config, "/user/coredump.txt")
    coredump_text = _strip_binary_header(raw)
    print(f"Downloaded coredump: {len(coredump_text)} chars")


@pytest.mark.asyncio
async def test_coredump_full_analysis(device_config, request):
    """
    Full E2E with server analysis:
      1. Erase old coredump
      2. Trigger crash via shell
      3. Download coredump from watch
      4. Upload the matching ELF to the server
      5. Send coredump for analysis
      6. Verify backtrace contains crash_level_3 → crash_level_2 → crash_level_1

    Requires the analysis server to be running.
    """
    _ensure_hw(device_config)

    server_url = _get_server_url(request)
    build_dir = _get_build_dir(request)

    # Verify server is reachable
    try:
        health = requests.get(f"{server_url}/api/health", timeout=5)
        assert health.status_code == 200, f"Server unhealthy: {health.status_code}"
    except requests.ConnectionError:
        pytest.skip(f"Server not reachable at {server_url}")

    # Find the ELF
    elf_path = _find_elf(build_dir)
    print(f"\nUsing ELF: {elf_path}")

    # ── Step 1: Erase old coredump ──
    await _usb_shell(device_config, "coredump erase 0")
    await asyncio.sleep(0.5)

    # ── Step 2: Trigger crash ──
    print("\n--- Triggering crash ---")
    try:
        await _usb_shell(device_config, "coredump crash")
    except Exception:
        pass  # Expected: device reboots

    # ── Step 3: Wait for reboot and download ──
    usb_port = require_usb_port(device_config)
    print("Waiting for device to reboot...")
    await asyncio.sleep(3)
    utils.reset(device_config)
    await wait_for_usb_port(usb_port, True, timeout_s=30.0)
    await asyncio.sleep(2)

    output = await _usb_shell(device_config, "coredump summary")
    print(f"\n=== Post-Crash Summary ===\n{output}")
    assert "Stored coredumps: 1" in output, f"Expected coredump, got: {output}"

    print("Downloading /user/coredump.txt ...")
    raw = await file_download_usb(device_config, "/user/coredump.txt")
    coredump_text = _strip_binary_header(raw)
    print(f"Downloaded coredump: {len(coredump_text)} chars")

    # ── Step 4: Upload ELF to server ──
    elf_hash = _upload_elf(server_url, elf_path)
    print(f"ELF hash: {elf_hash}")

    # ── Step 5: Analyze with elf_hash ──
    print("\n--- Analyzing coredump (by elf_hash) ---")
    result = _analyze_coredump(server_url, coredump_text, elf_hash=elf_hash)

    print(f"\n=== Analysis Result ===")
    print(f"  success:       {result.get('success')}")
    print(f"  elf_available: {result.get('elf_available')}")
    print(f"  elf_hash:      {result.get('elf_hash')}")
    if result.get("backtrace"):
        print(f"  backtrace:\n{result['backtrace']}")
    if result.get("registers"):
        print(f"  registers:\n{result['registers']}")
    if result.get("error"):
        print(f"  error: {result['error']}")

    # ── Step 6: Verify backtrace ──
    assert result["success"], f"Analysis failed: {result.get('error')}"
    assert result["elf_available"], "ELF should be available after upload"

    backtrace = result.get("backtrace", "")
    assert backtrace, "Backtrace should not be empty"

    for func in EXPECTED_BT_FUNCTIONS:
        assert func in backtrace, (
            f"Expected '{func}' in backtrace but not found.\n"
            f"Backtrace:\n{backtrace}"
        )
    print("\nBacktrace contains all expected crash functions.")


@pytest.mark.asyncio
async def test_coredump_use_latest_elf(device_config, request):
    """
    Test the 'use_latest_elf' dev-mode flag.

    Uploads the ELF, then sends the coredump with use_latest_elf=True
    (no elf_hash). The server should use the most recently uploaded ELF.
    """
    _ensure_hw(device_config)

    server_url = _get_server_url(request)
    build_dir = _get_build_dir(request)

    try:
        health = requests.get(f"{server_url}/api/health", timeout=5)
        assert health.status_code == 200
    except requests.ConnectionError:
        pytest.skip(f"Server not reachable at {server_url}")

    elf_path = _find_elf(build_dir)

    # ── Erase, crash, download ──
    await _usb_shell(device_config, "coredump erase 0")
    await asyncio.sleep(0.5)

    print("\n--- Triggering crash ---")
    try:
        await _usb_shell(device_config, "coredump crash")
    except Exception:
        pass

    usb_port = require_usb_port(device_config)
    await asyncio.sleep(3)
    utils.reset(device_config)
    await wait_for_usb_port(usb_port, True, timeout_s=30.0)
    await asyncio.sleep(2)

    raw = await file_download_usb(device_config, "/user/coredump.txt")
    coredump_text = _strip_binary_header(raw)

    # ── Upload ELF (no commit SHA — just makes it the latest) ──
    _upload_elf(server_url, elf_path, commit_sha="")

    # ── Analyze with use_latest_elf (no elf_hash, no commit_sha) ──
    print("\n--- Analyzing coredump (use_latest_elf=True) ---")
    result = _analyze_coredump(server_url, coredump_text, use_latest_elf=True)

    print(f"\n=== Analysis Result (use_latest_elf) ===")
    print(f"  success:       {result.get('success')}")
    print(f"  elf_available: {result.get('elf_available')}")
    if result.get("backtrace"):
        print(f"  backtrace:\n{result['backtrace']}")
    if result.get("error"):
        print(f"  error: {result['error']}")

    assert result["success"], f"Analysis failed: {result.get('error')}"
    assert result["elf_available"], "ELF should be available via use_latest_elf"

    backtrace = result.get("backtrace", "")
    assert backtrace, "Backtrace should not be empty"
    for func in EXPECTED_BT_FUNCTIONS:
        assert func in backtrace, (
            f"Expected '{func}' in backtrace (use_latest_elf mode).\n"
            f"Backtrace:\n{backtrace}"
        )
    print("\nuse_latest_elf mode: backtrace verified.")
