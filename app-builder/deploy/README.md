# ZSWatch App Builder — Deployment Guide

API server for compiling LLEXT apps and converting images for ZSWatch.

The website (Docusaurus) is hosted on GitHub Pages. This server provides
the `/api/compile` and `/api/convert-image` endpoints that the website
calls over HTTPS.

## Architecture

```
  zswatch.dev (GitHub Pages)
        │
        │  HTTPS  /api/compile, /api/convert-image
        ▼
  Railway / your-domain.up.railway.app
  ┌──────────────────────────────────┐
  │  FastAPI container               │
  │   ├── /api/compile               │
  │   ├── /api/convert-image         │
  │   ├── /api/health                │
  │   └── /api/edk-versions          │
  │                                  │
  │  ARM toolchain (Zephyr SDK)      │
  │  EDKs (persistent volume)        │
  └──────────────────────────────────┘
```

---

## Railway (Recommended)

Connect your GitHub repo and Railway auto-deploys on every push.
No server management, no SSH, no Dokku config.

### 1. Create a Railway Project

1. Go to [railway.app](https://railway.app) and sign in with GitHub
2. Click **"New Project" → "Deploy from GitHub Repo"**
3. Select your `ZSWatch` repository
4. Railway detects `railway.toml` and finds the Dockerfile automatically

### 2. Set Environment Variables

In the Railway dashboard, go to your service → **Variables**:

| Variable | Value |
|----------|-------|
| `CORS_ORIGIN` | `https://zswatch.dev` |
| `EDK_BASE_DIR` | `/opt/edks` |
| `COMPILE_TIMEOUT` | `60` |

Railway sets `PORT` automatically — don't set it manually.

### 3. Add a Persistent Volume

EDKs need to persist across deploys:

1. In your service, click **"+ New" → "Volume"**
2. **Mount path:** `/opt/edks`
3. **Size:** 1 GB (plenty for multiple EDK versions)

### 4. Upload an EDK

Generate an EDK from your firmware build:

```bash
source .nrf_env.sh
west build --build-dir app/build_dbg_dk app \
  --board watchdk@1/nrf5340/cpuapp \
  -- -DEXTRA_CONF_FILE="boards/debug.conf;boards/log_on_uart.conf" \
     -DEXTRA_DTC_OVERLAY_FILE="boards/log_on_uart.overlay"

./app/scripts/package_edk.sh \
  app/build_dbg_dk/zephyr/llext-edk.tar.xz \
  $(cat app/VERSION | head -4 | tr '\n' '.' | sed 's/\.$//')
```

Upload via Railway CLI:

```bash
# Install Railway CLI (once)
npm install -g @railway/cli
railway login

# Upload EDK to the volume
railway volume ls                    # find your volume id
railway volume cp zswatch-edk-*.tar.xz /opt/edks/

# Or use railway shell to extract
railway shell
tar -xf /opt/edks/zswatch-edk-*.tar.xz -C /opt/edks/
exit
```

### 5. Set the API URL in the Website

Once Railway gives you a domain (e.g., `zswatch-app-builder-production.up.railway.app`),
update the **GitHub Pages build** to use it.

In your GitHub Actions workflow for the website, set the build env var:

```yaml
- name: Build website
  env:
    API_BASE_URL: https://zswatch-app-builder-production.up.railway.app/api
  run: npm run build
```

Or if using a custom domain (e.g., `api.zswatch.dev`):

```yaml
  env:
    API_BASE_URL: https://api.zswatch.dev/api
```

### 6. (Optional) Custom Domain

1. In Railway dashboard → service → **Settings → Networking**
2. Add custom domain: `api.zswatch.dev`
3. Add the CNAME record Railway tells you to your DNS
4. HTTPS is automatic (free)

### 7. Deploy

That's it — every `git push` to your connected branch triggers a deploy.
Railway builds the Docker image, health-checks `/api/health`, and swaps
traffic with zero downtime.

---

## Commands Reference

```bash
# Install CLI
npm install -g @railway/cli
railway login
railway link       # link to your project

# View logs
railway logs

# Open shell in running container
railway shell

# Restart
railway service restart

# Check status
railway status
```

---

## Alternative: Dokku on DigitalOcean

If you prefer self-hosted, use [`dokku-setup.sh`](dokku-setup.sh):

```bash
scp app-builder/deploy/dokku-setup.sh root@YOUR_IP:/tmp/
ssh root@YOUR_IP 'bash /tmp/dokku-setup.sh apps.zswatch.dev'
git remote add dokku dokku@apps.zswatch.dev:zswatch-apps
git push dokku main
```

See the script for full details.

---

## Environment Variables

| Variable | Default | Description |
|----------|---------|-------------|
| `PORT` | `5000` | Port (set automatically by Railway/Dokku) |
| `EDK_BASE_DIR` | `/opt/edks` | Path to EDK directory |
| `COMPILE_TIMEOUT` | `60` | Max seconds for compilation |
| `CORS_ORIGIN` | *(empty)* | Additional CORS origin to allow |

## File Structure

```
app-builder/
├── Dockerfile                    # Backend-only (local dev)
├── docker-compose.yml            # Local dev compose
├── requirements.txt              # Python dependencies
├── server/
│   ├── main.py                   # FastAPI app
│   ├── compiler.py               # /api/compile endpoint
│   └── image_converter.py        # /api/convert-image endpoint
└── deploy/
    ├── README.md                 # This file
    ├── Dockerfile.dokku          # Production image (API + toolchain)
    ├── CHECKS                    # Dokku health check
    ├── dokku-setup.sh            # Dokku server setup script
    ├── docker-compose.prod.yml   # Alternative: manual compose
    ├── Dockerfile.nginx          # Alternative: nginx image
    └── nginx.conf                # Alternative: nginx config
railway.toml                      # (repo root) Railway build config
```
