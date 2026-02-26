// Copyright (c) 2025 ZSWatch Project
// SPDX-License-Identifier: Apache-2.0

/**
 * ZSWatch Web Bluetooth module.
 *
 * Handles BLE connection, NUS (Gadgetbridge JSON) commands, and MCUmgr SMP
 * file upload for installing LLEXT apps directly from the browser.
 *
 * SMP file upload is delegated to the existing MCUManager library
 * (mcumgr-web) which handles CBOR encoding, BLE chunking, response
 * reassembly, timeouts and retries.
 *
 * Upload flow:
 *  1. Connect BLE, discover NUS service
 *  2. Enable SMP via NUS command → firmware registers SMP GATT service
 *  3. Attach MCUManager to the existing GATT server (discovers SMP service)
 *  4. Send mkdir command via NUS → firmware creates /lvgl_lfs/apps/<id>/
 *  5. Upload .llext file via MCUManager's cmdUploadFileSystemImage()
 *  6. Disable SMP via NUS → firmware re-enables XIP
 *  7. Send load command via NUS → firmware hot-loads the app
 */

import MCUManager from '../../mcumgr-web/js/mcumgr';

// ── BLE UUIDs ──────────────────────────────────────────────────────────────────

const NUS_SERVICE = '6e400001-b5a3-f393-e0a9-e50e24dcca9e';
const NUS_RX_CHAR = '6e400002-b5a3-f393-e0a9-e50e24dcca9e'; // write TO watch
const NUS_TX_CHAR = '6e400003-b5a3-f393-e0a9-e50e24dcca9e'; // notify FROM watch

const SMP_SERVICE = '8d53dc1d-1db7-4cd3-868b-8a527460aa84';

const MAX_BLE_WRITE        = 240;   // conservative limit for 247-byte MTU
const SMP_DISCOVER_RETRIES = 3;
const SMP_DISCOVER_DELAY   = 2000;  // ms to wait after enabling SMP

// ── Module State ───────────────────────────────────────────────────────────────

let device = null;
let server = null;
let nusRx  = null;
let disconnectCbs = [];

// ── Helpers ────────────────────────────────────────────────────────────────────

function sleep(ms) {
  return new Promise((r) => setTimeout(r, ms));
}

// ── NUS Client ─────────────────────────────────────────────────────────────────

async function nusSend(json) {
  const msg = new TextEncoder().encode(`GB(${json})\n`);
  for (let off = 0; off < msg.length; off += MAX_BLE_WRITE) {
    const end = Math.min(off + MAX_BLE_WRITE, msg.length);
    await nusRx.writeValueWithoutResponse(msg.slice(off, end));
  }
}

// ── Public API ─────────────────────────────────────────────────────────────────

/** true if currently connected to a ZSWatch device */
export function isConnected() {
  return device?.gatt?.connected === true;
}

/** Connected device name, or null */
export function getDeviceName() {
  return device?.name ?? null;
}

/** Register a callback for disconnection events */
export function onDisconnect(cb) {
  disconnectCbs.push(cb);
  return () => { disconnectCbs = disconnectCbs.filter((c) => c !== cb); };
}

/** Pair and connect to a ZSWatch via Web Bluetooth */
export async function connect() {
  if (isConnected()) return;

  if (!navigator.bluetooth) {
    throw new Error('Web Bluetooth is not supported. Use Chrome or Edge on desktop.');
  }

  device = await navigator.bluetooth.requestDevice({
    filters: [{ namePrefix: 'ZSWatch' }],
    optionalServices: [NUS_SERVICE, SMP_SERVICE],
  });

  device.addEventListener('gattserverdisconnected', () => {
    nusRx = null;
    for (const cb of disconnectCbs) cb();
  });

  server = await device.gatt.connect();

  // Discover NUS service
  const nus = await server.getPrimaryService(NUS_SERVICE);
  nusRx = await nus.getCharacteristic(NUS_RX_CHAR);
  const nusTx = await nus.getCharacteristic(NUS_TX_CHAR);
  await nusTx.startNotifications();
}

/** Disconnect from the watch */
export async function disconnect() {
  if (device?.gatt?.connected) device.gatt.disconnect();
  device = null;
  server = null;
  nusRx = null;
}

/**
 * Install a compiled LLEXT app on the connected ZSWatch.
 *
 * @param {string}   appName    App name (e.g. "my_app")
 * @param {Blob}     llextBlob  Compiled .llext binary
 * @param {function} onStatus   (statusText: string) => void
 * @param {function} onProgress (fraction: number 0-1) => void
 */
