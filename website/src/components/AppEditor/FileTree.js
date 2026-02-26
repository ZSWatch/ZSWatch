// Copyright (c) 2025 ZSWatch Project
// SPDX-License-Identifier: Apache-2.0

import React, { useState } from 'react';

/**
 * File tree sidebar — lists source files, allows creating/deleting files
 * and uploading images via drag-and-drop.
 */
export default function FileTree({
  files,
  activeFile,
  onSelectFile,
  onCreateFile,
  onDeleteFile,
  onRenameFile,
  onUploadImage,
}) {
  const [dragOver, setDragOver] = useState(false);

  const handleNewFile = () => {
    const name = prompt('File name (e.g. my_ui.c or my_ui.h):');
    if (name && (name.endsWith('.c') || name.endsWith('.h'))) {
      onCreateFile(name);
    } else if (name) {
      alert('Only .c and .h files are supported.');
    }
  };

  const handleRename = (oldName) => {
    const newName = prompt(`Rename "${oldName}" to:`, oldName);
    if (!newName) return;
    if (!newName.endsWith('.c') && !newName.endsWith('.h')) {
      alert('Only .c and .h files are supported.');
      return;
    }
    onRenameFile(oldName, newName);
  };

  const handleDrop = (e) => {
    e.preventDefault();
    setDragOver(false);
    const file = e.dataTransfer.files[0];
    if (file && file.type.startsWith('image/')) {
      onUploadImage(file);
    }
  };

  const cFiles = Object.keys(files).filter(n => n.endsWith('.c') || n.endsWith('.h'));
  const canDelete = cFiles.length > 1;

  return (
    <div style={styles.container}>
      <div style={styles.header}>
        <span style={styles.title}>Files</span>
        <div style={styles.actions}>
          <button
            onClick={handleNewFile}
            style={styles.btn}
            title="New file"
          >
            +
          </button>
        </div>
      </div>

      <div style={styles.fileList}>
        {cFiles.map((name) => (
          <div
            key={name}
            style={{
              ...styles.fileItem,
              ...(name === activeFile ? styles.fileItemActive : {}),
            }}
            onClick={() => onSelectFile(name)}
          >
            <span style={styles.fileIcon}>
              {name.endsWith('.h') ? '📄' : '📝'}
            </span>
            <span style={styles.fileName}>{name}</span>
            <button
              onClick={(e) => {
                e.stopPropagation();
                handleRename(name);
              }}
              style={styles.actionBtn}
              title="Rename file"
            >
              ✏️
            </button>
            {canDelete && (
              <button
                onClick={(e) => {
                  e.stopPropagation();
                  if (confirm(`Delete ${name}?`)) {
                    onDeleteFile(name);
                  }
                }}
                style={styles.deleteBtn}
                title="Delete file"
              >
                ×
              </button>
            )}
          </div>
        ))}
      </div>

      <div
        style={{
          ...styles.dropZone,
          ...(dragOver ? styles.dropZoneActive : {}),
        }}
        onDragOver={(e) => {
          if ([...e.dataTransfer.types].includes('Files')) {
            e.preventDefault();
            setDragOver(true);
          }
        }}
        onDragLeave={() => setDragOver(false)}
        onDrop={handleDrop}
      >
        <span style={styles.dropIcon}>🖼</span>
        <span style={styles.dropText}>Drop image here</span>
        <span style={styles.dropHint}>PNG, JPG, BMP → LVGL C array</span>
      </div>
    </div>
  );
}

const styles = {
  container: {
    display: 'flex',
    flexDirection: 'column',
    height: '100%',
    borderRight: '1px solid var(--ifm-color-emphasis-300)',
    backgroundColor: 'var(--ifm-background-surface-color)',
  },
  header: {
    display: 'flex',
    justifyContent: 'space-between',
    alignItems: 'center',
    padding: '8px 12px',
    borderBottom: '1px solid var(--ifm-color-emphasis-300)',
  },
  title: {
    fontWeight: 'bold',
    fontSize: '13px',
    textTransform: 'uppercase',
    letterSpacing: '0.5px',
  },
  actions: {
    display: 'flex',
    gap: '4px',
  },
  btn: {
    background: 'none',
    border: '1px solid var(--ifm-color-emphasis-300)',
    borderRadius: '4px',
    cursor: 'pointer',
    padding: '2px 8px',
    fontSize: '14px',
    color: 'var(--ifm-font-color-base)',
  },
  fileList: {
    flex: 1,
    overflowY: 'auto',
    padding: '4px 0',
  },
  fileItem: {
    display: 'flex',
    alignItems: 'center',
    padding: '6px 12px',
    cursor: 'pointer',
    fontSize: '13px',
    gap: '6px',
    transition: 'background-color 0.1s',
  },
  fileItemActive: {
    backgroundColor: 'var(--ifm-color-primary-lightest)',
    fontWeight: 'bold',
  },
  fileIcon: {
    fontSize: '12px',
  },
  fileName: {
    flex: 1,
    overflow: 'hidden',
    textOverflow: 'ellipsis',
    whiteSpace: 'nowrap',
  },
  actionBtn: {
    background: 'none',
    border: 'none',
    cursor: 'pointer',
    fontSize: '12px',
    padding: '0 2px',
    lineHeight: 1,
    opacity: 0.6,
  },
  deleteBtn: {
    background: 'none',
    border: 'none',
    cursor: 'pointer',
    fontSize: '16px',
    color: 'var(--ifm-color-danger)',
    padding: '0 4px',
    lineHeight: 1,
  },
  dropZone: {
    margin: '8px',
    padding: '16px 12px',
    border: '2px dashed var(--ifm-color-emphasis-400)',
    borderRadius: '8px',
    display: 'flex',
    flexDirection: 'column',
    alignItems: 'center',
    gap: '4px',
    cursor: 'default',
    transition: 'border-color 0.15s, background-color 0.15s',
  },
  dropZoneActive: {
    borderColor: 'var(--ifm-color-primary)',
    backgroundColor: 'rgba(var(--ifm-color-primary-rgb, 37, 194, 160), 0.08)',
  },
  dropIcon: {
    fontSize: '24px',
  },
  dropText: {
    fontSize: '12px',
    fontWeight: 'bold',
    color: 'var(--ifm-font-color-base)',
  },
  dropHint: {
    fontSize: '10px',
    color: 'var(--ifm-color-emphasis-600)',
  },
};
