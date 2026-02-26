// Copyright (c) 2025 ZSWatch Project
// SPDX-License-Identifier: Apache-2.0

import React from 'react';

/**
 * Output panel — shows compiler output, errors, and warnings.
 */
export default function OutputPanel({ output, errors, building }) {
  const errorCount = errors.filter((e) => e.severity === 'error').length;
  const warnCount = errors.filter((e) => e.severity === 'warning').length;

  return (
    <div style={styles.container}>
      <div style={styles.header}>
        <span style={styles.title}>Build Output</span>
        {errors.length > 0 && (
          <span style={styles.counts}>
            {errorCount > 0 && (
              <span style={styles.errorCount}>
                {errorCount} error{errorCount !== 1 ? 's' : ''}
              </span>
            )}
            {warnCount > 0 && (
              <span style={styles.warnCount}>
                {warnCount} warning{warnCount !== 1 ? 's' : ''}
              </span>
            )}
          </span>
        )}
        {building && <span style={styles.spinner}>⏳</span>}
      </div>

      <div style={styles.content}>
        {errors.length > 0 && (
          <div style={styles.errorList}>
            {errors.map((err, i) => (
              <div
                key={i}
                style={{
                  ...styles.errorItem,
                  ...(err.severity === 'error'
                    ? styles.errorSeverity
                    : err.severity === 'warning'
                      ? styles.warnSeverity
                      : styles.noteSeverity),
                }}
              >
                <span style={styles.errorBadge}>
                  {err.severity === 'error' ? '❌' : err.severity === 'warning' ? '⚠️' : 'ℹ️'}
                </span>
                <span style={styles.errorLocation}>
                  {err.file && (
                    <>
                      {err.file}
                      {err.line && `:${err.line}`}
                      {err.column && `:${err.column}`}
                    </>
                  )}
                </span>
                <span style={styles.errorMessage}>{err.message}</span>
              </div>
            ))}
          </div>
        )}

        {output && (
          <pre style={styles.rawOutput}>{output}</pre>
        )}

        {!output && errors.length === 0 && !building && (
          <div style={styles.placeholder}>
            Click <strong>Build</strong> to compile your app.
          </div>
        )}
      </div>
    </div>
  );
}

const styles = {
  container: {
    display: 'flex',
    flexDirection: 'column',
    height: '100%',
    borderTop: '1px solid var(--ifm-color-emphasis-300)',
    backgroundColor: 'var(--ifm-background-surface-color)',
  },
  header: {
    display: 'flex',
    alignItems: 'center',
    gap: '12px',
    padding: '6px 12px',
    borderBottom: '1px solid var(--ifm-color-emphasis-300)',
    minHeight: '32px',
  },
  title: {
    fontWeight: 'bold',
    fontSize: '12px',
    textTransform: 'uppercase',
    letterSpacing: '0.5px',
  },
  counts: {
    display: 'flex',
    gap: '8px',
    fontSize: '12px',
  },
  errorCount: {
    color: 'var(--ifm-color-danger)',
    fontWeight: 'bold',
  },
  warnCount: {
    color: 'var(--ifm-color-warning-darkest)',
    fontWeight: 'bold',
  },
  spinner: {
    marginLeft: 'auto',
  },
  content: {
    flex: 1,
    overflowY: 'auto',
    padding: '8px 12px',
    fontSize: '13px',
    fontFamily: 'var(--ifm-font-family-monospace)',
  },
  errorList: {
    display: 'flex',
    flexDirection: 'column',
    gap: '4px',
    marginBottom: '8px',
  },
  errorItem: {
    display: 'flex',
    alignItems: 'flex-start',
    gap: '8px',
    padding: '4px 8px',
    borderRadius: '4px',
    fontSize: '12px',
    lineHeight: 1.5,
  },
  errorSeverity: {
    backgroundColor: 'rgba(244, 67, 54, 0.1)',
    borderLeft: '3px solid var(--ifm-color-danger)',
  },
  warnSeverity: {
    backgroundColor: 'rgba(255, 152, 0, 0.1)',
    borderLeft: '3px solid var(--ifm-color-warning)',
  },
  noteSeverity: {
    backgroundColor: 'rgba(33, 150, 243, 0.1)',
    borderLeft: '3px solid var(--ifm-color-info)',
  },
  errorBadge: {
    flexShrink: 0,
    fontSize: '12px',
  },
  errorLocation: {
    flexShrink: 0,
    color: 'var(--ifm-color-emphasis-700)',
    fontWeight: 'bold',
    minWidth: '80px',
  },
  errorMessage: {
    flex: 1,
    wordBreak: 'break-word',
  },
  rawOutput: {
    margin: 0,
    padding: '8px',
    backgroundColor: 'var(--ifm-background-color)',
    borderRadius: '4px',
    fontSize: '11px',
    lineHeight: 1.5,
    whiteSpace: 'pre-wrap',
    wordBreak: 'break-all',
    maxHeight: '200px',
    overflowY: 'auto',
  },
  placeholder: {
    color: 'var(--ifm-color-emphasis-500)',
    textAlign: 'center',
    padding: '20px',
    fontFamily: 'var(--ifm-font-family-base)',
  },
};
