# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

"""
ZSWatch LLEXT App Builder — FastAPI server.

Endpoints:
  POST /api/compile        — Compile source files into an .llext binary
  POST /api/convert-image  — Convert a PNG/JPG to an LVGL v9 C array
  GET  /api/health         — Health check
  GET  /api/edk-versions   — List available EDK versions
"""

import os
import logging

from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
from fastapi.responses import FileResponse, HTMLResponse
from fastapi.staticfiles import StaticFiles

from .compiler import router as compiler_router
from .image_converter import router as image_router

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

app = FastAPI(
    title="ZSWatch App Builder",
    description="Compile LLEXT apps and convert images for ZSWatch",
    version="0.1.0",
)

# CORS — allow the website frontend
app.add_middleware(
    CORSMiddleware,
    allow_origins=[
        "http://localhost:3000",    # Docusaurus dev server
        "http://localhost:3001",
        "https://zswatch.dev",
        "https://apps.zswatch.dev",
        os.environ.get("CORS_ORIGIN", ""),
    ],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

app.include_router(compiler_router, prefix="/api")
app.include_router(image_router, prefix="/api")

# EDK directory - set via environment variable or default
EDK_BASE_DIR = os.environ.get("EDK_BASE_DIR", "/opt/edks")


@app.get("/api/health")
async def health():
    return {"status": "ok"}


@app.get("/api/edk-versions")
async def edk_versions():
    """List available EDK versions."""
    versions = []
    if os.path.isdir(EDK_BASE_DIR):
        for entry in sorted(os.listdir(EDK_BASE_DIR)):
            version_file = os.path.join(EDK_BASE_DIR, entry, "VERSION")
            if os.path.isfile(version_file):
                with open(version_file) as f:
                    version = f.read().strip()
                versions.append({"id": entry, "version": version})
    return {"versions": versions}

