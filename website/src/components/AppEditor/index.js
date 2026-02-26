// Copyright (c) 2025 ZSWatch Project
// SPDX-License-Identifier: Apache-2.0

/**
 * ZSWatch LLEXT App Editor — main component.
 *
 * Full-page editor with:
 * - Left sidebar: file tree
 * - Center: Monaco code editor
 * - Top: build toolbar
 * - Bottom: build output panel
 */

import React, { useState, useEffect, useCallback, useRef } from 'react';
import Layout from '@theme/Layout';
import BrowserOnly from '@docusaurus/BrowserOnly';
import FileTree from './FileTree';
import BuildToolbar from './BuildToolbar';
import OutputPanel from './OutputPanel';
import InstallModal from './InstallModal';
import { compileApp, convertImage, checkHealth } from './api';
import { connect as bleConnect, disconnect as bleDisconnect,
         isConnected as bleIsConnected, getDeviceName as bleDeviceName,
         installApp as bleInstall, removeApp as bleRemove,
         rebootWatch as bleReboot,
         listInstalledApps as bleListApps,
         onDisconnect as bleOnDisconnect } from './ble';
import { DEFAULT_FILES } from './defaultTemplate';

// ── Installed-apps persistence ─────────────────────────────────────────────────

const INSTALLED_APPS_KEY = 'zswatch-installed-apps';

function loadInstalledApps() {
  if (typeof window === 'undefined') return [];
  try {
    return JSON.parse(localStorage.getItem(INSTALLED_APPS_KEY)) || [];
  } catch { return []; }
}

function saveInstalledApps(apps) {
  if (typeof window !== 'undefined') {
    localStorage.setItem(INSTALLED_APPS_KEY, JSON.stringify(apps));
  }
}