export async function installApp(appName, llextBlob, onStatus, onProgress) {
  const appId   = appName.endsWith('_ext') ? appName : `${appName}_ext`;
  const filePath = `/lvgl_lfs/apps/${appId}/app.llext`;

  console.log(`[ble] Installing LLEXT app: ${appId} (${llextBlob.size} bytes)`);

  // ── Step 1: Enable SMP BLE service ──
  onStatus?.('Enabling SMP transport…');
  await nusSend(JSON.stringify({ t: 'smp', status: true }));

  // ── Step 2: Wait for SMP GATT service to register, then attach MCUManager ──
  onStatus?.('Waiting for SMP service…');

  // We need to wait for the firmware to register the SMP service before
  // MCUManager can discover it.  Retry discovery a few times.
  let mcumgr = null;
  for (let attempt = 0; attempt < SMP_DISCOVER_RETRIES; attempt++) {
    await sleep(SMP_DISCOVER_DELAY);
    try {
      mcumgr = new MCUManager();
      await mcumgr.connectWithServer(server);
      console.log('[ble] MCUManager attached to SMP service');
      break;
    } catch {
      console.log(`[ble] SMP discovery attempt ${attempt + 1}/${SMP_DISCOVER_RETRIES} failed`);
      mcumgr = null;
      if (attempt === SMP_DISCOVER_RETRIES - 1) {
        throw new Error(
          'Could not discover SMP service. The watch may need to be reconnected.'
        );
      }
    }
  }

  try {
    // ── Step 3: Create app directory ──
    onStatus?.('Creating app directory…');
    await nusSend(JSON.stringify({ t: 'llext', op: 'mkdir', id: appId }));
    await sleep(500);

    // ── Step 4: Upload .llext binary via MCUManager ──
    onStatus?.('Uploading app…');
    const fileData = await llextBlob.arrayBuffer();
    console.log(`[ble] File data size: ${fileData.byteLength} bytes`);
    if (!fileData.byteLength) {
      throw new Error('LLEXT binary is empty — nothing to upload');
    }

    // Wire up progress and completion callbacks
    const uploadDone = new Promise((resolve, reject) => {
      mcumgr.onFsUploadProgress(({ percentage }) => {
        onProgress?.(percentage / 100);
      });
      mcumgr.onFsUploadFinished((ok) => {
        if (ok) {
          resolve();
        } else {
          reject(new Error('SMP filesystem upload failed'));
        }
      });
    });

    await mcumgr.cmdUploadFileSystemImage(fileData, filePath);
    await uploadDone;

    console.log('[ble] SMP upload complete');

    // ── Step 5: Disable SMP (re-enables XIP) ──
    onStatus?.('Finalizing…');
    await mcumgr.detach();
    mcumgr = null;
    await nusSend(JSON.stringify({ t: 'smp', status: false }));
    await sleep(1000);

    // ── Step 6: Hot-load the app ──
    onStatus?.('Loading app on watch…');
    await nusSend(JSON.stringify({ t: 'llext', op: 'load', id: appId }));
    await sleep(500);

    onStatus?.('installed');
    console.log(`[ble] App '${appId}' installed successfully`);
    return true;
  } catch (err) {
    console.error('[ble] Install failed:', err);
    onStatus?.(`Install failed: ${err.message}`);
    throw err;
  } finally {
    // Clean up MCUManager
    if (mcumgr) {
      try { await mcumgr.detach(); } catch { /* ignore */ }
    }
  }
}

/**
 * Remove an LLEXT app from the connected ZSWatch.
 * Only requires NUS — SMP is NOT needed.
 * Note: the app remains loaded in memory until reboot.
 *
 * @param {string} appName  App name (e.g. "my_app")
 */
export async function removeApp(appName) {
  const appId = appName.endsWith('_ext') ? appName : `${appName}_ext`;
  console.log(`[ble] Removing LLEXT app: ${appId}`);
  await nusSend(JSON.stringify({ t: 'llext', op: 'rm', id: appId }));
  console.log(`[ble] Remove command sent for '${appId}'`);
}

/**
 * Reboot the connected ZSWatch.
 * Sends the reset NUS command — the watch will disconnect and restart.
 */
export async function rebootWatch() {
  console.log('[ble] Sending reboot command');
  await nusSend(JSON.stringify({ t: 'reset' }));
}

/**
 * List installed LLEXT apps by probing known paths on the watch filesystem.
 *
 * Requires BLE connection.  Enables SMP temporarily, probes each known app
 * path via MCUManager's cmdFileStatus(), then disables SMP.
 *
 * @param {function} onStatus  Optional (statusText: string) => void
 * @param {string[]} extraIds  Additional app IDs to probe (e.g. user-installed)
 * @returns {Promise<Array<{id: string, name: string, size: number}>>}
 */
export async function listInstalledApps(onStatus, extraIds = []) {
  if (extraIds.length === 0) {
    console.log('[ble] No app IDs to probe — nothing to scan');
    return [];
  }

  const allIds = [...new Set(extraIds)];

  console.log('[ble] Listing installed LLEXT apps…');
  onStatus?.('Enabling SMP to probe apps…');

  await nusSend(JSON.stringify({ t: 'smp', status: true }));

  let mcumgr = null;
  for (let attempt = 0; attempt < SMP_DISCOVER_RETRIES; attempt++) {
    await sleep(SMP_DISCOVER_DELAY);
    try {
      mcumgr = new MCUManager();
      await mcumgr.connectWithServer(server);
      break;
    } catch {
      mcumgr = null;
      if (attempt === SMP_DISCOVER_RETRIES - 1) {
        await nusSend(JSON.stringify({ t: 'smp', status: false }));
        throw new Error('Could not discover SMP service.');
      }
    }
  }

  const found = [];
  try {
    onStatus?.('Scanning for installed apps…');
    for (const appId of allIds) {
      const path = `/lvgl_lfs/apps/${appId}/app.llext`;
      try {
        const stat = await mcumgr.cmdFileStatus(path);
        const name = appId.replace(/_ext$/, '');
        found.push({ id: appId, name, size: stat.len });
        console.log(`[ble]   found: ${appId} (${stat.len} bytes)`);
      } catch {
        // File not found — not installed, skip
      }
    }
    console.log(`[ble] Found ${found.length} installed app(s)`);
  } finally {
    if (mcumgr) {
      try { await mcumgr.detach(); } catch { /* ignore */ }
    }
    await nusSend(JSON.stringify({ t: 'smp', status: false }));
    await sleep(500);
  }

  onStatus?.('');
  return found;
}
