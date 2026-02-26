// Copyright (c) 2025 ZSWatch Project
// SPDX-License-Identifier: Apache-2.0

import React, { useState } from 'react';

/**
 * Install-to-watch modal dialog.
 *
 * Shows BLE connection status, upload progress, and status messages.
 */
export default function InstallModal({
  open,
  onClose,
  connected,
  connecting,
  deviceName,
  onConnect,
  onDisconnect,
  onInstall,
  installing,
  installStatus,
  installProgress,
  hasBlob,
  installedApps = [],
  onRemoveApp,
  onRefreshApps,
  onReboot,
  loadingApps,
}) {
  if (!open) return null;

  const isSuccess  = installStatus === 'installed';
  const isError    = installStatus?.startsWith('Install failed') ||
                     installStatus?.startsWith('Remove failed') ||
                     installStatus?.startsWith('Connection failed');
  const showProgress = installing && installProgress > 0 && !isSuccess;
  const progressPct  = Math.round((installProgress || 0) * 100);

  return (
    <div style={styles.overlay} onClick={onClose}>
      <div style={styles.modal} onClick={(e) => e.stopPropagation()}>
        {/* Header */}
        <div style={styles.header}>
          <span style={styles.title}>Install to Watch</span>
          <button style={styles.closeBtn} onClick={onClose} title="Close">✕</button>
        </div>

        {/* Body */}
        <div style={styles.body}>
          {/* Web Bluetooth support check */}
          {typeof navigator !== 'undefined' && !navigator.bluetooth && (
            <div style={styles.warning}>
              Web Bluetooth is not available in this browser. Use Chrome or Edge on desktop.
            </div>
          )}

          {/* Connection section */}
          <div style={styles.section}>
            <div style={styles.row}>
              {connecting ? (
                <span style={styles.spinner} />
              ) : (
                <span style={styles.dot(connected)} />
              )}
              <span style={styles.label}>
                {connecting
                  ? 'Connecting…'
                  : connected
                    ? `Connected to ${deviceName || 'ZSWatch'}`
                    : 'Not connected'}
              </span>
            </div>
            {!connected && !connecting && (
              <button style={styles.btn} onClick={onConnect} disabled={installing}>
                Connect to ZSWatch
              </button>
            )}
            {connected && !installing && (
              <button
                style={{ ...styles.btn, ...styles.disconnectBtn }}
                onClick={onDisconnect}
              >
                Disconnect
              </button>
            )}
          </div>

          {/* Install section */}
          {connected && (
            <div style={styles.section}>
              <button
                style={{
                  ...styles.btn,
                  ...styles.installBtn,
                  ...((!hasBlob || installing) ? styles.btnDisabled : {}),
                }}
                onClick={onInstall}
                disabled={!hasBlob || installing}
              >
                {installing ? '⏳ Installing…' : '📲 Install App'}
              </button>
              {!hasBlob && (
                <div style={styles.hint}>Build your app first before installing.</div>
              )}
            </div>
          )}

          {/* Progress bar */}
          {showProgress && (
            <div style={styles.section}>
              <div style={styles.progressTrack}>
                <div
                  style={{ ...styles.progressBar, width: `${progressPct}%` }}
                />
              </div>
              <div style={styles.progressLabel}>{progressPct}%</div>
            </div>
          )}

          {/* Status message */}
          {installStatus && installStatus !== 'installed' && (
            <div style={{
              ...styles.status,
              ...(isError ? styles.statusError : {}),
              ...(installStatus?.startsWith('Removed') ? styles.statusSuccess : {}),
            }}>
              {installStatus}
            </div>
          )}

          {/* Success message */}
          {isSuccess && (
            <div style={styles.success}>
              ✅ App installed and loaded on your watch!
            </div>
          )}

          {/* ── App Management section ── */}
          <div style={styles.section}>
            <div style={styles.row}>
              <div style={styles.sectionTitle}>Installed Apps</div>
              <div style={{ display: 'flex', gap: '6px' }}>
                {connected && (
                  <button
                    style={styles.refreshBtn}
                    onClick={onRefreshApps}
                    disabled={loadingApps || installing}
                    title="Fetch installed apps from watch"
                  >
                    {loadingApps ? '⏳' : '🔄'} {loadingApps ? 'Scanning…' : 'Refresh'}
                  </button>
                )}
                {connected && (
                  <button
                    style={{ ...styles.refreshBtn, ...styles.rebootBtn }}
                    onClick={() => {
                      if (confirm('Reboot the watch? This is needed to apply app changes.')) {
                        onReboot();
                      }
                    }}
                    disabled={installing}
                    title="Reboot watch to apply changes (delete/update apps)"
                  >
                    🔁 Reboot
                  </button>
                )}
              </div>
            </div>

            <div style={styles.hint}>
              ℹ️ A reboot is needed after deleting or re-installing apps to apply changes.
            </div>

            {installedApps.length > 0 && (
              <div style={styles.appList}>
                {installedApps.map((app) => (
                  <div key={app.id} style={styles.appItem}>
                    <span style={styles.appName}>
                      {app.name || app.id}
                      {app.size ? ` (${(app.size / 1024).toFixed(1)} KB)` : ''}
                    </span>
                    <button
                      style={styles.removeBtn}
                      onClick={() => {
                        if (confirm(`Remove "${app.name || app.id}" from the watch?`)) {
                          onRemoveApp(app.id);
                        }
                      }}
                      disabled={!connected || installing}
                      title={connected ? 'Remove app from watch' : 'Connect to watch first'}
                    >
                      🗑
                    </button>
                  </div>
                ))}
              </div>
            )}

            {connected && installedApps.length === 0 && !loadingApps && (
              <div style={styles.hint}>
                No apps found. Press Refresh to scan the watch.
              </div>
            )}
          </div>
        </div>
      </div>
    </div>
  );
}

