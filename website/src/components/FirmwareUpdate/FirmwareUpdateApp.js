import React, { useState } from "react";
import Layout from "@theme/Layout";
import "./mcumgr.css";

// Components
import PrebuiltFirmwares from "./components/PrebuiltFirmwares";
import FileSystemUpload from "./components/FileSystemUpload";
import ShellConsole from "./components/ShellConsole";

// Hooks
import { useMCUManager } from "./hooks/useMCUManager";
import { useFirmwareUpdate } from "./hooks/useFirmwareUpdate";
import UpdateStatus from "./components/UpdateStatus";
import { useFirmwareManagement } from "./hooks/useFirmwareManagement";

// Constants
import { SCREENS, useGithubConfig } from "./constants";

const TROUBLESHOOTING_ITEMS = [
  {
    title: "General issues",
    items: [
      "Press F12 and check console for errors",
      "Reload the page",
    ]
  },
  {
    title: "Connection issues",
    items: [
      "Forget device in OS Bluetooth settings",
      "Ensure watch is on",
      "Use supported browser"
    ]
  },
  {
    title: "Upload fails",
    items: [
      "Reset watch, reload page and reconnect",
      "Check file format (.zip or .bin)",
      "Try switching to USB serial mode."
    ]
  }
];

const FirmwareUpdateApp = () => {
  const githubConfig = useGithubConfig();

  const {
    mcumgr,
    deviceName,
    screen,
    images,
    deviceInfo,
    connectionStatus,
    connectionError,
    discovery,
    bluetoothAvailable,
    serialAvailable,
    transport,
    isSerialRecoveryMode,
    handleTransportChange,
    handleDeviceNameChange,
    handleConnect,
    handleDisconnect,
    refreshImages,
    preparingFirmware,
    registerShellListener,
  } = useMCUManager();

  const isConnected = (mcumgr?.isConnected?.() && (screen === SCREENS.CONNECTED)) ?? false;

  const update = useFirmwareUpdate(mcumgr, isSerialRecoveryMode, isConnected);
  const firmwareManager = useFirmwareManagement(githubConfig, update.loadFile);
  const isBusy = update.isBusy || firmwareManager.isDownloading || preparingFirmware;
  const {
    selection, progress, fileInputRef, fileFsInputRef, handleFileChange,
    handleFileUpload, handleFileSystemSelection, startFileSystemUpload,
  } = update;
  const fileInfos = selection?.images || [];
  const fileStatus = update.status;
  const isFileUploadInProgress = update.activity === "updating";
  const fileUploadPercentage = progress?.stage.startsWith("Uploading ") && progress.stage !== "Uploading filesystem" ? progress.percentage : null;
  const fileUploadSpeed = fileUploadPercentage !== null && progress?.speed ? (progress.speed / 1024).toFixed(2) : null;
  const fileSystemUploadProgress = progress?.stage === "Uploading filesystem" && update.isBusy ? progress.percentage : null;
  const fileSystemUploadSpeed = fileSystemUploadProgress !== null && progress?.speed ? (progress.speed / 1024).toFixed(2) : null;
  const fileSystemUploadStatus = selection?.filesystem ? `Selected: ${selection.filesystem.name}` : "Select a filesystem file";
  const [isShellConsoleModalOpen, setShellConsoleModalOpen] = useState(false);

  const handleEchoTest = () => {
    const message = prompt("Enter a text message to send", "Hello World!");
    if (message && mcumgr) {
      mcumgr.smpEcho(message);
    }
  };

  const handleReset = () => {
    if (mcumgr) {
      mcumgr.cmdReset();
    }
  };

  const handleConfirmImage = () => {
    if (!mcumgr || !images || images.length === 0) {
      alert("No images available to confirm");
      return;
    }

    // Find images that are not yet confirmed
    const unconfirmedImages = images.filter(img => img.confirmed === false);

    // Confirm each image
    unconfirmedImages.forEach(img => {
      if (img.hash) {
        console.log('Confirming unconfirmed image with hash:', img.hash);
        mcumgr.cmdImageConfirm(img.hash);
      }
    });
  };

  const renderScreen = () => {
    // Always render the unified interface
    return renderUnifiedInterface();
  };

  const renderUnifiedInterface = () => {
    const isConnecting = screen === SCREENS.CONNECTING;
    const selectedTransportAvailable = transport === 'ble' ? bluetoothAvailable : serialAvailable;
    const transportLabel = transport === 'ble' ? 'BLE' : 'USB Serial';

    return (
      <div className="grid grid-cols-1 lg:grid-cols-8 xl:grid-cols-11 gap-4">
        {/* Left Column - Sidebar with Connection, Browser Support & Troubleshooting */}
        <fieldset disabled={isBusy} className="order-1 lg:col-span-2 xl:col-span-3 space-y-3 min-w-0 border-0 p-0 m-0">
          {/* Connection Status & Controls */}
          <div>
            <h2 className="text-base font-semibold text-gray-900 dark:text-white mb-2">Connection</h2>
            <div className="border-b border-gray-200 dark:border-white/20 mb-2"></div>

            <div className="bg-white dark:bg-white/5 backdrop-blur-sm rounded-lg p-3 shadow-sm">
              <div className="flex items-center justify-between mb-2">
                <div className={`px-2 py-1 rounded-full text-xs font-medium ${
                  isConnected ? 'bg-green-100 dark:bg-green-900/30 text-green-800 dark:text-green-300' :
                  isConnecting ? 'bg-yellow-100 dark:bg-yellow-900/30 text-yellow-800 dark:text-yellow-300 animate-pulse' :
                  'bg-gray-100 dark:bg-gray-700 text-gray-800 dark:text-gray-300'
                }`}>
                  {isConnected ? `Connected (${transportLabel})` : isConnecting ? 'Connecting...' : 'Disconnected'}
                </div>
              </div>
              {isConnected && <div className="text-xs text-gray-600 dark:text-gray-300 mb-2 space-y-1">
                <p className="m-0 break-all">Hardware: {deviceInfo.hardwareVersion || "Not reported — select hardware manually"}</p>
                {deviceInfo.firmwareVersion && <p className="m-0">Firmware: {deviceInfo.firmwareVersion}</p>}
                {Number.isFinite(deviceInfo.batteryLevel) && <p className="m-0">Battery: {deviceInfo.batteryLevel}%</p>}
              </div>}
              {isConnected && deviceInfo.batteryLevel < 20 && <p role="alert" className="text-sm p-3 rounded bg-amber-100 dark:bg-amber-900/30 text-amber-900 dark:text-amber-200">Battery low ({deviceInfo.batteryLevel}%). Connect the watch to power before updating.</p>}
              {isConnecting && connectionStatus && <p role="status" className="text-xs">{connectionStatus}</p>}
              {connectionError && <p role="alert" className="text-sm text-red-600 dark:text-red-400">{connectionError}</p>}
              <div className="space-y-3">
                <div>
                  <label className="block text-sm font-medium text-gray-700 dark:text-gray-300 mb-2">Connection method</label>
                  <div className="flex gap-2">
                    <button
                      type="button"
                      disabled={isConnected || isConnecting}
                      onClick={() => handleTransportChange('ble')}
                      className={`flex-1 px-3 py-2 text-sm rounded-lg border transition-all disabled:opacity-50 disabled:cursor-not-allowed ${
                        transport === 'ble'
                          ? 'bg-zswatch-primary text-black border-zswatch-primary'
                          : 'bg-gray-100 dark:bg-white/10 text-gray-700 dark:text-gray-200 border-gray-300 dark:border-white/10 hover:bg-gray-200 dark:hover:bg-white/20'
                      }`}
                    >
                      Bluetooth
                    </button>
                    <button
                      type="button"
                      disabled={isConnected || isConnecting}
                      onClick={() => handleTransportChange('serial')}
                      className={`flex-1 px-3 py-2 text-sm rounded-lg border transition-all disabled:opacity-50 disabled:cursor-not-allowed ${
                        transport === 'serial'
                          ? 'bg-zswatch-primary text-black border-zswatch-primary'
                          : 'bg-gray-100 dark:bg-white/10 text-gray-700 dark:text-gray-200 border-gray-300 dark:border-white/10 hover:bg-gray-200 dark:hover:bg-white/20'
                      }`}
                    >
                      USB Serial
                    </button>
                  </div>
                  <div className="mt-2 flex flex-wrap gap-3 text-xs text-gray-500 dark:text-gray-400">
                    <span className={bluetoothAvailable ? 'text-green-600 dark:text-green-300' : 'text-red-500 dark:text-red-400'}>
                      BLE: {bluetoothAvailable ? 'Available' : 'Unavailable'}
                    </span>
                    <span className={serialAvailable ? 'text-green-600 dark:text-green-300' : 'text-red-500 dark:text-red-400'}>
                      USB Serial: {serialAvailable ? 'Available' : 'Unavailable'}
                    </span>
                  </div>
                </div>

                {!selectedTransportAvailable && (
                  <div className="p-3 rounded-lg border bg-red-50 dark:bg-red-900/20 border-red-200 dark:border-red-800">
                    <div className="flex items-center">
                      <span className="mr-2">❌</span>
                      <span className="text-sm font-medium text-red-800 dark:text-red-300">
                        {transport === 'ble' ? 'Bluetooth (BLE)' : 'USB Serial (WebSerial)'} Unavailable
                      </span>
                    </div>
                    <p className="mt-2 mb-0 text-xs leading-relaxed text-red-700 dark:text-red-300">
                      {transport === 'ble'
                        ? 'Requires a browser with Web Bluetooth support (Chrome, Edge, Opera).'
                        : 'Requires Web Serial support (Chrome or Edge).'}
                    </p>
                  </div>
                )}

                <details className="p-3 rounded-lg bg-blue-50 dark:bg-blue-900/20 border border-blue-200 dark:border-blue-800">
                  <summary className="cursor-pointer text-sm font-semibold">Connection setup & recovery</summary>
                  <p className="mt-2 mb-0 text-xs leading-relaxed text-blue-700 dark:text-blue-200">
                    For your first Bluetooth connection, open <b>Settings → Bluetooth</b> on the watch, enable <b>Pairable</b>, and accept pairing on your computer. Bluetooth updates are enabled automatically when connecting. For USB, go to ZSWatch <b>System {`->`} Update App</b> and enable USB.
                    If automatic Bluetooth setup fails on older firmware, enable Bluetooth there and reconnect.
                    <b> USB is significantly faster than Bluetooth</b> due to Web Bluetooth API limitations.
                    <br />
                    <br />
                    <b>Serial Recovery:</b> If ZSWatch is not responding enter MCUBoot by holding down two right buttons for 25 seconds.
                  </p>
                </details>

                <div>
                  <label className="block text-sm font-medium text-gray-700 dark:text-gray-300 mb-2">Device name</label>
                  <input
                    type="text"
                    value={deviceName}
                    onChange={(e) => handleDeviceNameChange(e.target.value)}
                    disabled={transport !== 'ble' || isConnected || !bluetoothAvailable}
                    className="w-full bg-white dark:bg-black/30 border border-gray-300 dark:border-white/20 rounded-lg px-3 py-2 text-gray-900 dark:text-white placeholder-gray-500 dark:placeholder-gray-400 focus:border-zswatch-primary focus:outline-none focus:ring-2 focus:ring-zswatch-primary/50 transition-all disabled:opacity-50 disabled:cursor-not-allowed text-sm"
                    placeholder="ZSWatch"
                  />
                  <p className="text-xs text-gray-500 dark:text-gray-400 mt-1">
                    {transport === 'ble'
                      ? 'Optional prefix filter. Leave empty to scan for any compatible device.'
                      : 'Not required when connecting over USB.'}
                  </p>
                </div>

                <div className="flex flex-col gap-2">
                  {!isConnected ? (
                    <button
                      onClick={() => handleConnect()}
                      disabled={!selectedTransportAvailable || isConnecting}
                      className="px-4 py-2 bg-zswatch-primary text-black rounded-lg font-medium hover:bg-zswatch-primary/90 transition-all disabled:opacity-50 disabled:cursor-not-allowed"
                    >
                      {isConnecting ? 'Connecting...' : transport === 'ble' ? 'Connect via BLE' : 'Connect via Serial'}
                    </button>
                  ) : (
                    <button
                      onClick={handleDisconnect}
                      className="flex items-center justify-center gap-2 px-4 py-2 bg-red-100 dark:bg-red-500/20 text-red-700 dark:text-red-400 rounded-lg border border-red-300 dark:border-red-500/30 hover:bg-red-200 dark:hover:bg-red-500/30 transition-all"
                    >
                      <span className="text-sm">🔌</span>
                      Disconnect
                    </button>
                  )}

                  {transport === 'ble' && !isConnected && !isConnecting && discovery.savedName && <div className="text-xs text-gray-500 dark:text-gray-400">
                    <p className="m-0">Saved watch: {discovery.savedName}{discovery.recentlySeen ? ' · recently detected' : ''}</p>
                    <button type="button" disabled={!bluetoothAvailable} onClick={() => handleConnect({ chooseDevice: true })} className="underline mt-1 disabled:opacity-50">Choose another watch</button>
                  </div>}

                  {isConnected && (
                    <div className="flex gap-2">
                      <button
                        onClick={handleEchoTest}
                        className="flex-1 flex items-center justify-center gap-1 px-3 py-2 bg-gray-100 dark:bg-white/10 text-gray-700 dark:text-white rounded-lg border border-gray-300 dark:border-white/20 hover:bg-gray-200 dark:hover:bg-white/20 transition-all text-sm"
                      >
                        <span className="text-xs">📢</span>
                        Echo
                      </button>

                      <button
                        onClick={handleReset}
                        className="flex-1 flex items-center justify-center gap-1 px-3 py-2 bg-gray-100 dark:bg-white/10 text-gray-700 dark:text-white rounded-lg border border-gray-300 dark:border-white/20 hover:bg-gray-200 dark:hover:bg-white/20 transition-all text-sm"
                      >
                        <span className="text-xs">🔄</span>
                        Reset
                      </button>
                    </div>
                  )}
                </div>
              </div>
            </div>
          </div>

          {/* Browser Support Info */}
          <details>
            <summary className="cursor-pointer text-sm font-semibold">Browser support</summary>

            <div className="bg-white dark:bg-white/5 backdrop-blur-sm rounded-lg p-3 shadow-sm">
              <ul className="space-y-2 text-sm text-gray-600 dark:text-gray-300" style={{ paddingLeft: 0, paddingInlineStart: 0, margin: 0 }}>
                <li className="flex items-center">
                  <span className="w-2 h-2 bg-green-500 rounded-full mr-3"></span>
                  Chrome, Edge, Opera (Desktop)
                </li>
                <li className="flex items-center">
                  <span className="w-2 h-2 bg-green-500 rounded-full mr-3"></span>
                  Chrome, Edge (WebSerial / USB serial updates)
                </li>
                <li className="flex items-center">
                  <span className="w-2 h-2 bg-green-500 rounded-full mr-3"></span>
                  Chrome (Android)
                </li>
                <li className="flex items-center">
                  <span className="w-2 h-2 bg-yellow-500 rounded-full mr-3"></span>
                  Bluefy (iOS/iPadOS)
                </li>
              </ul>
            </div>
          </details>

          {/* Troubleshooting */}
          <details>
            <summary className="cursor-pointer text-sm font-semibold">Troubleshooting</summary>

            <div className="bg-white dark:bg-white/5 backdrop-blur-sm rounded-lg p-3 shadow-sm">
              <div className="space-y-3">
                {TROUBLESHOOTING_ITEMS.map((section, index) => (
                  <details key={index} className="group">
                    <summary className="flex items-center justify-between cursor-pointer text-gray-900 dark:text-white hover:text-zswatch-primary transition-colors text-sm">
                      <span>{section.title}</span>
                      <span className="group-open:rotate-180 transition-transform">▼</span>
                    </summary>
                    <div className="mt-3 space-y-2 text-xs text-gray-600 dark:text-gray-300 pl-4 border-l-2 border-zswatch-primary/30">
                      {section.items.map((item, itemIndex) => (
                        <p key={itemIndex} className="m-0">• {item}</p>
                      ))}
                    </div>
                  </details>
                ))}
              </div>
            </div>
          </details>
        </fieldset>

        {/* Prebuilt Firmware & Images */}
        <div className="order-2 lg:col-span-3 xl:col-span-4 space-y-3 min-w-0">
          {/* Prebuilt Firmware */}
          <div>
            <h2 className="text-base font-semibold text-gray-900 dark:text-white mb-2">Prebuilt Firmware</h2>
            <div className="border-b border-gray-200 dark:border-white/20 mb-2"></div>

            <div className="bg-white dark:bg-white/5 backdrop-blur-sm rounded-lg p-3 shadow-sm">
              <PrebuiltFirmwares manager={firmwareManager} githubConfig={githubConfig} disabled={isBusy} hardwareVersion={deviceInfo.hardwareVersion} />
            </div>
          </div>

          {/* Shell Console */}
          <details>
            <summary className="cursor-pointer text-sm font-semibold">Shell console</summary>
            <div className="flex items-center justify-between mb-2">
              <h2 className="text-base font-semibold text-gray-900 dark:text-white">Shell Console</h2>
              <button
                type="button"
                onClick={() => setShellConsoleModalOpen(true)}
                className="px-3 py-2 text-xs font-medium rounded-md border border-gray-300 dark:border-white/20 text-gray-700 dark:text-gray-200 bg-white dark:bg-white/10 hover:bg-gray-100 dark:hover:bg-white/20 transition-all"
              >
                Open in Modal
              </button>
            </div>
            <div className="border-b border-gray-200 dark:border-white/20 mb-2"></div>

            <ShellConsole
              mcumgr={mcumgr}
              registerShellListener={registerShellListener}
              isConnected={isConnected && !isBusy}
            />
          </details>

          {/* Images Status */}
          <div>
            <h2 className="text-base font-semibold text-gray-900 dark:text-white mb-1">Images</h2>
            <div className="border-b border-gray-200 dark:border-white/20 mb-2"></div>

            <div className="bg-white dark:bg-white/5 backdrop-blur-sm rounded-lg p-3 shadow-sm">
              {!isConnected ? (
                <div className="text-center py-2">
                  <p className="text-gray-600 dark:text-gray-300 text-xs m-0">Connect to view installed images</p>
                </div>
              ) : images && images.length > 0 ? (
                <div className="space-y-0">
                  {images.map((image, index) => (
                    <div key={index} className={`border rounded p-2 ${
                      image.active
                        ? 'bg-green-50 dark:bg-green-900/20 border-green-200 dark:border-green-500/30'
                        : 'border-gray-200 dark:border-white/10'
                    }`}>
                      <div className="flex items-start justify-between">
                        <div className="flex-1">
                          <h4 className="font-medium text-gray-900 dark:text-white text-sm m-0 mb-1">
                            Image #{image.image} - Slot {image.slot}
                          </h4>
                          <div className="grid grid-cols-2 gap-x-4 gap-y-0 text-xs text-gray-500 dark:text-gray-400">
                            <div>Version: {image.version || 'Unknown'}</div>
                            <div>{image.bootable ? 'Bootable' : 'Not Bootable'}</div>
                            <div>Confirmed: {image.confirmed ? 'Yes' : 'No'}</div>
                            <div>Pending: {image.pending ? 'Yes' : 'No'}</div>
                          </div>
                          {image.hash && (
                            <div className="text-xs text-gray-500 dark:text-gray-400 font-mono mt-1">
                              Hash: {Array.from(image.hash).map(byte => byte.toString(16).padStart(2, '0')).join('').substring(0, 16)}...
                            </div>
                          )}
                        </div>
                        <div className="flex flex-wrap gap-1 ml-2">
                          {image.active && (
                            <span className="text-xs bg-green-100 dark:bg-green-500/20 text-green-700 dark:text-green-400 px-2 py-0.5 rounded">
                              ✅ Active
                            </span>
                          )}
                          {image.pending && (
                            <span className="text-xs bg-yellow-100 dark:bg-yellow-500/20 text-yellow-700 dark:text-yellow-400 px-2 py-0.5 rounded">
                              ⏳ Pending
                            </span>
                          )}
                          {image.confirmed && (
                            <span className="text-xs bg-blue-100 dark:bg-blue-500/20 text-blue-700 dark:text-blue-400 px-2 py-0.5 rounded">
                              ✓ Confirmed
                            </span>
                          )}
                          {image.permanent && (
                            <span className="text-xs bg-purple-100 dark:bg-purple-500/20 text-purple-700 dark:text-purple-400 px-2 py-0.5 rounded">
                              📌 Permanent
                            </span>
                          )}
                        </div>
                      </div>
                    </div>
                  ))}

                  <div className="flex gap-2 pt-3 border-t border-gray-200 dark:border-white/10">
                    <button
                      disabled={isBusy}
                      onClick={() => !isBusy && refreshImages()}
                      className="flex-1 bg-gray-100 dark:bg-white/10 text-gray-700 dark:text-white px-3 py-2 rounded text-sm hover:bg-gray-200 dark:hover:bg-white/20 transition-all"
                    >
                      Refresh
                    </button>
                    {!isSerialRecoveryMode && (
                      <>
                        <button
                          disabled={isBusy}
                          onClick={() => !isBusy && mcumgr.cmdImageTest()}
                          className="flex-1 bg-blue-100 dark:bg-blue-900/30 text-blue-700 dark:text-blue-300 px-3 py-2 rounded text-sm hover:bg-blue-200 dark:hover:bg-blue-900/50 transition-all"
                        >
                          Test
                        </button>
                        <button
                          disabled={isBusy}
                          onClick={handleConfirmImage}
                          className="flex-1 bg-green-100 dark:bg-green-900/30 text-green-700 dark:text-green-300 px-3 py-2 rounded text-sm hover:bg-green-200 dark:hover:bg-green-900/50 transition-all"
                        >
                          Confirm
                        </button>
                        <button
                          disabled={isBusy}
                          onClick={() => !isBusy && mcumgr.cmdImageErase()}
                          className="flex-1 bg-red-100 dark:bg-red-900/30 text-red-700 dark:text-red-300 px-3 py-2 rounded text-sm hover:bg-red-200 dark:hover:bg-red-900/50 transition-all"
                        >
                          Erase
                        </button>
                      </>
                    )}
                  </div>

                  {!isSerialRecoveryMode ? (
                    <div className="text-xs text-gray-500 dark:text-gray-400 mt-2 flex items-center">
                      <span className="mr-1">ℹ️</span>
                      Use Test/Confirm/Erase to manage firmware images
                    </div>
                  ) : (
                    <div className="text-xs text-gray-500 dark:text-gray-400 mt-2 flex items-center">
                      <span className="mr-1">ℹ️</span>
                      Updates flash directly in MCUBoot mode and restart the watch automatically.
                    </div>
                  )}
                </div>
              ) : (
                <div className="text-center py-2">
                  <div className="text-gray-400 text-2xl mb-2">📱</div>
                  <p className="text-gray-600 dark:text-gray-300 text-sm">No images found on device</p>
                </div>
              )}
            </div>
          </div>
        </div>
        {/* Manual Upload & File System */}
        <div className="order-3 lg:col-span-3 xl:col-span-4 space-y-3 min-w-0">
          {/* Manual Firmware Upload */}
          <div>
            <h2 className="text-base font-semibold text-gray-900 dark:text-white mb-2">Firmware Package</h2>
            <div className="border-b border-gray-200 dark:border-white/20 mb-2"></div>

            <div className="bg-white dark:bg-white/5 backdrop-blur-sm rounded-lg p-3 shadow-sm">
              <UpdateStatus update={update} isConnected={isConnected} isSerialRecoveryMode={isSerialRecoveryMode} disabled={isBusy} />

            </div>
          </div>

          {/* File System Upload */}
          {update.advanced && <details className="text-sm">
            <summary className="text-xs text-gray-600 dark:text-gray-400 cursor-pointer">Manual resource upload</summary>
            <div className="border-b border-gray-200 dark:border-white/20 mb-2"></div>

            <div className="bg-white dark:bg-white/5 backdrop-blur-sm rounded-lg p-3 shadow-sm">
              <fieldset disabled={isBusy} className="min-w-0 border-0 p-0 m-0">
              <FileSystemUpload
                hasFile={!!selection?.filesystem}
                fileFsInputRef={fileFsInputRef}
                fileSystemUploadStatus={fileSystemUploadStatus}
                fileSystemUploadProgress={fileSystemUploadProgress}
                fileSystemUploadSpeed={fileSystemUploadSpeed}
                onFileSystemSelection={handleFileSystemSelection}
                onFileSystemUploadStart={startFileSystemUpload}
                isConnected={isConnected && !isBusy}
                isSerialRecoveryMode={isSerialRecoveryMode}
              />
              </fieldset>
            </div>
          </details>}
        </div>

      </div>
    );
  };

  return (
    <Layout>
      <div className="min-h-screen zswatch-background-gradient">
        <div className="container mx-auto px-2 py-4">
          {/* Header */}
          <div className="text-center mb-4">
            <h1 className="text-2xl font-bold mb-2 text-gray-900 dark:text-gray-100">
              ZSWatch Firmware Update
            </h1>
            <p className="text-sm text-gray-600 dark:text-gray-300 mb-2">
              Update your ZSWatch firmware and image resources over USB or Bluetooth
            </p>
            <details className="inline-block text-left max-w-2xl mx-auto">
              <summary className="cursor-pointer text-sm font-medium text-zswatch-primary hover:underline">
                How does this work?
              </summary>
              <div className="mt-3 p-4 rounded-lg bg-white dark:bg-white/5 border border-gray-200 dark:border-white/10 text-sm text-gray-700 dark:text-gray-300 space-y-2">
                <p className="m-0"><strong>Step 1:</strong> For USB, open <strong>Apps &rarr; Update</strong> on the watch and enable <strong>USB</strong>. For your first Bluetooth connection, enable <strong>Pairable</strong> in <strong>Settings &rarr; Bluetooth</strong> and accept pairing on your computer. Bluetooth updates are enabled automatically when connecting.</p>
                <p className="m-0"><strong>Step 2:</strong> Connect to the watch using the <strong>Connection</strong> panel on the left. USB is much faster than BLE.</p>
                <p className="m-0"><strong>Step 3 &ndash; Firmware:</strong> Choose a release or CI build in <strong>Prebuilt Firmware</strong>. Bluetooth automatically filters builds using the watch's reported hardware; choose it manually if unavailable. Select the entire downloaded ZIP in <strong>Firmware Package</strong>; it is unpacked automatically.</p>
                <p className="m-0"><strong>Step 4 &ndash; Image Resources:</strong> Use <strong>Update watch</strong> to upload image resources and firmware together. You can also upload firmware or <code>lvgl_resources_raw.bin</code> separately.</p>
                <p className="m-0">Updates restart the watch automatically. Bluetooth reconnects and verifies the installed firmware; over USB, reconnect to check the installed images.</p>
                <p className="m-0 text-xs text-gray-500 dark:text-gray-400">Firmware downloads are available from <a href="https://github.com/ZSWatch/ZSWatch/releases" target="_blank" rel="noopener noreferrer" className="underline">GitHub Releases</a> and <a href="https://github.com/ZSWatch/ZSWatch/actions" target="_blank" rel="noopener noreferrer" className="underline">GitHub Actions</a>.</p>
              </div>
            </details>
          </div>

          {/* Main Content */}
          <div className="px-2 pb-8 max-w-7xl mx-auto">
            {renderScreen()}
          </div>

          {/* Information Cards - moved to sidebar in connected state */}
        </div>
      </div>

      {isShellConsoleModalOpen && (
        <div
          className="fixed inset-0 z-50 flex items-center justify-center p-4"
          onClick={() => setShellConsoleModalOpen(false)}
        >
          <div className="absolute inset-0 bg-black/60"></div>
          <div
            className="relative z-10 w-full max-w-5xl"
            onClick={event => event.stopPropagation()}
          >
            <div className="bg-white dark:bg-gray-900 rounded-lg shadow-2xl overflow-hidden">
              <div className="flex items-center justify-between px-4 py-3 border-b border-gray-200 dark:border-white/10">
                <h3 className="text-lg font-semibold text-gray-900 dark:text-white m-0">Shell Console</h3>
                <button
                  type="button"
                  onClick={() => setShellConsoleModalOpen(false)}
                  className="text-gray-500 hover:text-gray-800 dark:text-gray-300 dark:hover:text-white transition"
                >
                  ✕
                </button>
              </div>
              <div className="p-4">
                <ShellConsole
                  mcumgr={mcumgr}
                  registerShellListener={registerShellListener}
                  isConnected={isConnected && !isBusy}
                  className="shadow-none p-0 bg-transparent"
                  textareaClassName="h-[28rem] text-sm"
                />
              </div>
            </div>
          </div>
        </div>
      )}

    </Layout>
  );
};

export default FirmwareUpdateApp;
