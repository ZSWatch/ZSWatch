# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

"""
Coredump analysis module — decodes a ZSWatch coredump and returns a backtrace.

Flow:
  1. Client POSTs coredump.txt content + elf_hash (or fw_commit_sha as fallback).
     For dev builds, the client can set use_latest_elf=true to decode with the
     most recently uploaded ELF regardless of hash/commit matching.
  2. Server looks up ELF by content hash, or resolves commit_sha → hash via mapping.
  3. Runs coredump_serial_log_parser.py to convert hex → binary
  4. Starts coredump_gdbserver.py as a subprocess (GDB RSP server)
  5. Connects arm-zephyr-eabi-gdb, runs "bt" + "info registers"
  6. Returns { backtrace, registers, file, line, timestamp }
"""

import asyncio
import gzip
import hashlib
import json
import logging
import os
import signal
import subprocess
import tempfile
from pathlib import Path
from typing import Optional

import hmac

import httpx
from fastapi import APIRouter, File, Form, HTTPException, Request, UploadFile
from pydantic import BaseModel

logger = logging.getLogger(__name__)

router = APIRouter()

# Paths
COREDUMP_TOOLS_DIR = Path(__file__).parent.parent / "coredump_tools"
ELF_CACHE_DIR = Path(os.environ.get("ELF_CACHE_DIR", "/tmp/zswatch_elfs"))
GDB_BINARY = os.environ.get("GDB_BINARY", "gdb-multiarch")

# GitHub release ELF asset naming (fallback for tagged releases)
GITHUB_REPO = os.environ.get("GITHUB_REPO", "ZSWatch/ZSWatch")
ELF_ASSET_TEMPLATE = os.environ.get(
    "ELF_ASSET_TEMPLATE",
    "{board}_nrf5340_cpuapp_zephyr.elf.gz"
)

ANALYZE_TIMEOUT = int(os.environ.get("COREDUMP_ANALYZE_TIMEOUT", "30"))
GDBSERVER_PORT = int(os.environ.get("COREDUMP_GDBSERVER_PORT", "1234"))

# Max uploaded ELF size: 50 MB (gzipped)
MAX_ELF_UPLOAD_SIZE = int(os.environ.get("MAX_ELF_UPLOAD_SIZE", str(50 * 1024 * 1024)))

# ELF cache eviction: max total size (default 500 MB), TTL for user uploads (30 days)
MAX_ELF_CACHE_SIZE = int(os.environ.get("MAX_ELF_CACHE_SIZE", str(500 * 1024 * 1024)))
ELF_CACHE_TTL_DAYS = int(os.environ.get("ELF_CACHE_TTL_DAYS", "30"))

# API key for ELF uploads (required — set via COREDUMP_API_KEY env var)
COREDUMP_API_KEY = os.environ.get("COREDUMP_API_KEY", "")


# ──────────────────────────────────────────────────────────────────────────────
# Request / response models
# ──────────────────────────────────────────────────────────────────────────────

class CoredumpAnalyzeRequest(BaseModel):
    coredump_txt: str           # Raw contents of /user/coredump.txt from the watch
    fw_commit_sha: str = ""     # e.g. "abc1234def56" (used to resolve elf_hash if not provided)
    elf_hash: Optional[str] = None  # SHA256 of the ELF (12 chars); primary lookup key
    use_latest_elf: bool = False  # Dev mode: ignore hash/commit, use the most recently uploaded ELF
    fw_version: str = ""        # e.g. "0.8.0" (fallback for GitHub release lookup)
    board: Optional[str] = "watchdk"
    build_type: Optional[str] = "debug"
    crash_file: Optional[str] = None
    crash_line: Optional[int] = None
    crash_time: Optional[str] = None


class CoredumpAnalyzeResponse(BaseModel):
    success: bool
    fw_version: str = ""
    fw_commit_sha: str = ""
    crash_file: Optional[str] = None
    crash_line: Optional[int] = None
    crash_time: Optional[str] = None
    backtrace: Optional[str] = None
    registers: Optional[str] = None
    raw_output: str = ""
    error: Optional[str] = None
    elf_available: bool = False
    elf_hash: Optional[str] = None  # The hash of the ELF used for analysis


