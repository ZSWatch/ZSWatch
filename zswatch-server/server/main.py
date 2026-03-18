# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

"""
ZSWatch Server — FastAPI backend.

Endpoints:
  POST /api/coredump/analyze    — Decode a coredump with GDB
  POST /api/coredump/upload-elf — Upload ELF for crash analysis
  GET  /api/health              — Health check
"""

import os
import logging

from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware

from .coredump import router as coredump_router

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

app = FastAPI(
    title="ZSWatch Server",
    description="Coredump analysis for ZSWatch",
    version="0.2.0",
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

app.include_router(coredump_router, prefix="/api")


@app.get("/api/health")
async def health():
    return {"status": "ok"}
