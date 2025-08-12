import React, { useEffect } from "react";
import Layout from "@theme/Layout";
import "./mcumgr.css";

// Components
import BluetoothConnect from "./components/BluetoothConnect";
import ConnectingScreen from "./components/ConnectingScreen"; 
import DeviceControls from "./components/DeviceControls";
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
    setScreen,
    handleDeviceNameChange,
    handleConnect,
    handleDisconnect,
  } = useMCUManager();

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
    clearFiles,
    setupUploadListeners,
    setFileInfos,
  } = useFileUpload(mcumgr);

  const {
    fileSystemUploadProgress,
    fileSystemUploadStatus,
    fileFsInputRef,
    selectedFile,
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

  const handleAdvancedMode = () => {
    setScreen(SCREENS.CONNECTED_ADVANCED);
  };

  const handlePreviewDemo = () => {
    setScreen(SCREENS.CONNECTED);
  };

  const renderScreen = () => {
    // Always render the unified interface
    return renderUnifiedInterface();
  };

  const renderUnifiedInterface = () => {
    const isConnected = screen === SCREENS.CONNECTED || screen === SCREENS.CONNECTED_ADVANCED;
    const isConnecting = screen === SCREENS.CONNECTING;
    
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
                  {isConnected ? 'Connected' : isConnecting ? 'Connecting...' : 'Disconnected'}
                </div>
              </div>

              {/* Bluetooth Availability Status */}
              <div className={`p-3 rounded-lg mb-4 ${
                bluetoothAvailable 
                  ? 'bg-green-50 dark:bg-green-900/20 border border-green-200 dark:border-green-800' 
                  : 'bg-red-50 dark:bg-red-900/20 border border-red-200 dark:border-red-800'
              }`}>
                <div className="flex items-center">
                  <span className="mr-2">{bluetoothAvailable ? '✅' : '❌'}</span>
                  <span className={`text-sm font-medium ${
                    bluetoothAvailable ? 'text-green-800 dark:text-green-300' : 'text-red-800 dark:text-red-300'
                  }`}>
                    Bluetooth {bluetoothAvailable ? 'Available' : 'Not Available'}
                  </span>
                </div>
              </div>

              <div className="space-y-4">
                <div>
                  <label className="block text-sm font-medium text-gray-700 dark:text-gray-300 mb-2">Device name</label>
                  <input 
                    type="text" 
                    value={deviceName}
                    onChange={(e) => handleDeviceNameChange(e.target.value)}
                    disabled={isConnected || !bluetoothAvailable}
                    className="w-full bg-white dark:bg-black/30 border border-gray-300 dark:border-white/20 rounded-lg px-3 py-2 text-gray-900 dark:text-white placeholder-gray-500 dark:placeholder-gray-400 focus:border-zswatch-primary focus:outline-none focus:ring-2 focus:ring-zswatch-primary/50 transition-all disabled:opacity-50 disabled:cursor-not-allowed text-sm"
                    placeholder="ZSWatch"
                  />
                </div>
                
                <div className="flex flex-col gap-2">
                  {!isConnected ? (
                    <button 
                      onClick={handleConnect}
                      disabled={!bluetoothAvailable || isConnecting}
                      className="px-4 py-2 bg-zswatch-primary text-black rounded-lg font-medium hover:bg-zswatch-primary/90 transition-all disabled:opacity-50 disabled:cursor-not-allowed"
                    >
                      {isConnecting ? 'Connecting...' : 'Connect'}
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
                    <p>• Forget device in OS Bluetooth settings</p>
                    <p>• Ensure watch is in DFU mode</p>
                    <p>• Use supported browser</p>
                  </div>
                </details>
                
                <details className="group">
                  <summary className="flex items-center justify-between cursor-pointer text-gray-900 dark:text-white hover:text-zswatch-primary transition-colors text-sm">
                    <span>Upload fails</span>
                    <span className="group-open:rotate-180 transition-transform">▼</span>
                  </summary>
                  <div className="mt-3 space-y-2 text-xs text-gray-600 dark:text-gray-300 pl-4 border-l-2 border-zswatch-primary/30">
                    <p>• Reset watch and reconnect</p>
                    <p>• Check file format (.zip or .bin)</p>
                    <p>• Try USB method if persistent</p>
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
                onClearFiles={clearFiles}
                isConnected={isConnected}
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
                    <button 
                      onClick={() => mcumgr && mcumgr.cmdImageTest()}
                      className="flex-1 bg-blue-100 dark:bg-blue-900/30 text-blue-700 dark:text-blue-300 px-3 py-2 rounded text-sm hover:bg-blue-200 dark:hover:bg-blue-900/50 transition-all"
                    >
                      Test
                    </button>
                    <button 
                      onClick={() => mcumgr && mcumgr.cmdImageConfirm()}
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
                  </div>
                  
                  <div className="text-xs text-gray-500 dark:text-gray-400 mt-2 flex items-center">
                    <span className="mr-1">🔗</span>
                    Bluetooth connection required
                  </div>
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
      <div className="min-h-screen" style={{
        background: "linear-gradient(135deg, #1a1f2e 0%, var(--zswatch-darker) 30%, #2a2f3e 60%, #1a1f2e 100%)"
      }}>
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
    </Layout>
  );
};

export default FirmwareUpdateApp;