class ElfUploadResponse(BaseModel):
    cached: bool
    elf_hash: str   # Server-computed SHA256 (12 chars) of the ELF content
    commit_sha: str = ""


# ──────────────────────────────────────────────────────────────────────────────
# ELF management — content-hash primary key, commit_sha → hash mapping
# ──────────────────────────────────────────────────────────────────────────────

def _compute_elf_hash(elf_data: bytes) -> str:
    """Compute a truncated SHA256 hash of raw ELF bytes (12 hex chars)."""
    return hashlib.sha256(elf_data).hexdigest()[:12]


def _elf_path_by_hash(elf_hash: str) -> Path:
    """Return the cache path for an ELF keyed by its content hash."""
    ELF_CACHE_DIR.mkdir(parents=True, exist_ok=True)
    return ELF_CACHE_DIR / f"{elf_hash}.elf"


def _mapping_file() -> Path:
    """JSON file mapping commit_sha → elf_hash."""
    ELF_CACHE_DIR.mkdir(parents=True, exist_ok=True)
    return ELF_CACHE_DIR / "commit_map.json"


def _load_commit_map() -> dict[str, str]:
    """Load the commit_sha → elf_hash mapping."""
    path = _mapping_file()
    if path.exists():
        try:
            data = json.loads(path.read_text())
            # Migrate old dict-value format to simple str values
            migrated = {}
            for k, v in data.items():
                if isinstance(v, dict):
                    migrated[k] = v["elf_hash"]
                else:
                    migrated[k] = v
            return migrated
        except Exception:
            return {}
    return {}


def _save_commit_map(mapping: dict[str, str]):
    """Persist the commit_sha → elf_hash mapping."""
    _mapping_file().write_text(json.dumps(mapping, indent=2))


def _register_elf(elf_data: bytes, commit_sha: str = "") -> str:
    """
    Cache an ELF by its content hash and update the commit_sha mapping.
    Returns the elf_hash.
    """
    elf_hash = _compute_elf_hash(elf_data)
    cache_path = _elf_path_by_hash(elf_hash)
    cache_path.write_bytes(elf_data)
    logger.info("ELF cached: %s (%d bytes, hash=%s)", cache_path.name, len(elf_data), elf_hash)

    if commit_sha:
        mapping = _load_commit_map()
        mapping[commit_sha] = elf_hash
        _save_commit_map(mapping)
        logger.info("Mapped commit %s → elf_hash %s", commit_sha, elf_hash)

    return elf_hash


def _resolve_elf_hash(elf_hash: Optional[str], commit_sha: str) -> Optional[str]:
    """Resolve to an elf_hash: use provided hash, or look up commit_sha mapping."""
    if elf_hash:
        return elf_hash
    if commit_sha:
        mapping = _load_commit_map()
        resolved = mapping.get(commit_sha)
        if resolved:
            logger.info("Resolved commit %s → elf_hash %s", commit_sha, resolved)
            return resolved
    return None


def _find_latest_elf() -> Optional[Path]:
    """Find the most recently modified ELF in the cache (for dev mode)."""
    if not ELF_CACHE_DIR.exists():
        return None
    elf_files = list(ELF_CACHE_DIR.glob("*.elf"))
    if not elf_files:
        return None
    latest = max(elf_files, key=lambda p: p.stat().st_mtime)
    logger.info("Using latest ELF (dev mode): %s", latest.name)
    return latest


def _find_elf(elf_hash: Optional[str], commit_sha: str, fw_version: str, board: str) -> Optional[Path]:
    """
    Find a cached ELF. Priority:
    1. elf_hash (direct or resolved from commit_sha mapping)
    2. Legacy version-keyed path (for GitHub release fallback)
    """
    resolved_hash = _resolve_elf_hash(elf_hash, commit_sha)
    if resolved_hash:
        path = _elf_path_by_hash(resolved_hash)
        if path.exists():
            logger.info("ELF found by hash: %s", resolved_hash)
            return path

    # Legacy fallback: version-keyed path
    if fw_version:
        safe_board = board.replace("/", "_").replace("@", "_")
        ver_path = ELF_CACHE_DIR / f"{safe_board}_nrf5340_cpuapp_{fw_version}_zephyr.elf"
        if ver_path.exists():
            logger.info("ELF cache hit (version): %s", ver_path)
            return ver_path

    return None