function AppEditorContent() {
  // File state
  const [files, setFiles] = useState(() => {
    // Try to restore from localStorage
    if (typeof window !== 'undefined') {
      const saved = localStorage.getItem('zswatch-app-editor-files');
      if (saved) {
        try {
          return JSON.parse(saved);
        } catch { /* ignore */ }
      }
    }
    return { ...DEFAULT_FILES };
  });
  const [activeFile, setActiveFile] = useState(() => {
    return Object.keys(files)[0] || 'my_app.c';
  });

  // Build state — editable app name persisted to localStorage
  const [appName, setAppName] = useState(() => {
    if (typeof window !== 'undefined') {
      return localStorage.getItem('zswatch-app-name') || 'my_app';
    }
    return 'my_app';
  });
  const [building, setBuilding] = useState(false);
  const [buildErrors, setBuildErrors] = useState([]);
  const [buildOutput, setBuildOutput] = useState('');
  const [llextBlob, setLlextBlob] = useState(null);
  const [serverOnline, setServerOnline] = useState(false);

  // Monaco editor ref
  const editorRef = useRef(null);
  const monacoRef = useRef(null);

  // BLE / install state
  const [bleConnected, setBleConnected] = useState(false);
  const [bleConnecting, setBleConnecting] = useState(false);
  const [bleDevice, setBleDevice] = useState(null);
  const [installModalOpen, setInstallModalOpen] = useState(false);
  const [installing, setInstalling] = useState(false);
  const [installStatus, setInstallStatus] = useState('');
  const [installProgress, setInstallProgress] = useState(0);

  // Installed apps tracking
  const [installedApps, setInstalledApps] = useState(loadInstalledApps);
  const [loadingApps, setLoadingApps] = useState(false);

  // Persist files to localStorage
  useEffect(() => {
    if (typeof window !== 'undefined') {
      localStorage.setItem('zswatch-app-editor-files', JSON.stringify(files));
    }
  }, [files]);

  // Persist app name
  useEffect(() => {
    if (typeof window !== 'undefined') {
      localStorage.setItem('zswatch-app-name', appName);
    }
  }, [appName]);

  // Health check on mount
  useEffect(() => {
    checkHealth().then(setServerOnline);
    const interval = setInterval(() => {
      checkHealth().then(setServerOnline);
    }, 10000);
    return () => clearInterval(interval);
  }, []);

  // BLE disconnect listener
  useEffect(() => {
    const unsub = bleOnDisconnect(() => {
      setBleConnected(false);
      setBleConnecting(false);
      setBleDevice(null);
    });
    return unsub;
  }, []);

  // Handle BLE connect
  const handleBleConnect = useCallback(async () => {
    setBleConnecting(true);
    setInstallStatus('');
    try {
      await bleConnect();
      setBleConnected(true);
      setBleDevice(bleDeviceName());
    } catch (err) {
      setInstallStatus(`Connection failed: ${err.message}`);
    } finally {
      setBleConnecting(false);
    }
  }, []);

  // Handle BLE disconnect
  const handleBleDisconnect = useCallback(async () => {
    await bleDisconnect();
    setBleConnected(false);
    setBleDevice(null);
  }, []);

  // Handle install
  const handleInstall = useCallback(async () => {
    if (!llextBlob) return;
    setInstalling(true);
    setInstallStatus('');
    setInstallProgress(0);
    try {
      await bleInstall(appName, llextBlob, setInstallStatus, setInstallProgress);
      // Track installed app
      setInstalledApps((prev) => {
        const appId = appName.endsWith('_ext') ? appName : `${appName}_ext`;
        const updated = prev.filter((a) => a.id !== appId);
        updated.push({ id: appId, name: appName, installedAt: new Date().toISOString() });
        saveInstalledApps(updated);
        return updated;
      });
    } catch (err) {
      // installApp already calls onStatus with error message,
      // but set it again as fallback in case it wasn't set.
      if (!installStatus?.startsWith('Install failed')) {
        setInstallStatus(`Install failed: ${err.message}`);
      }
    } finally {
      setInstalling(false);
    }
  }, [llextBlob, appName, installStatus]);

  // Update editor content when active file changes
  useEffect(() => {
    if (editorRef.current && files[activeFile] !== undefined) {
      const model = editorRef.current.getModel();
      if (model && model.getValue() !== files[activeFile]) {
        model.setValue(files[activeFile]);
      }
    }
  }, [activeFile]);

  // Handle editor content changes
  const handleEditorChange = useCallback(
    (value) => {
      setFiles((prev) => ({ ...prev, [activeFile]: value }));
    },
    [activeFile]
  );

  // Handle editor mount
  const handleEditorDidMount = useCallback((editor, monaco) => {
    editorRef.current = editor;
    monacoRef.current = monaco;

    // Capture Ctrl+S / Cmd+S to prevent browser "Save page" dialog
    editor.addAction({
      id: 'zswatch-save-noop',
      label: 'Save (no-op)',
      keybindings: [
        monaco.KeyMod.CtrlCmd | monaco.KeyCode.KeyS,
      ],
      run: () => {
        // Files are auto-saved to localStorage — nothing extra needed
      },
    });
  }, []);

  // Create a new file
  const handleCreateFile = useCallback((name) => {
    if (files[name]) {
      alert(`File ${name} already exists.`);
      return;
    }
    const header = name.endsWith('.h')
      ? `#pragma once\n\n/* ${name} */\n`
      : `/* ${name} */\n\n#include <lvgl.h>\n`;
    setFiles((prev) => ({ ...prev, [name]: header }));
    setActiveFile(name);
  }, [files]);

  // Delete a file
  const handleDeleteFile = useCallback((name) => {
    setFiles((prev) => {
      const next = { ...prev };
      delete next[name];
      return next;
    });
    if (activeFile === name) {
      const remaining = Object.keys(files).filter((n) => n !== name);
      setActiveFile(remaining[0] || 'my_app.c');
    }
  }, [activeFile, files]);

  // Rename a file
  const handleRenameFile = useCallback((oldName, newName) => {
    if (!newName || oldName === newName) return;
    if (files[newName]) {
      alert(`File ${newName} already exists.`);
      return;
    }
    setFiles((prev) => {
      const next = {};
      for (const [k, v] of Object.entries(prev)) {
        if (k === oldName) {
          next[newName] = v;
        } else {
          next[k] = v;
        }
      }
      return next;
    });
    if (activeFile === oldName) {
      setActiveFile(newName);
    }
  }, [activeFile, files]);

  // Fetch installed apps from the watch over BLE
  const handleRefreshApps = useCallback(async () => {
    if (!bleIsConnected()) return;
    setLoadingApps(true);
    try {
      // Pass locally-known app IDs so the scan also probes user-installed apps
      const extraIds = installedApps.map((a) => a.id);
      const apps = await bleListApps(setInstallStatus, extraIds);
      setInstalledApps(apps.map((a) => ({
        id: a.id, name: a.name, size: a.size,
        installedAt: new Date().toISOString(),
      })));
      saveInstalledApps(apps);
    } catch (err) {
      setInstallStatus(`Failed to list apps: ${err.message}`);
    } finally {
      setLoadingApps(false);
    }
  }, [installedApps]);

  // Handle remove app
  const handleRemoveApp = useCallback(async (appId) => {
    if (!bleConnected) {
      setInstallStatus('Connect to watch first to remove apps.');
      return;
    }
    try {
      await bleRemove(appId);
      setInstalledApps((prev) => {
        const updated = prev.filter((a) => a.id !== appId);
        saveInstalledApps(updated);
        return updated;
      });
      setInstallStatus(`Removed ${appId}. Reboot the watch to fully unload.`);
    } catch (err) {
      setInstallStatus(`Remove failed: ${err.message}`);
    }
  }, [bleConnected]);

  // Reboot the watch
  const handleReboot = useCallback(async () => {
    if (!bleConnected) return;
    try {
      await bleReboot();
      setInstallStatus('Reboot command sent — watch is restarting…');
      setBleConnected(false);
      setBleDevice(null);
    } catch (err) {
      setInstallStatus(`Reboot failed: ${err.message}`);
    }
  }, [bleConnected]);

  // Upload and convert an image
  const handleUploadImage = useCallback(async (imageFile) => {
    const varName = imageFile.name
      .replace(/\.[^.]+$/, '')
      .replace(/[^a-zA-Z0-9_]/g, '_');

    setBuildOutput(`Converting ${imageFile.name}...`);

    const result = await convertImage(imageFile, 'RGB565A8', varName, null, null);

    if (result.success) {
      const fileName = `${varName}.c`;
      setFiles((prev) => ({ ...prev, [fileName]: result.cSource }));
      setActiveFile(fileName);
      setBuildOutput(`Image converted: ${fileName} (include it with #include "${fileName}")`);
      setBuildErrors([]);
    } else {
      setBuildOutput(`Image conversion failed: ${result.error}`);
      setBuildErrors([{
        severity: 'error',
        message: `Image conversion failed: ${result.error}`,
      }]);
    }
  }, []);

  // Build
  const handleBuild = useCallback(async () => {
    setBuilding(true);
    setBuildErrors([]);
    setBuildOutput('Compiling...');
    setLlextBlob(null);

    // Clear previous error markers
    if (monacoRef.current && editorRef.current) {
      const model = editorRef.current.getModel();
      if (model) {
        monacoRef.current.editor.setModelMarkers(model, 'build', []);
      }
    }

    const fileList = Object.entries(files).map(([name, content]) => ({
      name,
      content,
    }));

    try {
      const result = await compileApp(fileList, appName);

      if (result.success && result.blob) {
        setBuildOutput(`Build successful! (${result.blob.size} bytes)`);
        setBuildErrors([]);
        setLlextBlob(result.blob);
      } else {
        setBuildOutput(result.raw_output || 'Build failed.');
        setBuildErrors(result.errors || []);
        setLlextBlob(null);

        // Set error markers in Monaco
        if (monacoRef.current && editorRef.current && result.errors) {
          const markers = result.errors
            .filter((e) => e.file === activeFile && e.line)
            .map((e) => ({
              severity:
                e.severity === 'error'
                  ? monacoRef.current.MarkerSeverity.Error
                  : monacoRef.current.MarkerSeverity.Warning,
              startLineNumber: e.line,
              startColumn: e.column || 1,
              endLineNumber: e.line,
              endColumn: e.column ? e.column + 1 : 1000,
              message: e.message,
            }));

          const model = editorRef.current.getModel();
          if (model) {
            monacoRef.current.editor.setModelMarkers(model, 'build', markers);
          }
        }
      }
    } catch (err) {
      setBuildOutput(`Build error: ${err.message}`);
      setBuildErrors([{ severity: 'error', message: err.message }]);
    } finally {
      setBuilding(false);
    }
  }, [files, appName, activeFile]);

  // Download .llext
  const handleDownload = useCallback(() => {
    if (!llextBlob) return;
    const url = URL.createObjectURL(llextBlob);
    const a = document.createElement('a');
    a.href = url;
    a.download = `${appName}.llext`;
    a.click();
    URL.revokeObjectURL(url);
  }, [llextBlob, appName]);

  // Keyboard shortcut: Ctrl+B to build
  useEffect(() => {
    const handler = (e) => {
      if ((e.ctrlKey || e.metaKey) && e.key === 'b') {
        e.preventDefault();
        handleBuild();
      }
    };
    window.addEventListener('keydown', handler);
    return () => window.removeEventListener('keydown', handler);
  }, [handleBuild]);

  return (
    <div style={styles.wrapper}>
      <BuildToolbar
        appName={appName}
        onAppNameChange={(v) => setAppName(v.replace(/[^a-zA-Z0-9_]/g, '_'))}
        onBuild={handleBuild}
        onDownload={handleDownload}
        onInstall={() => { setInstallStatus(''); setInstallProgress(0); setInstallModalOpen(true); }}
        building={building}
        hasOutput={!!llextBlob}
        serverOnline={serverOnline}
        bleConnected={bleConnected}
        bleConnecting={bleConnecting}
        bleDevice={bleDevice}
        onBleConnect={handleBleConnect}
        onBleDisconnect={handleBleDisconnect}
      />

      <InstallModal
        open={installModalOpen}
        onClose={() => { if (!installing) setInstallModalOpen(false); }}
        connected={bleConnected}
        connecting={bleConnecting}
        deviceName={bleDevice}
        onConnect={handleBleConnect}
        onDisconnect={handleBleDisconnect}
        onInstall={handleInstall}
        installing={installing}
        installStatus={installStatus}
        installProgress={installProgress}
        hasBlob={!!llextBlob}
        installedApps={installedApps}
        onRemoveApp={handleRemoveApp}
        onRefreshApps={handleRefreshApps}
        onReboot={handleReboot}
        loadingApps={loadingApps}
      />

      <div style={styles.main}>
        <div style={styles.sidebar}>
          <FileTree
            files={files}
            activeFile={activeFile}
            onSelectFile={setActiveFile}
            onCreateFile={handleCreateFile}
            onDeleteFile={handleDeleteFile}
            onRenameFile={handleRenameFile}
            onUploadImage={handleUploadImage}
          />
        </div>

        <div style={styles.editorContainer}>
          <div
          style={styles.editorPane}
          onDragOver={(e) => {
            if ([...e.dataTransfer.types].includes('Files')) {
              e.preventDefault();
              e.currentTarget.style.outline = '2px dashed var(--ifm-color-primary)';
            }
          }}
          onDragLeave={(e) => {
            e.currentTarget.style.outline = 'none';
          }}
          onDrop={(e) => {
            e.preventDefault();
            e.currentTarget.style.outline = 'none';
            const file = e.dataTransfer.files[0];
            if (file && file.type.startsWith('image/')) {
              handleUploadImage(file);
            }
          }}
        >
            <MonacoWrapper
              value={files[activeFile] || ''}
              onChange={handleEditorChange}
              onMount={handleEditorDidMount}
            />
          </div>

          <div style={styles.outputPane}>
            <OutputPanel
              output={buildOutput}
              errors={buildErrors}
              building={building}
            />
          </div>
        </div>
      </div>
    </div>
  );
}

