# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

"""
Compiler module — compiles source files into a .llext using the ZSWatch EDK.
"""

import logging
import os
import re
import subprocess
import tempfile
from typing import Optional

from fastapi import APIRouter, HTTPException
from fastapi.responses import Response
from pydantic import BaseModel

logger = logging.getLogger(__name__)

router = APIRouter()

EDK_BASE_DIR = os.environ.get("EDK_BASE_DIR", "/opt/edks")
COMPILE_TIMEOUT = int(os.environ.get("COMPILE_TIMEOUT", "60"))

# GCC binary — resolved from EDK or Zephyr SDK
GCC_BINARY = os.environ.get(
    "GCC_BINARY",
    "arm-zephyr-eabi-gcc"
)


class SourceFile(BaseModel):
    name: str
    content: str


class CompileRequest(BaseModel):
    files: list[SourceFile]
    app_name: Optional[str] = "my_app"
    edk_version: Optional[str] = None  # None = use latest


class CompileError(BaseModel):
    file: Optional[str] = None
    line: Optional[int] = None
    column: Optional[int] = None
    severity: str  # "error" | "warning" | "note"
    message: str


class CompileResponse(BaseModel):
    success: bool
    errors: list[CompileError] = []
    raw_output: str = ""


def _find_edk(version: Optional[str] = None) -> str:
    """Find the EDK directory to use."""
    if not os.path.isdir(EDK_BASE_DIR):
        raise HTTPException(
            status_code=500,
            detail=f"EDK base directory not found: {EDK_BASE_DIR}"
        )

    entries = sorted(os.listdir(EDK_BASE_DIR))
    if not entries:
        raise HTTPException(status_code=500, detail="No EDK versions available")

    if version:
        for entry in entries:
            version_file = os.path.join(EDK_BASE_DIR, entry, "VERSION")
            if os.path.isfile(version_file):
                with open(version_file) as f:
                    if f.read().strip() == version:
                        return os.path.join(EDK_BASE_DIR, entry)
        raise HTTPException(status_code=404, detail=f"EDK version {version} not found")

    # Use latest (last in sorted order)
    return os.path.join(EDK_BASE_DIR, entries[-1])


def _parse_gcc_errors(stderr: str) -> list[CompileError]:
    """Parse GCC error/warning output into structured errors."""
    errors = []
    # Match: file.c:10:5: error: message
    # or:    file.c:10: error: message
    pattern = re.compile(
        r"^(?P<file>[^:\s]+):(?P<line>\d+):(?:(?P<col>\d+):)?\s*"
        r"(?P<severity>error|warning|note|fatal error):\s*(?P<message>.+)$",
        re.MULTILINE,
    )
    for m in pattern.finditer(stderr):
        errors.append(CompileError(
            file=os.path.basename(m.group("file")),
            line=int(m.group("line")),
            column=int(m.group("col")) if m.group("col") else None,
            severity="error" if "error" in m.group("severity") else m.group("severity"),
            message=m.group("message").strip(),
        ))
    return errors


def _read_cflags(edk_dir: str) -> list[str]:
    """Read compile flags from the EDK cmake.cflags file.

    The cmake.cflags file contains a CMake set() call with semicolon-separated flags:
        set(LLEXT_CFLAGS "-DKERNEL;-D__ZEPHYR__=1;-fPIC;-I${CMAKE_CURRENT_LIST_DIR}/include/...")
    """
    cmake_cflags = os.path.join(edk_dir, "cmake.cflags")
    if not os.path.isfile(cmake_cflags):
        raise HTTPException(status_code=500, detail="EDK cmake.cflags not found")

    with open(cmake_cflags) as f:
        content = f.read()

    # Extract the LLEXT_CFLAGS value from: set(LLEXT_CFLAGS "flag1;flag2;...")
    cflags_match = re.search(
        r'set\(LLEXT_CFLAGS\s+"([^"]+)"\s*\)',
        content,
    )
    if not cflags_match:
        raise HTTPException(status_code=500, detail="LLEXT_CFLAGS not found in cmake.cflags")

    flags_str = cflags_match.group(1)

    # CMake list uses semicolons as separators
    flags = [f for f in flags_str.split(";") if f]

    # Resolve ${CMAKE_CURRENT_LIST_DIR} to the actual EDK directory
    flags = [f.replace("${CMAKE_CURRENT_LIST_DIR}", edk_dir) for f in flags]

    return flags


def _read_zswatch_extra(edk_dir: str) -> tuple[list[str], list[str]]:
    """Read ZSWatch-specific extra flags."""
    extra_cflags = []
    extra_ldflags = []
    extra_cmake = os.path.join(edk_dir, "zswatch_extra.cmake")
    if os.path.isfile(extra_cmake):
        with open(extra_cmake) as f:
            content = f.read()
        # Extract list(APPEND LLEXT_CFLAGS ...) entries
        for m in re.finditer(r'list\(APPEND\s+LLEXT_CFLAGS\s+"([^"]+)"\)', content):
            flag = m.group(1)
            # Replace ${CMAKE_CURRENT_LIST_DIR} with the actual EDK dir
            flag = flag.replace("${CMAKE_CURRENT_LIST_DIR}", edk_dir)
            extra_cflags.append(flag)
        # Extract link flags
        for m in re.finditer(r'set\(LLEXT_EXTRA_LINK_FLAGS\s+"([^"]+)"\)', content):
            extra_ldflags.extend(m.group(1).split())
    return extra_cflags, extra_ldflags