async def _fetch_elf(elf_hash: Optional[str], commit_sha: str, fw_version: str, board: str) -> Optional[Path]:
    """
    Look up ELF in cache, then try GitHub release by version.
    Returns the local .elf path, or None if not available.
    """
    cached = _find_elf(elf_hash, commit_sha, fw_version, board)
    if cached:
        return cached

    # Fallback: try GitHub release by version tag
    if not fw_version:
        return None

    asset_name = ELF_ASSET_TEMPLATE.format(board=board)
    tag = f"v{fw_version}"
    url = f"https://github.com/{GITHUB_REPO}/releases/download/{tag}/{asset_name}"
    logger.info("Downloading ELF from %s", url)

    try:
        async with httpx.AsyncClient(follow_redirects=True, timeout=60) as client:
            resp = await client.get(url)
            if resp.status_code == 404:
                logger.warning("ELF not found in GitHub releases for %s / %s", fw_version, board)
                return None
            resp.raise_for_status()
            gz_data = resp.content

        elf_data = gzip.decompress(gz_data)
        elf_hash_computed = _register_elf(elf_data, commit_sha)
        logger.info("ELF from GitHub release cached with hash %s", elf_hash_computed)
        return _elf_path_by_hash(elf_hash_computed)

    except Exception as exc:
        logger.error("Failed to fetch ELF: %s", exc)
        return None


