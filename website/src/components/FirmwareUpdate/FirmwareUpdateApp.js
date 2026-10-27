import React, { useState } from "react";
import Layout from "@theme/Layout";
import "./mcumgr.css";
import WatchConnection from "../WatchSession/WatchConnection";
import styles from "../WatchSession/workspace.module.css";

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

const FirmwareUpdateApp = () => {
  const githubConfig = useGithubConfig();

  const {
    mcumgr,
    screen,
    images,
    deviceInfo,
    isSerialRecoveryMode,
    refreshImages,
    preparingFirmware,
    registerShellListener,
  } = useMCUManager();

  const isConnected = (mcumgr?.isConnected?.() && (screen === SCREENS.CONNECTED)) ?? false;

  const update = useFirmwareUpdate(mcumgr, isSerialRecoveryMode, isConnected);
  const firmwareManager = useFirmwareManagement(githubConfig, update.loadFile);
  const isBusy = update.isBusy || firmwareManager.isDownloading || preparingFirmware;
  const {
    selection, progress, fileFsInputRef, handleFileSystemSelection, startFileSystemUpload,
  } = update;
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

  return (
    <Layout title="Firmware Update" description="Update your ZSWatch firmware over USB or Bluetooth.">
      <main className={styles.page}>
        <header className={styles.hero}>
          <h1>Firmware Update</h1>
          <p>Update your watch firmware and image resources over USB or Bluetooth.</p>
        </header>
        <WatchConnection firmwareUpdate disabled={isBusy} />
        <div className={styles.frame}>
          <div className={styles.firmwareGrid}>
            <section className={styles.section} aria-labelledby="firmware-catalog-title">
              <h2 id="firmware-catalog-title">Choose firmware</h2>
              <PrebuiltFirmwares manager={firmwareManager} githubConfig={githubConfig} disabled={isBusy} hardwareVersion={deviceInfo.hardwareVersion} />
            </section>
            <section className={`${styles.section} ${styles.packageCard}`} aria-labelledby="firmware-package-title">
              <h2 id="firmware-package-title">Firmware package</h2>
              <UpdateStatus update={update} isConnected={isConnected} isSerialRecoveryMode={isSerialRecoveryMode} disabled={isBusy} />
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
            </section>
          </div>
          <details className={styles.advanced}>
            <summary>How updates work</summary>
            <div className={styles.advancedContent}>
              <p>Choose a release or CI build, or open your downloaded firmware ZIP in Firmware package. Hardware is detected when the watch reports it; otherwise select it manually. Update firmware keeps your current image resources. Choose Update firmware + resources when the new version changes icons or graphics. Both actions restart the watch. Bluetooth reconnects to verify the result; over USB, reconnect to check installed images.</p>
            </div>
          </details>
          <details className={styles.advanced}>
            <summary>Installed images &amp; diagnostics</summary>
            <div className={styles.advancedContent}>
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
              <section>
                <h2 className="text-base font-semibold">Shell console</h2>
                <button className={styles.secondaryButton} onClick={() => setShellConsoleModalOpen(true)}>Open shell console</button>
                <div className="flex flex-wrap gap-2 mt-3">
                  <button className={styles.secondaryButton} disabled={!isConnected || isBusy} onClick={handleEchoTest}>Echo test</button>
                  <button className={styles.secondaryButton} disabled={!isConnected || isBusy} onClick={handleReset}>Restart watch</button>
                </div>
              </section>
            </div>
          </details>
          <footer className={styles.footer}>Your connection stays active when switching between Firmware Update and Watch Tools.</footer>
        </div>
      </main>

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
            <div role="dialog" aria-modal="true" aria-labelledby="firmware-shell-title" className="bg-white dark:bg-gray-900 rounded-lg shadow-2xl overflow-hidden">
              <div className="flex items-center justify-between px-4 py-3 border-b border-gray-200 dark:border-white/10">
                <h3 id="firmware-shell-title" className="text-lg font-semibold text-gray-900 dark:text-white m-0">Shell Console</h3>
                <button
                  type="button"
                  onClick={() => setShellConsoleModalOpen(false)}
                  aria-label="Close shell console"
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