/**
 * Monaco editor wrapper — loaded client-side only.
 */
function MonacoWrapper({ value, onChange, onMount }) {
  const [MonacoEditor, setMonacoEditor] = useState(null);

  useEffect(() => {
    import('@monaco-editor/react').then((mod) => {
      setMonacoEditor(() => mod.default);
    });
  }, []);

  if (!MonacoEditor) {
    return (
      <div style={styles.editorLoading}>
        Loading editor...
      </div>
    );
  }

  return (
    <MonacoEditor
      height="100%"
      language="c"
      theme="vs-dark"
      value={value}
      onChange={onChange}
      onMount={onMount}
      options={{
        fontSize: 13,
        fontFamily: "'JetBrains Mono', 'Fira Code', 'Cascadia Code', monospace",
        minimap: { enabled: false },
        scrollBeyondLastLine: false,
        automaticLayout: true,
        tabSize: 4,
        insertSpaces: true,
        wordWrap: 'off',
        lineNumbers: 'on',
        renderWhitespace: 'selection',
        bracketPairColorization: { enabled: true },
        suggest: { showKeywords: true },
      }}
    />
  );
}

/**
 * Top-level page component with Layout wrapper.
 */
export default function AppEditorPage() {
  return (
    <Layout
      title="App Editor"
      description="Build LLEXT apps for ZSWatch in your browser"
      noFooter
    >
      <BrowserOnly fallback={<div style={{ padding: 40 }}>Loading editor...</div>}>
        {() => <AppEditorContent />}
      </BrowserOnly>
    </Layout>
  );
}

const styles = {
  wrapper: {
    display: 'flex',
    flexDirection: 'column',
    height: 'calc(100vh - 60px)',
    overflow: 'hidden',
  },
  main: {
    display: 'flex',
    flex: 1,
    overflow: 'hidden',
  },
  sidebar: {
    width: '220px',
    flexShrink: 0,
    overflow: 'hidden',
  },
  editorContainer: {
    flex: 1,
    display: 'flex',
    flexDirection: 'column',
    overflow: 'hidden',
  },
  editorPane: {
    flex: 1,
    minHeight: 0,
  },
  outputPane: {
    height: '200px',
    flexShrink: 0,
  },
  editorLoading: {
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'center',
    height: '100%',
    color: 'var(--ifm-color-emphasis-500)',
    fontSize: '14px',
  },
};
