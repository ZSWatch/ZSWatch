#!/usr/bin/env bash
# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0
#
# ZSWatch App Builder — VPS deployment script.
#
# One-command deploy:
#   ./deploy.sh [EDK_TARBALL]
#
# This script:
#   1. Extracts the EDK (if provided or found in current dir)
#   2. Builds the Docker images (nginx + app-builder)
#   3. Starts everything with docker compose
#
# Prerequisites:
#   - Docker with compose plugin (docker compose v2)
#   - Internet access (to pull base images & npm packages on first build)
#
# Environment variables:
#   HTTP_PORT          — Port to listen on (default: 80)
#   COMPILE_TIMEOUT    — Max compile time in seconds (default: 60)

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DEPLOY_DIR="$SCRIPT_DIR"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

cd "$DEPLOY_DIR"

# ── Ensure edks/ directory exists ──────────────────────────────────
mkdir -p edks

# ── Extract EDK if provided or auto-detect ─────────────────────────
EDK_TARBALL="${1:-}"

if [ -z "$EDK_TARBALL" ]; then
    # Look for an EDK tarball in common locations
    for f in \
        "$DEPLOY_DIR"/zswatch-edk-*.tar.xz \
        "$PROJECT_ROOT"/zswatch-edk-*.tar.xz \
        "$PROJECT_ROOT"/app/build_dbg_dk/zephyr/llext-edk.tar.xz; do
        if [ -f "$f" ]; then
            EDK_TARBALL="$f"
            echo "Auto-detected EDK: $EDK_TARBALL"
            break
        fi
    done
fi

if [ -n "$EDK_TARBALL" ] && [ -f "$EDK_TARBALL" ]; then
    echo "==> Extracting EDK: $EDK_TARBALL"
    tar -xf "$EDK_TARBALL" -C edks/
    echo "    EDK contents:"
    ls -la edks/
elif [ -z "$(ls -A edks/ 2>/dev/null)" ]; then
    echo "WARNING: No EDK found in edks/ directory."
    echo "  Either provide one as argument: ./deploy.sh zswatch-edk-0.29.0.tar.xz"
    echo "  Or extract one manually:        tar -xf zswatch-edk-*.tar.xz -C edks/"
    echo ""
    echo "  To generate an EDK from firmware source:"
    echo "    west build ... -- -DCONFIG_LLEXT_EDK=y"
    echo "    ./app/scripts/package_edk.sh app/build_dbg_dk/zephyr/llext-edk.tar.xz <version>"
    echo ""
fi

# ── Build and start ────────────────────────────────────────────────
echo "==> Building Docker images..."
docker compose -f docker-compose.prod.yml build

echo "==> Starting services..."
docker compose -f docker-compose.prod.yml up -d

echo ""
echo "================================================"
echo "  ZSWatch App Builder is running!"
echo ""
echo "  Website:  http://localhost:${HTTP_PORT:-80}"
echo "  API:      http://localhost:${HTTP_PORT:-80}/api/health"
echo ""
echo "  Logs:     docker compose -f $(basename "$DEPLOY_DIR")/docker-compose.prod.yml logs -f"
echo "  Stop:     docker compose -f $(basename "$DEPLOY_DIR")/docker-compose.prod.yml down"
echo "================================================"