def _evict_elf_cache():
    """
    Enforce ELF cache size and TTL limits.
    - Remove .elf files older than ELF_CACHE_TTL_DAYS.
    - If total size still exceeds MAX_ELF_CACHE_SIZE, remove oldest files (LRU).
    """
    if not ELF_CACHE_DIR.exists():
        return

    import time
    now = time.time()
    ttl_seconds = ELF_CACHE_TTL_DAYS * 86400

    elf_files = sorted(ELF_CACHE_DIR.glob("*.elf"), key=lambda p: p.stat().st_mtime)
    if not elf_files:
        return

    evicted_hashes = []

    # Phase 1: TTL eviction
    for f in list(elf_files):
        age = now - f.stat().st_mtime
        if age > ttl_seconds:
            logger.info("ELF cache TTL eviction: %s (age: %d days)", f.name, age // 86400)
            evicted_hashes.append(f.stem)  # filename without .elf
            f.unlink(missing_ok=True)
            elf_files.remove(f)

    # Phase 2: Size eviction (LRU — oldest first)
    total_size = sum(f.stat().st_size for f in elf_files)
    while total_size > MAX_ELF_CACHE_SIZE and elf_files:
        victim = elf_files.pop(0)
        victim_size = victim.stat().st_size
        logger.info("ELF cache size eviction: %s (%d bytes)", victim.name, victim_size)
        evicted_hashes.append(victim.stem)
        victim.unlink(missing_ok=True)
        total_size -= victim_size

    # Clean up commit_map entries pointing to evicted hashes
    if evicted_hashes:
        evicted_set = set(evicted_hashes)
        mapping = _load_commit_map()
        mapping = {k: v for k, v in mapping.items() if v not in evicted_set}
        _save_commit_map(mapping)

    logger.info("ELF cache: %d files, %.1f MB", len(elf_files), total_size / (1024 * 1024))


# ──────────────────────────────────────────────────────────────────────────────
# Analysis
# ──────────────────────────────────────────────────────────────────────────────

def _strip_binary_header(coredump_txt: str) -> str:
    """
    Strip the binary zsw_coredump_sumary_t header that precedes #CD:BEGIN#.
    The watch firmware prepends a binary struct before the text coredump data.
    """
    marker = "#CD:BEGIN#"
    idx = coredump_txt.find(marker)
    if idx > 0:
        logger.info("Stripping %d byte binary header before #CD:BEGIN#", idx)
        return coredump_txt[idx:]
    return coredump_txt


def _convert_coredump_to_bin(coredump_txt: str, bin_output: Path) -> bool:
    """Run coredump_serial_log_parser.py to convert hex text → binary."""
    coredump_txt = _strip_binary_header(coredump_txt)
    parser = COREDUMP_TOOLS_DIR / "coredump_serial_log_parser.py"
    with tempfile.NamedTemporaryFile(mode="w", suffix=".txt", delete=False) as f:
        f.write(coredump_txt)
        txt_path = f.name

    try:
        result = subprocess.run(
            ["python3", str(parser), txt_path, str(bin_output)],
            capture_output=True, text=True, timeout=15
        )
        logger.info("Parser stdout: %s", result.stdout.strip())
        if result.stderr:
            logger.info("Parser stderr: %s", result.stderr.strip())
        if result.returncode != 0:
            logger.error("coredump_serial_log_parser failed: %s", result.stderr)
            return False
        return bin_output.exists()
    finally:
        os.unlink(txt_path)


def _run_gdb_analysis(elf_path: Path, bin_path: Path, timeout: int) -> dict:
    """
    Start coredump_gdbserver.py, connect GDB, get backtrace + registers.
    Returns { backtrace, registers, raw_output }.
    """
    gdbserver_script = COREDUMP_TOOLS_DIR / "coredump_gdbserver.py"
    port = GDBSERVER_PORT

    env = dict(os.environ)
    env["PYTHONPATH"] = str(COREDUMP_TOOLS_DIR)

    # Start GDB RSP server subprocess
    server_proc = subprocess.Popen(
        ["python3", str(gdbserver_script), str(elf_path), str(bin_path), "--port", str(port)],
        stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
        env=env
    )

    try:
        # Give the server a moment to start
        import time
        time.sleep(1.5)

        gdb_result = subprocess.run(
            [GDB_BINARY, "-q", str(elf_path), "--batch", "-ex",
             "set confirm off",
             "-ex", "set target-charset ASCII",
             "-ex", f"target remote localhost:{port}",
             "-ex", "bt",
             "-ex", "info registers",
             "-ex", "quit"],
            capture_output=True, text=True, timeout=timeout
        )

        raw = gdb_result.stdout + gdb_result.stderr
        logger.info("GDB raw output:\n%s", raw[:2000])
        bt, regs = _parse_gdb_output(raw)
        logger.info("Parsed backtrace: %s", bt[:500] if bt else "(none)")
        return {"backtrace": bt, "registers": regs, "raw_output": raw}
    finally:
        try:
            server_proc.send_signal(signal.SIGTERM)
            server_proc.wait(timeout=3)
        except Exception:
            server_proc.kill()


def _parse_gdb_output(gdb_output: str) -> tuple[Optional[str], Optional[str]]:
    """
    Split the GDB stdout into a backtrace section and registers section.
    Returns (backtrace_str, registers_str).
    """
    lines = gdb_output.splitlines()
    bt_lines = []
    reg_lines = []
    in_bt = False
    in_regs = False

    for line in lines:
        stripped = line.strip()
        # Backtrace starts with "#0 "
        if stripped.startswith("#0 "):
            in_bt = True
        if in_bt:
            # Backtrace ends when we hit a blank line or register dump starts
            if stripped.startswith("r0 ") or stripped.startswith("pc "):
                in_bt = False
                in_regs = True
            else:
                bt_lines.append(line)
        elif in_regs:
            reg_lines.append(line)

    backtrace = "\n".join(bt_lines).strip() or None
    registers = "\n".join(reg_lines).strip() or None
    return backtrace, registers


# ──────────────────────────────────────────────────────────────────────────────
# Endpoints
# ──────────────────────────────────────────────────────────────────────────────

@router.post("/coredump/analyze", response_model=CoredumpAnalyzeResponse)
async def analyze_coredump(req: CoredumpAnalyzeRequest):
    """
    Analyze a ZSWatch coredump.

    Accepts the raw coredump.txt content from the watch and returns a
    decoded backtrace + register dump.

    ELF lookup priority:
    1. elf_hash (direct content-hash lookup)
    2. commit_sha → elf_hash mapping (set by upload endpoints)
    3. GitHub release by fw_version (legacy fallback)
    """
    logger.info(
        "Coredump analysis requested: elf_hash=%s sha=%s fw=%s board=%s use_latest=%s",
        req.elf_hash, req.fw_commit_sha, req.fw_version, req.board, req.use_latest_elf,
    )

    base = CoredumpAnalyzeResponse(
        success=False,
        fw_version=req.fw_version,
        fw_commit_sha=req.fw_commit_sha,
        crash_file=req.crash_file,
        crash_line=req.crash_line,
        crash_time=req.crash_time,
    )

    # Fetch ELF — dev mode uses latest uploaded, normal mode does hash/commit lookup
    if req.use_latest_elf:
        elf_path = _find_latest_elf()
    else:
        elf_path = await _fetch_elf(req.elf_hash, req.fw_commit_sha, req.fw_version, req.board or "watchdk")

    if elf_path is None:
        base.success = True
        base.elf_available = False
        base.error = (
            f"ELF not found (hash={req.elf_hash}, commit={req.fw_commit_sha}). "
            "Upload via /api/coredump/upload-elf."
        )
        return base

    base.elf_available = True
    # Return the hash of the ELF being used
    base.elf_hash = _compute_elf_hash(elf_path.read_bytes())

    with tempfile.TemporaryDirectory() as tmpdir:
        bin_path = Path(tmpdir) / "coredump.bin"

        ok = _convert_coredump_to_bin(req.coredump_txt, bin_path)
        if not ok:
            raise HTTPException(
                status_code=422,
                detail="Failed to parse coredump.txt. The file may be incomplete or corrupted."
            )

        try:
            result = await asyncio.get_event_loop().run_in_executor(
                None, lambda: _run_gdb_analysis(elf_path, bin_path, ANALYZE_TIMEOUT)
            )
        except subprocess.TimeoutExpired:
            raise HTTPException(status_code=504, detail="GDB analysis timed out.")
        except Exception as exc:
            logger.exception("GDB analysis failed")
            raise HTTPException(status_code=500, detail=f"GDB analysis failed: {exc}")

    base.success = True
    base.backtrace = result.get("backtrace")
    base.registers = result.get("registers")
    base.raw_output = result.get("raw_output", "")
    return base


def _verify_api_key(request: Request):
    """Verify the Bearer token matches the configured COREDUMP_API_KEY."""
    if not COREDUMP_API_KEY:
        raise HTTPException(
            status_code=503,
            detail="ELF uploads are disabled — COREDUMP_API_KEY is not configured on the server.",
        )
    auth_header = request.headers.get("Authorization", "")
    if not auth_header.startswith("Bearer "):
        raise HTTPException(status_code=401, detail="Missing Authorization: Bearer <token> header.")
    token = auth_header[len("Bearer "):]
    if not hmac.compare_digest(token, COREDUMP_API_KEY):
        raise HTTPException(status_code=403, detail="Invalid API key.")


@router.post("/coredump/upload-elf", response_model=ElfUploadResponse)
async def upload_elf(
    request: Request,
    commit_sha: str = Form(""),
    board: str = Form("watchdk"),
    build_type: str = Form("debug"),
    elf_gz: UploadFile = File(...),
):
    """
    Upload a gzipped (or raw) ELF file.
    Server computes the content hash and caches the ELF by that hash.
    If commit_sha is provided, also updates the commit → hash mapping.

    Requires Authorization: Bearer <COREDUMP_API_KEY> header.
    """
    _verify_api_key(request)
    logger.info("ELF upload: commit_sha=%s board=%s build_type=%s", commit_sha, board, build_type)

    # Read and validate size
    data = await elf_gz.read()
    if len(data) > MAX_ELF_UPLOAD_SIZE:
        raise HTTPException(
            status_code=413,
            detail=f"ELF file too large ({len(data)} bytes). Max: {MAX_ELF_UPLOAD_SIZE}.",
        )

    # Decompress gzip → raw ELF
    try:
        elf_data = gzip.decompress(data)
    except Exception:
        # Maybe it was uploaded uncompressed — use as-is
        elf_data = data

    elf_hash = _register_elf(elf_data, commit_sha)
    _evict_elf_cache()

    return ElfUploadResponse(cached=True, elf_hash=elf_hash, commit_sha=commit_sha)