@router.post("/compile")
async def compile_app(request: CompileRequest):
    """
    Compile source files into a .llext binary.

    Returns the .llext binary on success, or JSON errors on failure.
    """
    if not request.files:
        raise HTTPException(status_code=400, detail="No source files provided")

    # Validate file names
    for f in request.files:
        if ".." in f.name or "/" in f.name or "\\" in f.name:
            raise HTTPException(status_code=400, detail=f"Invalid filename: {f.name}")
        if not (f.name.endswith(".c") or f.name.endswith(".h")):
            raise HTTPException(
                status_code=400,
                detail=f"Only .c and .h files supported: {f.name}"
            )

    edk_dir = _find_edk(request.edk_version)
    logger.info(f"Using EDK: {edk_dir}")

    # Read compile flags
    cflags = _read_cflags(edk_dir)
    extra_cflags, extra_ldflags = _read_zswatch_extra(edk_dir)

    with tempfile.TemporaryDirectory(prefix="zsw_build_") as tmpdir:
        src_dir = os.path.join(tmpdir, "src")
        obj_dir = os.path.join(tmpdir, "obj")
        os.makedirs(src_dir)
        os.makedirs(obj_dir)

        # Write source files
        c_files = []
        for f in request.files:
            path = os.path.join(src_dir, f.name)
            with open(path, "w") as fp:
                fp.write(f.content)
            if f.name.endswith(".c"):
                c_files.append(path)

        if not c_files:
            raise HTTPException(status_code=400, detail="No .c source files provided")

        # Compile each .c file
        obj_files = []
        all_stderr = []
        all_errors = []

        for c_file in c_files:
            basename = os.path.splitext(os.path.basename(c_file))[0]
            obj_file = os.path.join(obj_dir, f"{basename}.o")

            cmd = [
                GCC_BINARY,
                *cflags,
                *extra_cflags,
                f"-I{src_dir}",
                "-c",
                "-o", obj_file,
                c_file,
            ]

            logger.info(f"Compiling: {os.path.basename(c_file)}")
            logger.debug(f"Command: {' '.join(cmd)}")

            try:
                result = subprocess.run(
                    cmd,
                    capture_output=True,
                    text=True,
                    timeout=COMPILE_TIMEOUT,
                    cwd=tmpdir,
                )
            except subprocess.TimeoutExpired:
                raise HTTPException(
                    status_code=408,
                    detail=f"Compilation timed out after {COMPILE_TIMEOUT}s"
                )
            except FileNotFoundError:
                raise HTTPException(
                    status_code=500,
                    detail=f"Compiler not found: {GCC_BINARY}"
                )

            if result.stderr:
                all_stderr.append(result.stderr)
                all_errors.extend(_parse_gcc_errors(result.stderr))

            if result.returncode != 0:
                return CompileResponse(
                    success=False,
                    errors=all_errors,
                    raw_output="\n".join(all_stderr),
                ).model_dump()

            obj_files.append(obj_file)

        # Link into shared library
        output_file = os.path.join(tmpdir, f"{request.app_name}.llext")
        link_cmd = [
            GCC_BINARY,
            "-shared", "-nostdlib", "-nodefaultlibs",
            "-fPIC",
            *extra_ldflags,
            "-o", output_file,
            *obj_files,
        ]

        logger.info(f"Linking: {request.app_name}.llext")
        logger.debug(f"Link command: {' '.join(link_cmd)}")

        try:
            result = subprocess.run(
                link_cmd,
                capture_output=True,
                text=True,
                timeout=COMPILE_TIMEOUT,
                cwd=tmpdir,
            )
        except subprocess.TimeoutExpired:
            raise HTTPException(
                status_code=408,
                detail=f"Linking timed out after {COMPILE_TIMEOUT}s"
            )

        if result.stderr:
            all_stderr.append(result.stderr)
            all_errors.extend(_parse_gcc_errors(result.stderr))

        if result.returncode != 0:
            return CompileResponse(
                success=False,
                errors=all_errors,
                raw_output="\n".join(all_stderr),
            ).model_dump()

        # Read the output binary
        with open(output_file, "rb") as f:
            llext_data = f.read()

        logger.info(f"Build successful: {len(llext_data)} bytes")

        return Response(
            content=llext_data,
            media_type="application/octet-stream",
            headers={
                "Content-Disposition": f'attachment; filename="{request.app_name}.llext"',
                "X-Build-Warnings": str(len([e for e in all_errors if e.severity == "warning"])),
            },
        )