const styles = {
  overlay: {
    position: 'fixed',
    inset: 0,
    backgroundColor: 'rgba(0, 0, 0, 0.6)',
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'center',
    zIndex: 9999,
  },
  modal: {
    backgroundColor: 'var(--ifm-background-surface-color, #1e1e1e)',
    border: '1px solid var(--ifm-color-emphasis-300, #444)',
    borderRadius: '8px',
    width: '440px',
    maxWidth: '90vw',
    maxHeight: '80vh',
    overflowY: 'auto',
    boxShadow: '0 8px 32px rgba(0, 0, 0, 0.5)',
  },
  header: {
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'space-between',
    padding: '14px 18px',
    borderBottom: '1px solid var(--ifm-color-emphasis-300, #444)',
  },
  title: {
    fontWeight: 'bold',
    fontSize: '15px',
  },
  closeBtn: {
    background: 'none',
    border: 'none',
    color: 'var(--ifm-color-emphasis-600, #aaa)',
    fontSize: '16px',
    cursor: 'pointer',
    padding: '2px 6px',
    borderRadius: '4px',
  },
  body: {
    padding: '18px',
    display: 'flex',
    flexDirection: 'column',
    gap: '16px',
  },
  section: {
    display: 'flex',
    flexDirection: 'column',
    gap: '8px',
  },
  sectionTitle: {
    fontSize: '12px',
    fontWeight: 'bold',
    textTransform: 'uppercase',
    letterSpacing: '0.5px',
    color: 'var(--ifm-color-emphasis-600, #aaa)',
  },
  row: {
    display: 'flex',
    alignItems: 'center',
    gap: '8px',
  },
  dot: (on) => ({
    width: '10px',
    height: '10px',
    borderRadius: '50%',
    backgroundColor: on ? '#4caf50' : '#666',
    flexShrink: 0,
  }),
  spinner: {
    width: '10px',
    height: '10px',
    borderRadius: '50%',
    border: '2px solid #ffa726',
    borderTopColor: 'transparent',
    animation: 'spin 0.8s linear infinite',
    flexShrink: 0,
  },
  label: {
    fontSize: '13px',
  },
  btn: {
    padding: '8px 16px',
    borderRadius: '6px',
    border: '1px solid var(--ifm-color-emphasis-400, #555)',
    backgroundColor: 'var(--ifm-color-emphasis-200, #333)',
    color: 'inherit',
    fontSize: '13px',
    cursor: 'pointer',
    fontWeight: 500,
    textAlign: 'center',
  },
  installBtn: {
    backgroundColor: '#1a73e8',
    borderColor: '#1a73e8',
    color: '#fff',
  },
  disconnectBtn: {
    fontSize: '12px',
    padding: '5px 12px',
    opacity: 0.8,
  },
  btnDisabled: {
    opacity: 0.5,
    cursor: 'not-allowed',
  },
  hint: {
    fontSize: '12px',
    color: 'var(--ifm-color-emphasis-500, #888)',
  },
  progressTrack: {
    height: '6px',
    borderRadius: '3px',
    backgroundColor: 'var(--ifm-color-emphasis-200, #333)',
    overflow: 'hidden',
  },
  progressBar: {
    height: '100%',
    borderRadius: '3px',
    backgroundColor: '#1a73e8',
    transition: 'width 0.15s ease',
  },
  progressLabel: {
    fontSize: '12px',
    color: 'var(--ifm-color-emphasis-500, #888)',
    textAlign: 'center',
  },
  status: {
    fontSize: '12px',
    color: 'var(--ifm-color-emphasis-600, #aaa)',
    padding: '8px 12px',
    backgroundColor: 'var(--ifm-color-emphasis-100, #252525)',
    borderRadius: '4px',
  },
  statusError: {
    color: '#ef5350',
    backgroundColor: 'rgba(239, 83, 80, 0.1)',
  },
  statusSuccess: {
    color: '#4caf50',
    backgroundColor: 'rgba(76, 175, 80, 0.1)',
  },
  success: {
    fontSize: '14px',
    fontWeight: 500,
    color: '#4caf50',
    padding: '12px',
    backgroundColor: 'rgba(76, 175, 80, 0.1)',
    borderRadius: '6px',
    textAlign: 'center',
  },
  warning: {
    fontSize: '12px',
    color: '#ffa726',
    padding: '8px 12px',
    backgroundColor: 'rgba(255, 167, 38, 0.1)',
    borderRadius: '4px',
  },
  appList: {
    display: 'flex',
    flexDirection: 'column',
    gap: '4px',
  },
  appItem: {
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'space-between',
    padding: '6px 10px',
    backgroundColor: 'var(--ifm-color-emphasis-100, #252525)',
    borderRadius: '4px',
    fontSize: '13px',
  },
  appName: {
    flex: 1,
    overflow: 'hidden',
    textOverflow: 'ellipsis',
    whiteSpace: 'nowrap',
  },
  removeBtn: {
    background: 'none',
    border: 'none',
    cursor: 'pointer',
    fontSize: '14px',
    padding: '2px 6px',
    borderRadius: '4px',
    opacity: 0.7,
  },
  refreshBtn: {
    padding: '3px 10px',
    borderRadius: '4px',
    border: '1px solid var(--ifm-color-emphasis-400, #555)',
    backgroundColor: 'var(--ifm-color-emphasis-200, #333)',
    color: 'inherit',
    fontSize: '11px',
    cursor: 'pointer',
    whiteSpace: 'nowrap',
  },
  rebootBtn: {
    borderColor: '#c0392b',
    color: '#e74c3c',
  },
};
