import React, { useEffect } from "react";
import Layout from "@theme/Layout";
import "./mcumgr.css";

// Components
import PrebuiltFirmwares from "./components/PrebuiltFirmwares";
import FileUploadSection from "./components/FileUploadSection";
import FileSystemUpload from "./components/FileSystemUpload";

// Hooks
import { useMCUManager } from "./hooks/useMCUManager";
import { useFileUpload } from "./hooks/useFileUpload";
import { useFileSystemUpload } from "./hooks/useFileSystemUpload";
import { useFirmwareManagement } from "./hooks/useFirmwareManagement";

// Constants
import { SCREENS, useGithubConfig } from "./constants";

const FirmwareUpdateApp = () => {
  const githubConfig = useGithubConfig();
  
  const {
    mcumgr,
    deviceName,
    screen,
    images,
    bluetoothAvailable,
    serialAvailable,
    transport,
    handleTransportChange,
    handleDeviceNameChange,
    handleConnect,
    handleDisconnect,
  } = useMCUManager();

  const isConnected = mcumgr?.isConnected?.() ?? false;

  const {
    fileUploadPercentage,
    fileUploadSpeed,
    isFileUploadInProgress,
    fileInfos,
    fileStatus,
    setFileStatus,
    fileInputRef,
    handleFileChange,
    handleFileUpload,
    setupUploadListeners,
    showConfirmModal,
    confirmMode,
    handleConfirmationResponse,
  } = useFileUpload(mcumgr);

  const {
    fileSystemUploadProgress,
    fileSystemUploadStatus,
    fileFsInputRef,
    handleFileSystemSelection,
    startFileSystemUpload,
    setupFileSystemListeners,
  } = useFileSystemUpload(mcumgr);

  const {
    firmwares,
    isFetchingFirmwares,
    fetchAndSetFirmwares,
    downloadAndFlashFirmware,
  } = useFirmwareManagement(githubConfig);

  // Setup listeners when component mounts
  useEffect(() => {
    if (mcumgr) {
      setupUploadListeners();
      setupFileSystemListeners();
    }
  }, [setupUploadListeners, setupFileSystemListeners, mcumgr]);



  // Wrapper function that provides setFileStatus to downloadAndFlashFirmware
  const handleDownloadFirmware = (runId, artifactId) => {
    downloadAndFlashFirmware(runId, artifactId, setFileStatus);
  };

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
      <div className="grid grid-cols-1 lg:grid-cols-8 xl:grid-cols-11 gap-6">
        {/* Left Column - Sidebar with Connection, Browser Support & Troubleshooting */}
        <div className="lg:col-span-2 xl:col-span-3 space-y-6">
          {/* Connection Status & Controls */}
          <div>
            <h2 className="text-xl font-semibold text-gray-900 dark:text-white mb-3">Connection</h2>
            <div className="border-b border-gray-200 dark:border-white/20 mb-4"></div>
            
            <div className="bg-white dark:bg-white/5 backdrop-blur-sm rounded-lg p-6 shadow-sm">
              <div className="flex items-center justify-between mb-4">
                <div className={`px-2 py-1 rounded-full text-xs font-medium ${
                  isConnected ? 'bg-green-100 dark:bg-green-900/30 text-green-800 dark:text-green-300' :
                  isConnecting ? 'bg-yellow-100 dark:bg-yellow-900/30 text-yellow-800 dark:text-yellow-300 animate-pulse' :
                  'bg-gray-100 dark:bg-gray-700 text-gray-800 dark:text-gray-300'
                }`}>
                  {isConnected ? `Connected (${transportLabel})` : isConnecting ? 'Connecting...' : 'Disconnected'}
                </div>
              </div>
              <div className="space-y-4">
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
                      Bluetooth (BLE)
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
                      USB Serial (WebSerial)
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

                <div className={`p-3 rounded-lg border ${
                  selectedTransportAvailable
                    ? 'bg-green-50 dark:bg-green-900/20 border-green-200 dark:border-green-800'
                    : 'bg-red-50 dark:bg-red-900/20 border-red-200 dark:border-red-800'
                }`}>
                  <div className="flex items-center">
                    <span className="mr-2">{selectedTransportAvailable ? '✅' : '❌'}</span>
                    <span className={`text-sm font-medium ${
                      selectedTransportAvailable ? 'text-green-800 dark:text-green-300' : 'text-red-800 dark:text-red-300'
                    }`}>
                      {transport === 'ble' ? 'Bluetooth (BLE)' : 'USB Serial (WebSerial)'} {selectedTransportAvailable ? 'Available' : 'Unavailable'}
                    </span>
                  </div>
                  <p className={`mt-2 text-xs leading-relaxed ${
                    selectedTransportAvailable ? 'text-green-700 dark:text-green-200' : 'text-red-700 dark:text-red-300'
                  }`}>
                    {transport === 'ble'
                      ? 'Requires a browser with Web Bluetooth support (Chrome, Edge, Opera). Ensure the watch is advertising and not already connected to another device.'
                      : 'Requires Web Serial support (Chrome or Edge) and watch is in MCUBoot. Enter MCUBoot by holding down top right button while resetting the watch. Or holding down two right buttons for 25 seconds.'}
                  </p>
                </div>

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
                      onClick={handleConnect}
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
          <div>
            <h3 className="text-xl font-semibold text-gray-900 dark:text-white mb-3">Browser Support</h3>
            <div className="border-b border-gray-200 dark:border-white/20 mb-4"></div>
            
            <div className="bg-white dark:bg-white/5 backdrop-blur-sm rounded-lg p-6 shadow-sm">
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
          </div>

          {/* Troubleshooting */}
          <div>
            <h3 className="text-xl font-semibold text-gray-900 dark:text-white mb-3">Troubleshooting</h3>
            <div className="border-b border-gray-200 dark:border-white/20 mb-4"></div>
            
            <div className="bg-white dark:bg-white/5 backdrop-blur-sm rounded-lg p-6 shadow-sm">
              <div className="space-y-3">
                <details className="group">
                  <summary className="flex items-center justify-between cursor-pointer text-gray-900 dark:text-white hover:text-zswatch-primary transition-colors text-sm">
                    <span>Connection issues</span>
                    <span className="group-open:rotate-180 transition-transform">▼</span>
                  </summary>
                  <div className="mt-3 space-y-2 text-xs text-gray-600 dark:text-gray-300 pl-4 border-l-2 border-zswatch-primary/30">
                    <p className="m-0">• Forget device in OS Bluetooth settings</p>
                    <p className="m-0">• Ensure watch is on</p>
                    <p className="m-0">• Use supported browser</p>
                  </div>
                </details>
                
                <details className="group">
                  <summary className="flex items-center justify-between cursor-pointer text-gray-900 dark:text-white hover:text-zswatch-primary transition-colors text-sm">
                    <span>Upload fails</span>
                    <span className="group-open:rotate-180 transition-transform">▼</span>
                  </summary>
                  <div className="mt-3 space-y-2 text-xs text-gray-600 dark:text-gray-300 pl-4 border-l-2 border-zswatch-primary/30">
                    <p className="m-0">• Reset watch, reload page and reconnect</p>
                    <p className="m-0">• Check file format (.zip or .bin)</p>
                    <p className="m-0">• Try switching to USB serial mode.</p>
                  </div>
                </details>
              </div>
            </div>
          </div>
        </div>

        {/* Middle Column - Manual Upload & File System */}
        <div className="lg:col-span-3 xl:col-span-4 space-y-6">
          {/* Manual Firmware Upload */}
          <div>
            <h2 className="text-xl font-semibold text-gray-900 dark:text-white mb-3">Manual Upload</h2>
            <div className="border-b border-gray-200 dark:border-white/20 mb-4"></div>
            
            <div className="bg-white dark:bg-white/5 backdrop-blur-sm rounded-lg p-4 shadow-sm">
              <FileUploadSection
                fileInputRef={fileInputRef}
                fileStatus={fileStatus}
                fileUploadPercentage={fileUploadPercentage}
                fileUploadSpeed={fileUploadSpeed}
                fileInfos={fileInfos}
                isFileUploadInProgress={isFileUploadInProgress}
                onFileChange={handleFileChange}
                onFileUpload={handleFileUpload}
                onEraseFiles={() => mcumgr && mcumgr.cmdImageErase()}
                onConfirmFiles={() => mcumgr && mcumgr.cmdImageConfirm()}
                isConnected={isConnected}
                transport={transport}
              />
            </div>
          </div>

          {/* File System Upload */}
          <div>
            <h2 className="text-xl font-semibold text-gray-900 dark:text-white mb-3">File System</h2>
            <div className="border-b border-gray-200 dark:border-white/20 mb-4"></div>
            
            <div className="bg-white dark:bg-white/5 backdrop-blur-sm rounded-lg p-4 shadow-sm">
              <FileSystemUpload
                fileFsInputRef={fileFsInputRef}
                fileSystemUploadStatus={fileSystemUploadStatus}
                fileSystemUploadProgress={fileSystemUploadProgress}
                onFileSystemSelection={handleFileSystemSelection}
                onFileSystemUploadStart={startFileSystemUpload}
                isConnected={isConnected}
                transport={transport}
              />
            </div>
          </div>
        </div>

        {/* Right Column - Prebuilt Firmware & Images */}
        <div className="lg:col-span-3 xl:col-span-4 space-y-6">
          {/* Prebuilt Firmware */}
          <div>
            <h2 className="text-xl font-semibold text-gray-900 dark:text-white mb-3">Prebuilt Firmware</h2>
            <div className="border-b border-gray-200 dark:border-white/20 mb-4"></div>
            
            <div className="bg-white dark:bg-white/5 backdrop-blur-sm rounded-lg p-6 shadow-sm">
              <div className="flex items-center justify-between mb-4">
                <button
                  onClick={() => fetchAndSetFirmwares(6)}
                  disabled={isFetchingFirmwares}
                  className="bg-zswatch-secondary/20 text-zswatch-secondary border border-zswatch-secondary/30 px-3 py-1 rounded text-sm hover:bg-zswatch-secondary/30 transition-all flex items-center gap-1"
                >
                  <span className="text-xs">🔄</span>
                  {isFetchingFirmwares ? 'Fetching...' : 'Fetch Latest'}
                </button>
              </div>
              <p className="text-gray-600 dark:text-gray-300 text-sm mb-4">Download the latest firmware from GitHub Actions.</p>
              
              <PrebuiltFirmwares 
                firmwares={firmwares}
                isFetchingFirmwares={isFetchingFirmwares}
                onFetchFirmwares={fetchAndSetFirmwares}
                onDownloadFirmware={handleDownloadFirmware}
                isConnected={isConnected}
              />
            </div>
          </div>

          {/* Images Status */}
          <div>
            <h2 className="text-xl font-semibold text-gray-900 dark:text-white mb-1">Images</h2>
            <div className="border-b border-gray-200 dark:border-white/20 mb-2"></div>
            
            <div className="bg-white dark:bg-white/5 backdrop-blur-sm rounded-lg p-6 shadow-sm">
              {!isConnected ? (
                <div className="text-center py-6">
                  <div className="text-amber-500 text-2xl mb-2">⚠️</div>
                  <p className="text-gray-600 dark:text-gray-300 text-sm">Connect to device to view image status</p>
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
                      onClick={() => mcumgr && mcumgr.cmdImageState()}
                      className="flex-1 bg-gray-100 dark:bg-white/10 text-gray-700 dark:text-white px-3 py-2 rounded text-sm hover:bg-gray-200 dark:hover:bg-white/20 transition-all"
                    >
                      Refresh
                    </button>
                    {transport === 'ble' && (
                      <>
                        <button
                          onClick={() => mcumgr && mcumgr.cmdImageTest()}
                          className="flex-1 bg-blue-100 dark:bg-blue-900/30 text-blue-700 dark:text-blue-300 px-3 py-2 rounded text-sm hover:bg-blue-200 dark:hover:bg-blue-900/50 transition-all"
                        >
                          Test
                        </button>
                        <button
                          onClick={handleConfirmImage}
                          className="flex-1 bg-green-100 dark:bg-green-900/30 text-green-700 dark:text-green-300 px-3 py-2 rounded text-sm hover:bg-green-200 dark:hover:bg-green-900/50 transition-all"
                        >
                          Confirm
                        </button>
                        <button
                          onClick={() => mcumgr && mcumgr.cmdImageErase()}
                          className="flex-1 bg-red-100 dark:bg-red-900/30 text-red-700 dark:text-red-300 px-3 py-2 rounded text-sm hover:bg-red-200 dark:hover:bg-red-900/50 transition-all"
                        >
                          Erase
                        </button>
                      </>
                    )}
                  </div>
                  
                  {transport === 'ble' ? (
                    <div className="text-xs text-gray-500 dark:text-gray-400 mt-2 flex items-center">
                      <span className="mr-1">🔗</span>
                      Bluetooth connection required for test/confirm/erase
                    </div>
                  ) : (
                    <div className="text-xs text-gray-500 dark:text-gray-400 mt-2 flex items-center">
                      <span className="mr-1">ℹ️</span>
                      Serial updates flash directly. Just reset to boot new firmware.
                    </div>
                  )}
                </div>
              ) : (
                <div className="text-center py-6">
                  <div className="text-gray-400 text-2xl mb-2">📱</div>
                  <p className="text-gray-600 dark:text-gray-300 text-sm">No images found on device</p>
                </div>
              )}
            </div>
          </div>
        </div>
      </div>
    );
  };

  return (
    <Layout>
      <div className="min-h-screen zswatch-background-gradient">
        <div className="container mx-auto px-2 py-8">
          {/* Header */}
          <div className="text-center mb-8">
            <h1 className="text-4xl font-bold mb-2 text-gray-900 dark:text-gray-100">
              ZSWatch Firmware Update
            </h1>
            <p className="text-lg text-gray-600 dark:text-gray-300">
              Update your ZSWatch firmware wirelessly over Bluetooth Low Energy
            </p>
          </div>

          {/* Main Content */}
          <div className="px-2 pb-8 max-w-7xl mx-auto">
            {renderScreen()}
          </div>

          {/* Information Cards - moved to sidebar in connected state */}
        </div>
      </div>

      {/* Confirmation Modal */}
      {showConfirmModal && (
        <div className="fixed inset-0 bg-black bg-opacity-50 flex items-center justify-center z-50">
          <div className="bg-white dark:bg-gray-800 p-6 rounded-lg shadow-xl max-w-md mx-4">
            {confirmMode === 'serial' ? (
              <>
                <h3 className="text-xl font-bold mb-4 text-gray-900 dark:text-gray-100">
                  Firmware Uploaded
                </h3>
                <p className="text-gray-600 dark:text-gray-300 mb-6">
                  Update complete. Reset the device to boot the new firmware.
                </p>
                <div className="flex gap-3 justify-end">
                  <button
                    onClick={() => handleConfirmationResponse(false)}
                    className="px-4 py-2 bg-gray-500 text-white rounded hover:bg-gray-600 transition-colors"
                  >
                    Close
                  </button>
                  <button
                    onClick={() => { handleReset(); handleConfirmationResponse(false); }}
                    className="px-4 py-2 bg-blue-600 text-white rounded hover:bg-blue-700 transition-colors"
                  >
                    Reset Now
                  </button>
                </div>
              </>
            ) : (
              <>
                <h3 className="text-xl font-bold mb-4 text-gray-900 dark:text-gray-100">
                  Confirm Firmware Images
                </h3>
                <p className="text-gray-600 dark:text-gray-300 mb-6">
                  Do you want to confirm the uploaded images and restart the device?
                  After confirming, you need to restart the device to apply the changes.
                </p>
                <div className="flex gap-3 justify-end">
                  <button
                    onClick={() => handleConfirmationResponse(false)}
                    className="px-4 py-2 bg-gray-500 text-white rounded hover:bg-gray-600 transition-colors"
                  >
                    Cancel
                  </button>
                  <button
                    onClick={() => handleConfirmationResponse(true)}
                    className="px-4 py-2 bg-blue-600 text-white rounded hover:bg-blue-700 transition-colors"
                  >
                    Confirm
                  </button>
                </div>
              </>
            )}
          </div>
        </div>
      )}
    </Layout>
  );
};

export default FirmwareUpdateApp;
