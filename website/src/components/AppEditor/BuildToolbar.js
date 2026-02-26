// Copyright (c) 2025 ZSWatch Project
// SPDX-License-Identifier: Apache-2.0

import React from 'react';

/**
 * Build toolbar — build button, app name input, download button.
 */
export default function BuildToolbar({
  appName,
  onAppNameChange,
  onBuild,
  onDownload,
  onInstall,
  building,
  hasOutput,
  serverOnline,
  bleConnected,
  bleConnecting,
  bleDevice,
  onBleConnect,
  onBleDisconnect,
}) {
  const bleColor = bleConnected ? '#4caf50' : bleConnecting ? '#ffa726' : '#666';
  const bleLabel = bleConnected
    ? `Connected: ${bleDevice || 'ZSWatch'}`
    : bleConnecting
      ? 'Connecting…'
      : 'Disconnected';

  return (
    <div style={styles.toolbar}>
      <div style={styles.left}>
        {/* Build-server status dot */}
        <span
          style={{
            ...styles.dot,
            backgroundColor: serverOnline ? '#4caf50' : '#f44336',
          }}
          title={serverOnline ? 'Build server connected' : 'Build server offline'}
        />

        {/* Editable app name */}
        <label style={styles.nameLabel}>
          App:
          <input
            type="text"
            value={appName}
            onChange={(e) => onAppNameChange(e.target.value)}
            style={styles.nameInput}
            spellCheck={false}
            title="App name used for the LLEXT binary and on-watch ID"
          />
        </label>
      </div>

      <div style={styles.center}>
        {/* BLE status indicator */}
        <button
          onClick={bleConnected ? onBleDisconnect : (bleConnecting ? undefined : onBleConnect)}
          disabled={bleConnecting}
          style={{
            ...styles.bleBtn,
            borderColor: bleColor,
            opacity: bleConnecting ? 0.7 : 1,
            cursor: bleConnecting ? 'wait' : 'pointer',
          }}
          title={bleConnected ? 'Click to disconnect' : 'Click to connect via BLE'}
        >
          <span style={{ ...styles.dot, backgroundColor: bleColor }} />
          <span style={styles.bleLabel}>{bleLabel}</span>
        </button>
      </div>

      <div style={styles.right}>
        <button
          onClick={onBuild}
          disabled={building || !serverOnline}
          style={{
            ...styles.buildBtn,
            ...((building || !serverOnline) ? styles.buildBtnDisabled : {}),
          }}
          title={!serverOnline ? 'Build server is offline' : 'Build app (Ctrl+B)'}
        >
          {building ? '⏳ Building...' : !serverOnline ? '🔨 Server Offline' : '🔨 Build'}
        </button>

        {hasOutput && (
          <button onClick={onDownload} style={styles.downloadBtn}>
            ⬇ Download .llext
          </button>
        )}

        <button
          onClick={onInstall}
          style={{
            ...styles.installBtn,
            ...(bleConnected ? styles.installBtnConnected : {}),
          }}
        >
          📲 Install to Watch
        </button>
      </div>
    </div>
  );
}

const styles = {
  toolbar: {
    display: 'flex',
    justifyContent: 'space-between',
    alignItems: 'center',
    padding: '8px 16px',
    borderBottom: '1px solid var(--ifm-color-emphasis-300)',
    backgroundColor: 'var(--ifm-background-surface-color)',
    gap: '16px',
    flexWrap: 'wrap',
  },
  left: {
    display: 'flex',
    alignItems: 'center',
    gap: '10px',
  },
  center: {
    display: 'flex',
    alignItems: 'center',
  },
  right: {
    display: 'flex',
    alignItems: 'center',
    gap: '8px',
  },
  dot: {
    width: '8px',
    height: '8px',
    borderRadius: '50%',
    display: 'inline-block',
    flexShrink: 0,
  },
  nameLabel: {
    fontSize: '13px',
    color: 'var(--ifm-font-color-secondary)',
    display: 'flex',
    alignItems: 'center',
    gap: '6px',
  },
  nameInput: {
    width: '120px',
    padding: '3px 8px',
    fontSize: '13px',
    fontFamily: 'monospace',
    border: '1px solid var(--ifm-color-emphasis-300)',
    borderRadius: '4px',
    backgroundColor: 'var(--ifm-color-emphasis-100)',
    color: 'inherit',
    outline: 'none',
  },
  bleBtn: {
    display: 'flex',
    alignItems: 'center',
    gap: '6px',
    padding: '4px 12px',
    borderRadius: '12px',
    border: '1px solid #666',
    background: 'none',
    color: 'inherit',
    fontSize: '12px',
    cursor: 'pointer',
  },
  bleLabel: {
    fontSize: '12px',
    whiteSpace: 'nowrap',
  },
  buildBtn: {
    padding: '6px 20px',
    backgroundColor: 'var(--ifm-color-primary)',
    color: '#fff',
    border: 'none',
    borderRadius: '4px',
    cursor: 'pointer',
    fontWeight: 'bold',
    fontSize: '13px',
  },
  buildBtnDisabled: {
    opacity: 0.6,
    cursor: 'not-allowed',
  },
  downloadBtn: {
    padding: '6px 16px',
    backgroundColor: 'var(--ifm-color-success)',
    color: '#fff',
    border: 'none',
    borderRadius: '4px',
    cursor: 'pointer',
    fontWeight: 'bold',
    fontSize: '13px',
  },
  installBtn: {
    padding: '6px 16px',
    backgroundColor: '#1a73e8',
    color: '#fff',
    border: 'none',
    borderRadius: '4px',
    cursor: 'pointer',
    fontWeight: 'bold',
    fontSize: '13px',
  },
  installBtnConnected: {
    boxShadow: '0 0 0 2px #4caf50',
  },
};
