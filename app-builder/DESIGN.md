# ZSWatch LLEXT App Builder — System Design Document

## Overview

The LLEXT App Builder lets users **write, compile, and install** ZSWatch apps
entirely from the browser on [zswatch.dev](https://zswatch.dev). It consists of
three parts:

1. **Website** (Docusaurus, hosted on GitHub Pages) — code editor UI
2. **API Server** (FastAPI + ARM cross-compiler, hosted on Railway) — compiles code
3. **Web Bluetooth** (browser-native) — installs `.llext` binaries to the watch

```
┌─────────────────────────────────┐
│  Browser (zswatch.dev)          │
│                                 │
│  Monaco Editor ──► Build ─────────► API Server (Railway)
│       │                         │       │
│       │                         │       ▼
│       │              ◄──────────── .llext binary
│       │                         │
│       ▼                         │
│  Install to Watch ──────────────────► ZSWatch (BLE)
│  (Web Bluetooth)                │
└─────────────────────────────────┘
```

---

## Part 1: EDK (Extension Development Kit)

### What is the EDK?

The EDK is a self-contained package extracted from a ZSWatch firmware build. It
contains everything needed to compile LLEXT apps **without** the full Zephyr/NCS
source tree:

- Pre-extracted compiler flags (`cmake.cflags`)
- All kernel/driver/LVGL headers needed by apps
- ZSWatch-specific headers (app manager, events, UI utils)
- CMake build template
- Toolchain file

### Directory structure inside an EDK

```
zswatch-edk-X.Y.Z/
├── VERSION                    # Firmware version string
├── cmake.cflags               # set(LLEXT_CFLAGS "...") — all compiler flags
├── zswatch_extra.cmake        # ZSWatch-specific: -mlong-calls, extra includes
├── toolchain.cmake            # CMake toolchain file for arm-zephyr-eabi-gcc
├── CMakeLists.txt.template    # CMake template for out-of-tree LLEXT builds
├── include/                   # Full header tree (kernel, LVGL, drivers, etc.)
│   ├── zephyr/
│   ├── gui/lvgl/
│   └── zswatch/               # ZSWatch-specific headers
│       ├── managers/zsw_app_manager.h
│       ├── llext/zsw_llext_iflash.h
│       ├── llext/zsw_llext_log.h
│       ├── events/*.h
│       ├── ui/utils/zsw_ui_utils.h
│       ├── history/
│       ├── sensor_fusion/
│       └── ble/
├── template/                  # App template
│   ├── my_app.c
│   └── CMakeLists.txt
└── Makefile.cflags            # Same flags for Makefile-based builds
```

### Generating an EDK

```bash
# 1. Build firmware (generates the Zephyr EDK tarball)
source .nrf_env.sh
west build --build-dir app/build_dbg_dk app \
  --board watchdk@1/nrf5340/cpuapp \
  -- -DEXTRA_CONF_FILE="boards/debug.conf;boards/log_on_uart.conf" \
     -DEXTRA_DTC_OVERLAY_FILE="boards/log_on_uart.overlay"

# 2. Package the EDK (adds ZSWatch headers, patches flags, bundles template)
./app/scripts/package_edk.sh \
  app/build_dbg_dk/zephyr/llext-edk.tar.xz \
  $(cat app/VERSION | head -4 | tr '\n' '.' | sed 's/\.$//')

# Output: zswatch-edk-X.Y.Z.tar.xz in current directory
```

The `package_edk.sh` script does:
1. Extracts the Zephyr EDK tarball
2. Strips `LV_CONF_PATH` from cflags (not needed for LLEXT apps)
3. Copies ZSWatch-specific headers into `include/zswatch/`
4. Copies missing LVGL root headers (`lvgl.h`, etc.)
5. Adds `zswatch_extra.cmake` with `-mlong-calls` and extra include paths
6. Adds CMake template and toolchain file
7. Writes VERSION file
8. Repackages as `zswatch-edk-X.Y.Z.tar.xz`

### EDK versioning

The EDK version matches the firmware version. A new EDK is needed when:
- Firmware version changes
- Kernel/LVGL API changes
- New ZSWatch APIs are exposed to LLEXT apps

---

## Part 2: API Server (FastAPI)

### Endpoints

| Method | Path | Description |
|--------|------|-------------|
| `POST` | `/api/compile` | Compile source files → `.llext` binary |
| `POST` | `/api/convert-image` | Convert PNG/JPG → LVGL v9 C array |
| `GET`  | `/api/health` | Health check → `{"status": "ok"}` |
| `GET`  | `/api/edk-versions` | List available EDK versions |

### File layout

```
app-builder/
├── Dockerfile                   # Local dev image
├── docker-compose.yml           # Local dev compose (hot-reload)
├── requirements.txt             # Python deps: fastapi, uvicorn, Pillow
├── server/
│   ├── main.py                  # FastAPI app, CORS, health, edk-versions
│   ├── compiler.py              # /api/compile — the core compilation logic
│   └── image_converter.py       # /api/convert-image — PNG→LVGL C array
└── deploy/
    ├── Dockerfile.dokku         # Production image (used by Railway + Dokku)
    ├── README.md                # Deployment guide
    ├── CHECKS                   # Dokku health check
    ├── dokku-setup.sh           # Dokku server setup script
    ├── docker-compose.prod.yml  # Alternative: manual VPS compose
    ├── Dockerfile.nginx         # Alternative: nginx reverse proxy
    └── nginx.conf               # Alternative: nginx config
```

### Compile flow (`/api/compile`)

```
POST /api/compile
  { files: [{name, content}], app_name, edk_version }
      │
      ▼
  1. Find EDK (latest or specific version)
  2. Read cmake.cflags → parse LLEXT_CFLAGS
  3. Read zswatch_extra.cmake → extra cflags + ldflags
  4. For each .c file:
     arm-zephyr-eabi-gcc $CFLAGS -c -o file.o file.c
  5. Link:
     arm-zephyr-eabi-gcc -shared -nostdlib -nodefaultlibs \
       -fPIC $LDFLAGS -o app.llext *.o
      │
      ▼
  Success → 200 + binary (.llext)
  Failure → 200 + JSON {success: false, errors: [...], raw_output: "..."}
```

Key details:
- GCC errors are parsed into structured `{file, line, column, severity, message}`
- The server resolves `${CMAKE_CURRENT_LIST_DIR}` in cflags to the actual EDK path
- Timeout: `COMPILE_TIMEOUT` seconds (default 60)
- Temp directory is used per request and cleaned up automatically

### Image converter (`/api/convert-image`)

Converts PNG/JPG/BMP/GIF to LVGL v9 C arrays. Supports:
- **RGB565** — 16-bit color, no alpha
- **RGB565A8** — 16-bit color + 8-bit alpha map
- **ARGB8888** — 32-bit color with alpha
- **L8** — 8-bit grayscale

Optional resizing via `max_width`/`max_height` (proportional).

### Environment variables

| Variable | Default | Description |
|----------|---------|-------------|
| `PORT` | `5000` (prod) / `8000` (dev) | Listen port |
| `EDK_BASE_DIR` | `/opt/edks` | Directory containing EDK versions |
| `COMPILE_TIMEOUT` | `60` | Max compilation time in seconds |
| `CORS_ORIGIN` | *(empty)* | Extra CORS origin to allow |
| `GCC_BINARY` | `arm-zephyr-eabi-gcc` | Path to cross-compiler |

### EDK directory layout on server

```
/opt/edks/
├── zswatch-edk-0.29.0/
│   ├── VERSION          # "0.29.0"
│   ├── cmake.cflags
│   ├── zswatch_extra.cmake
│   ├── include/
│   └── ...
├── zswatch-edk-0.30.0/
│   ├── VERSION
│   └── ...
```

Multiple EDK versions can coexist. The API picks the latest by default,
or a specific version if `edk_version` is provided in the request.

---

## Part 3: Website (Docusaurus)

### App Editor page

Located at `website/src/components/AppEditor/`. Accessible at `/app-editor`.

| File | Purpose |
|------|---------|
| `index.js` | Main component — state, layout, file persistence (localStorage) |
| `api.js` | API client — `compileApp()`, `convertImage()`, `fetchEdkVersions()`, `checkHealth()` |
| `ble.js` | Web Bluetooth — `connect()`, `disconnect()`, `installApp()` |
| `BuildToolbar.js` | Toolbar — app name, build/download/install buttons, server status |
| `InstallModal.js` | BLE install modal — connect, progress, status |
| `OutputPanel.js` | Build output — errors, warnings, raw GCC output |
| `FileTree.js` | File tree sidebar — create/delete/rename files |
| `DefaultFiles.js` | Default template code pre-loaded for new users |

### API URL configuration

The API URL is configured in `docusaurus.config.js`:

```js
customFields: {
    apiBaseUrl: process.env.API_BASE_URL || 'http://localhost:8000/api',
}
```

`api.js` reads this at runtime:
- `localhost` → `http://localhost:8000/api` (dev)
- Production → value from `apiBaseUrl` (set at GitHub Pages build time)

In the GitHub Actions workflow that builds the website, set:

```yaml
env:
  API_BASE_URL: https://YOUR-RAILWAY-DOMAIN.up.railway.app/api
```

### Web Bluetooth install flow

The install flow uses **Nordic UART Service (NUS)** and **MCUmgr SMP**:

```
Browser                              ZSWatch
   │                                    │
   ├── BLE connect ────────────────────►│
   ├── Discover NUS service             │
   │                                    │
   ├── NUS write: "llext enable_smp" ──►│  Enable SMP service
   ├── Wait for SMP service to appear   │
   │                                    │
   ├── NUS write: "llext mkdir ..." ───►│  Create app directory
   │                                    │
   ├── SMP fs_upload (chunked) ────────►│  Upload .llext file
   │   └── 128-byte chunks with CBOR    │  to /lfs1/apps/<name>/app.llext
   │      headers                       │
   │                                    │
   ├── NUS write: "llext disable_smp" ─►│  Disable SMP service
   ├── NUS write: "llext load <name>" ─►│  Load the app
   │                                    │
   └── Done                             │
```

Requirements:
- Browser must support Web Bluetooth (Chrome/Edge, not Firefox/Safari)
- Site must be served over HTTPS (or localhost)
- Watch must be running firmware with LLEXT + BLE support enabled

---

## Running Locally

### Option A: Run server directly (fastest feedback loop)

```bash
# Terminal 1: Start the API server
cd app-builder
source ../nrf_env.sh   # or wherever your Zephyr SDK is
EDK_BASE_DIR=/path/to/edks uvicorn server.main:app --host 0.0.0.0 --port 8000 --reload --reload-dir server

# Terminal 2: Start the website dev server
cd website
npm install   # first time only
npm start     # opens http://localhost:3000
```

The website dev server auto-detects `localhost` and points the API client
to `http://localhost:8000/api`. Hot-reload works for both.

Requirements:
- `arm-zephyr-eabi-gcc` must be on `$PATH` (from Zephyr SDK or `source .nrf_env.sh`)
- At least one EDK extracted in the `EDK_BASE_DIR` directory

### Option B: Run server via Docker (no local Zephyr SDK needed)

```bash
cd app-builder
mkdir -p edks
tar -xf /path/to/zswatch-edk-X.Y.Z.tar.xz -C edks/
docker compose up       # starts on http://localhost:8000, hot-reloads server/

# In another terminal:
cd website && npm start  # http://localhost:3000
```

The `docker-compose.yml` mounts `./server` for live reload and `./edks` for EDKs.

### Option C: Run everything in Docker (fully isolated)

```bash
cd app-builder/deploy
mkdir -p edks && tar -xf /path/to/zswatch-edk-*.tar.xz -C edks/
docker compose -f docker-compose.prod.yml up --build
# API on http://localhost:8000 (via nginx)
```

---

## Deploying to Production (Railway)

### First-time setup

1. Go to [railway.app](https://railway.app), sign in with GitHub
2. "New Project" → "Deploy from GitHub Repo" → select ZSWatch repo
3. Railway reads `railway.toml` from the repo root and finds the Dockerfile

`railway.toml`:
```toml
[build]
dockerfilePath = "app-builder/deploy/Dockerfile.dokku"

[deploy]
healthcheckPath = "/api/health"
healthcheckTimeout = 30
restartPolicyType = "ON_FAILURE"
restartPolicyMaxRetries = 3
```

4. Set environment variables in Railway dashboard:
   - `CORS_ORIGIN` = `https://zswatch.dev`
   - `EDK_BASE_DIR` = `/opt/edks`
   - `COMPILE_TIMEOUT` = `60`

5. Add a persistent volume:
   - Mount path: `/opt/edks`
   - Size: 1 GB

6. Upload an EDK (see "Updating EDKs" below)

7. After first deploy, optionally add a custom domain (e.g., `api.zswatch.dev`)
   — Railway provides free HTTPS

### Deploying updates

Every `git push` to the connected branch auto-deploys. Railway builds the
Docker image on their servers, runs the health check, and swaps traffic.

### Viewing logs

```bash
npm install -g @railway/cli    # once
railway login
railway link
railway logs
```

---

## Updating EDKs

### When to update

A new EDK is needed when:
- The firmware version changes (new release)
- Kernel or LVGL API changes affect LLEXT apps
- New ZSWatch APIs are exposed to LLEXT apps

### Generating a new EDK

```bash
# Build firmware
source .nrf_env.sh
west build --build-dir app/build_dbg_dk app \
  --board watchdk@1/nrf5340/cpuapp \
  -- -DEXTRA_CONF_FILE="boards/debug.conf;boards/log_on_uart.conf" \
     -DEXTRA_DTC_OVERLAY_FILE="boards/log_on_uart.overlay"

# Package EDK
./app/scripts/package_edk.sh \
  app/build_dbg_dk/zephyr/llext-edk.tar.xz \
  $(cat app/VERSION | head -4 | tr '\n' '.' | sed 's/\.$//')
```

### Updating locally

```bash
# Extract the new EDK into your local edks directory
tar -xf zswatch-edk-X.Y.Z.tar.xz -C /path/to/edks/

# If using Docker, the volume mount picks it up automatically
# If running directly, the server finds it on next /api/edk-versions call
```

### Updating on Railway (production)

```bash
# Option A: Railway CLI
railway login && railway link
railway shell
# Inside the container shell:
#   Upload the EDK tarball somehow (curl, wget from a release URL)
#   tar -xf zswatch-edk-X.Y.Z.tar.xz -C /opt/edks/
exit

# Option B: Use railway volume commands
railway volume cp zswatch-edk-X.Y.Z.tar.xz /opt/edks/
railway shell
tar -xf /opt/edks/zswatch-edk-X.Y.Z.tar.xz -C /opt/edks/
exit

# Option C: Automate in CI — build EDK in GitHub Actions,
# upload to a download URL, then curl it in the container
```

After uploading, restarts are NOT needed — the server discovers EDK versions
dynamically from the `/opt/edks/` directory.

### Updating on Dokku (alternative deployment)

```bash
scp zswatch-edk-X.Y.Z.tar.xz root@YOUR_IP:/tmp/
ssh root@YOUR_IP 'tar -xf /tmp/zswatch-edk-X.Y.Z.tar.xz \
  -C /var/lib/dokku/data/storage/zswatch-apps/edks/'
```

---

## Docker Image Details

The production Dockerfile (`deploy/Dockerfile.dokku`) builds a ~700 MB image:

| Layer | Size | Contents |
|-------|------|----------|
| Ubuntu 24.04 base | ~80 MB | OS + Python 3 |
| Zephyr SDK (ARM only) | ~500 MB | `arm-zephyr-eabi-gcc` and friends |
| Python deps | ~30 MB | FastAPI, uvicorn, Pillow |
| Server code | ~50 KB | `server/main.py`, `compiler.py`, `image_converter.py` |

The image does NOT contain:
- The website (hosted on GitHub Pages)
- EDKs (mounted as a persistent volume)
- The full Zephyr/NCS source tree

Build time: ~3-5 minutes on Railway (Docker layer caching makes subsequent
builds much faster since only the server code layer changes).

---

## CORS Configuration

The API server allows cross-origin requests from:

| Origin | Purpose |
|--------|---------|
| `http://localhost:3000` | Docusaurus dev server |
| `http://localhost:3001` | Alt dev port |
| `https://zswatch.dev` | Production website (GitHub Pages) |
| `https://apps.zswatch.dev` | Legacy/alternative domain |
| `$CORS_ORIGIN` env var | Additional origin (set in Railway) |

---

## Troubleshooting

### "Compiler not found"
- Ensure `arm-zephyr-eabi-gcc` is on `$PATH`
- For local dev: `source .nrf_env.sh`
- In Docker: the Dockerfile adds it to PATH automatically

### "No EDK versions available"
- Check that `EDK_BASE_DIR` points to a directory with extracted EDKs
- Each EDK must have a `VERSION` file at its root
- Run `curl http://localhost:8000/api/edk-versions` to verify

### "CORS error" in browser
- Check the `CORS_ORIGIN` env var on the server
- The request origin must exactly match one of the allowed origins
- For local dev, ensure the website runs on port 3000 or 3001

### Website can't reach the API
- Check `API_BASE_URL` was set when building the Docusaurus site
- Verify with: open browser console → `fetch('/api/health')` or the full URL
- The API server must be running and reachable from the browser (not just the server)

### Web Bluetooth install fails
- Web Bluetooth requires HTTPS (or localhost)
- Only Chrome/Edge support Web Bluetooth (not Firefox/Safari)
- The watch must be in range and have BLE + LLEXT support enabled
- Check browser console for detailed BLE error messages

### Build works locally but fails on server
- Compare EDK versions: `curl https://YOUR-API/api/edk-versions`
- The server and local should use the same EDK version
- Check server logs: `railway logs` or container stdout
