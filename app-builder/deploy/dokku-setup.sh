#!/usr/bin/env bash
# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0
#
# ZSWatch App Builder — Dokku server setup script.
#
# Run this ONCE on a fresh DigitalOcean droplet (or any Ubuntu VPS) to:
#   1. Install Dokku
#   2. Create the app
#   3. Configure persistent storage for EDKs
#   4. Set up Let's Encrypt HTTPS
#   5. Configure git remote for push-to-deploy
#
# Usage (run on the VPS as root):
#   curl -sL https://raw.githubusercontent.com/jakkra/ZSWatch/main/app-builder/deploy/dokku-setup.sh | bash -s YOUR_DOMAIN
#
# Or copy this file to the server and run:
#   chmod +x dokku-setup.sh
#   ./dokku-setup.sh apps.zswatch.dev
#
# After setup, deploy from your dev machine:
#   git remote add dokku dokku@YOUR_VPS:zswatch-apps
#   git push dokku main

set -euo pipefail

DOMAIN="${1:-}"
APP_NAME="zswatch-apps"

if [ -z "$DOMAIN" ]; then
    echo "Usage: $0 <domain>"
    echo "  e.g.: $0 apps.zswatch.dev"
    echo ""
    echo "  The domain should already have a DNS A record pointing to this server."
    exit 1
fi

echo "==> Setting up ZSWatch App Builder on Dokku"
echo "    App name: $APP_NAME"
echo "    Domain:   $DOMAIN"
echo ""

# ── 1. Install Dokku (if not present) ──────────────────────────────
if ! command -v dokku &> /dev/null; then
    echo "==> Installing Dokku..."
    wget -NP . https://dokku.com/bootstrap.sh
    sudo DOKKU_TAG=v0.35.18 bash bootstrap.sh
    echo "==> Dokku installed."
else
    echo "==> Dokku already installed: $(dokku version)"
fi

# ── 2. Create app ──────────────────────────────────────────────────
if dokku apps:exists "$APP_NAME" 2>/dev/null; then
    echo "==> App '$APP_NAME' already exists, skipping creation."
else
    echo "==> Creating app: $APP_NAME"
    dokku apps:create "$APP_NAME"
fi

# ── 3. Set the Dockerfile path ─────────────────────────────────────
# The Dockerfile is not at the repo root — tell Dokku where it is
echo "==> Configuring Dockerfile path..."
dokku docker-options:set "$APP_NAME" build "--file app-builder/deploy/Dockerfile.dokku"

# ── 4. Persistent storage for EDKs ─────────────────────────────────
echo "==> Setting up persistent storage for EDKs..."
sudo mkdir -p /var/lib/dokku/data/storage/$APP_NAME/edks
sudo chown -R dokku:dokku /var/lib/dokku/data/storage/$APP_NAME
dokku storage:mount "$APP_NAME" /var/lib/dokku/data/storage/$APP_NAME/edks:/opt/edks

# ── 5. Environment variables ───────────────────────────────────────
echo "==> Setting environment variables..."
dokku config:set --no-restart "$APP_NAME" \
    EDK_BASE_DIR=/opt/edks \
    COMPILE_TIMEOUT=60

# ── 6. Domain & HTTPS ──────────────────────────────────────────────
echo "==> Configuring domain: $DOMAIN"
dokku domains:set "$APP_NAME" "$DOMAIN"

# Install letsencrypt plugin if not present
if ! dokku plugin:list | grep -q letsencrypt; then
    echo "==> Installing Let's Encrypt plugin..."
    sudo dokku plugin:install https://github.com/dokku/dokku-letsencrypt.git
fi

echo "==> Setting up Let's Encrypt..."
dokku letsencrypt:set "$APP_NAME" email "admin@$DOMAIN"
# Note: HTTPS cert will be provisioned after the first deploy

# ── 7. Proxy configuration ─────────────────────────────────────────
# Increase proxy timeouts for compilation requests
dokku nginx:set "$APP_NAME" proxy-read-timeout 120s
dokku nginx:set "$APP_NAME" proxy-send-timeout 120s
dokku nginx:set "$APP_NAME" client-max-body-size 10m

# ── 8. Health check ────────────────────────────────────────────────
# Copy CHECKS file for zero-downtime deploy
sudo mkdir -p /var/lib/dokku/data/storage/$APP_NAME
sudo cp "$(dirname "$0")/CHECKS" /var/lib/dokku/data/storage/$APP_NAME/CHECKS 2>/dev/null || true

echo ""
echo "========================================================"
echo "  Dokku setup complete!"
echo ""
echo "  Next steps:"
echo ""
echo "  1. Upload an EDK to the server:"
echo "     scp zswatch-edk-*.tar.xz root@$DOMAIN:/tmp/"
echo "     ssh root@$DOMAIN 'tar -xf /tmp/zswatch-edk-*.tar.xz -C /var/lib/dokku/data/storage/$APP_NAME/edks/'"
echo ""
echo "  2. Add your SSH key (if not already done):"
echo "     cat ~/.ssh/id_rsa.pub | ssh root@$DOMAIN 'dokku ssh-keys:add admin'"
echo ""
echo "  3. Deploy from your dev machine:"
echo "     cd /path/to/ZSWatchDev"
echo "     git remote add dokku dokku@$DOMAIN:$APP_NAME"
echo "     git push dokku main"
echo ""
echo "  4. After first deploy, enable HTTPS:"
echo "     ssh root@$DOMAIN 'dokku letsencrypt:enable $APP_NAME'"
echo "     ssh root@$DOMAIN 'dokku letsencrypt:cron-job --add'"
echo ""
echo "  Site will be at: https://$DOMAIN"
echo "========================================================"
