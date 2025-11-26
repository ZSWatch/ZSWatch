import { useState } from "react";

const FileSystemUpload = ({
  fileFsInputRef,
  fileSystemUploadStatus,
  fileSystemUploadProgress,
  fileSystemUploadSpeed,
  onFileSystemSelection,
  onFileSystemUploadStart,
  isConnected = false,
  isSerialRecoveryMode = true,
}) => {
  const [isDragOver, setIsDragOver] = useState(false);
  // Filesystem upload is supported in full application mode (not serial recovery mode)
  const canUploadFilesystem = !isSerialRecoveryMode && isConnected;

  const handleDragOver = (e) => {
    e.preventDefault();
    setIsDragOver(true);
  };

  const handleDragLeave = (e) => {
    e.preventDefault();
    setIsDragOver(false);
  };

  const handleDrop = (e) => {
    e.preventDefault();
    setIsDragOver(false);
    if (e.dataTransfer.files.length > 0) {
      const syntheticEvent = {
        target: { files: e.dataTransfer.files }
      };
      onFileSystemSelection(syntheticEvent);
    }
  };

  return (
    <div className="space-y-6">
      {/* Enhanced File System Drag & Drop Zone */}
      <div className="relative">
        <input 
          type="file" 
          ref={fileFsInputRef}
          onChange={onFileSystemSelection}
          className="absolute inset-0 w-full h-full opacity-0 cursor-pointer z-10" 
          accept=".bin"
        />
        <div 
          className={`
            relative border-2 border-dashed rounded-xl p-4 text-center transition-all duration-200
            ${isDragOver 
              ? 'border-zswatch-secondary bg-zswatch-secondary/5 scale-105' 
              : 'border-gray-300 dark:border-gray-600 hover:border-zswatch-secondary/50 hover:bg-zswatch-secondary/5'
            }
          `}
          onDragOver={handleDragOver}
          onDragLeave={handleDragLeave}
          onDrop={handleDrop}
        >
          <div className={`transition-all duration-200 ${isDragOver ? 'scale-110' : ''}`}>
            <h3 className="text-lg font-semibold text-gray-900 dark:text-white mb-2">
              {isDragOver ? 'Drop your file system file here' : 'Upload File System'}
            </h3>
            <p className="text-gray-600 dark:text-gray-400 mb-4">
              Drag and drop your lvgl_resources_raw.bin file here to upload full filesystem
            </p>
          </div>
        </div>
      </div>

      {/* Filesystem upload status messages */}
      {!isConnected && (
        <div className="text-xs text-gray-600 dark:text-gray-300">
          ℹ️ Please connect to device to upload filesystem.
        </div>
      )}
      {isConnected && isSerialRecoveryMode && (
        <div className="text-xs text-amber-600 dark:text-amber-400">
          ⚠️ Filesystem upload not available. Device must be running the application (not in MCUBoot mode).
        </div>
      )}

      {/* Upload Progress */}
      {fileSystemUploadProgress !== null && (
        <div className="space-y-2">
          <div className="flex justify-between text-sm">
            <span className="text-gray-600 dark:text-gray-400">Upload Progress</span>
            <div className="flex items-center gap-2">
              <span className="font-medium text-gray-900 dark:text-white">{fileSystemUploadProgress}%</span>
              {fileSystemUploadSpeed && (
                <span className="text-xs text-gray-500 dark:text-gray-400">
                  ({fileSystemUploadSpeed} KB/s)
                </span>
              )}
            </div>
          </div>
          <div className="w-full bg-gray-200 dark:bg-gray-700 rounded-full h-3">
            <div 
              className="bg-gradient-to-r from-zswatch-secondary to-zswatch-primary h-3 rounded-full transition-all duration-300 relative overflow-hidden" 
              style={{width: `${fileSystemUploadProgress}%`}}
            >
              <div className="absolute inset-0 bg-white/20 animate-pulse"></div>
            </div>
          </div>
        </div>
      )}

      {/* File Status and Upload Button */}
      {fileSystemUploadStatus && fileSystemUploadStatus !== "Select a filesystem file" && (
        <div className="space-y-4">
          <div className="flex items-center gap-2">
            <h4 className="text-sm font-medium text-gray-900 dark:text-white">
              📋 File System
              <span className="ml-2 text-xs text-gray-600 dark:text-gray-400">- {fileSystemUploadStatus}</span>
            </h4>
          </div>
          
          {/* Upload Button */}
          <div className="flex gap-3">
            <button 
              onClick={onFileSystemUploadStart}
              disabled={!canUploadFilesystem || fileSystemUploadProgress !== null}
              className="flex-1 px-4 py-2 text-sm font-medium transition-all border rounded group bg-blue-50 dark:bg-blue-500/10 text-blue-700 dark:text-blue-400 border-blue-200 dark:border-blue-500/30 hover:bg-blue-100 dark:hover:bg-blue-500/20 disabled:opacity-50 disabled:cursor-not-allowed"
            >
              <div className="relative">
                {fileSystemUploadProgress !== null ? 'Uploading...' : 'Upload File System'}
              </div>
            </button>
          </div>
        </div>
      )}
    </div>
  );
};

export default FileSystemUpload;
