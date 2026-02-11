import React from "react";

const FileUploadSection = ({ 
  fileInputRef,
  fileStatus,
  fileUploadPercentage,
  fileUploadSpeed,
  fileInfos,
  isFileUploadInProgress,
  onFileChange,
  onFileUpload,
  onEraseFiles,
  onConfirmFiles,
  isConnected = false,
  isSerialRecoveryMode = true,
}) => {
  const [isDragOver, setIsDragOver] = React.useState(false);

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
      onFileChange(syntheticEvent);
    }
  };

  const formatFileSize = (bytes) => {
    return (bytes / 1024).toFixed(1) + " KB";
  };

  return (
    <div className="space-y-6">
      {/* Enhanced Drag & Drop Zone */}
      <div className="relative">
        <input 
          type="file" 
          className="absolute inset-0 w-full h-full opacity-0 cursor-pointer z-10" 
          onChange={onFileChange}
          ref={fileInputRef}
          accept=".zip,.bin"
        />
        <div 
          className={`
            relative border-2 border-dashed rounded-xl p-4 text-center transition-all duration-200
            ${isDragOver 
              ? 'border-zswatch-primary bg-zswatch-primary/5 scale-105' 
              : 'border-gray-300 dark:border-gray-600 hover:border-zswatch-primary/50 hover:bg-zswatch-primary/5'
            }
          `}
          onDragOver={handleDragOver}
          onDragLeave={handleDragLeave}
          onDrop={handleDrop}
        >
          <div className={`transition-all duration-200 ${isDragOver ? 'scale-110' : ''}`}>
            <h3 className="text-lg font-semibold text-gray-900 dark:text-white mb-2">
              {isDragOver ? 'Drop your files here' : 'Upload Firmware Files'}
            </h3>
            <p className="text-gray-600 dark:text-gray-400 mb-2">
              Drag and drop the <code>dfu_application.zip</code> from your firmware package here
            </p>
          </div>
        </div>
      </div>

      {/* Upload Progress */}
      {fileUploadPercentage !== null && (
        <div className="space-y-2">
          <div className="flex justify-between text-sm">
            <span className="text-gray-600 dark:text-gray-400">Upload Progress</span>
            <span className="font-medium text-gray-900 dark:text-white">{fileUploadPercentage}%</span>
          </div>
          <div className="w-full bg-gray-200 dark:bg-gray-700 rounded-full h-3">
            <div 
              className="bg-gradient-to-r from-zswatch-secondary to-zswatch-primary h-3 rounded-full transition-all duration-300 relative overflow-hidden" 
              style={{width: `${fileUploadPercentage}%`}}
            >
              <div className="absolute inset-0 bg-white/20 animate-pulse"></div>
            </div>
          </div>
          {fileUploadSpeed && (
            <p className="text-sm text-gray-600 dark:text-gray-400 text-right">{fileUploadSpeed} KB/s</p>
          )}
        </div>
      )}

      {/* Enhanced File Information with Two-Column Grid */}
      {fileInfos && fileInfos.length > 0 && (
        <div className="space-y-4">
          <div className="flex items-center gap-2">
            <h4 className="text-sm font-medium text-gray-900 dark:text-white">
              📋 Files ({fileInfos.length})
              {fileStatus && fileStatus !== "Select image files (.bin or .zip)" && (
                <span className="ml-2 text-xs text-gray-600 dark:text-gray-400">- {fileStatus}</span>
              )}
            </h4>
          </div>
          <div className="grid grid-cols-1 lg:grid-cols-2 gap-3">
            {fileInfos.map((fileInfo, index) => (
              <div key={index} className="bg-white dark:bg-white/5 border border-gray-200 dark:border-white/10 rounded-lg p-3">
                <div className="flex items-start justify-between mb-2">
                  <div className="flex items-start gap-2 min-w-0 flex-1">
                    <span className="text-sm mt-0.5">📄</span>
                    <div className="min-w-0 flex-1">
                      <h5 className="font-medium text-gray-900 dark:text-white text-sm truncate mb-1">{fileInfo.name}</h5>
                      <div className="space-y-1">
                        <div className="flex items-center gap-3 text-xs text-gray-500 dark:text-gray-400">
                          <span className="bg-gray-100 dark:bg-gray-700 px-1.5 py-0.5 rounded">Image #{fileInfo.imageNumber}</span>
                          <span>v{fileInfo.version}</span>
                        </div>
                        <div className="text-xs text-gray-500 dark:text-gray-400">
                          {formatFileSize(fileInfo.fileSize)}
                        </div>
                      </div>
                    </div>
                  </div>
                  <div className="flex flex-col items-end gap-1 flex-shrink-0 ml-2">
                    {fileInfo.isUploaded && (
                      <span className="bg-green-100 dark:bg-green-500/20 text-green-700 dark:text-green-400 px-2 py-1 rounded text-xs">
                        ✅ Done
                      </span>
                    )}
                    {fileInfo.isUploading && (
                      <span className="bg-blue-100 dark:bg-blue-500/20 text-blue-700 dark:text-blue-400 px-2 py-1 rounded text-xs flex items-center gap-1">
                        <div className="w-2 h-2 border border-blue-500 border-t-transparent rounded-full animate-spin"></div>
                        {fileInfo.uploadPercentage || 0}%
                      </span>
                    )}
                  </div>
                </div>
                

              </div>
            ))}
          </div>
        </div>
      )}

      {/* Enhanced Action Buttons */}
      <div className="flex flex-wrap gap-3 mt-4">
        <button 
          onClick={onFileUpload}
          disabled={!fileInfos || fileInfos.length === 0 || isFileUploadInProgress || !isConnected}
          className="flex-1 px-4 py-2 text-sm font-medium transition-all border rounded group bg-blue-50 dark:bg-blue-500/10 text-blue-700 dark:text-blue-400 border-blue-200 dark:border-blue-500/30 hover:bg-blue-100 dark:hover:bg-blue-500/20 disabled:opacity-50 disabled:cursor-not-allowed"
        >
          <div className="relative">
            {isFileUploadInProgress ? 'Uploading...' : 'Upload Firmware'}
          </div>
        </button>
        {!isSerialRecoveryMode && (
          <>
            <button
              onClick={onEraseFiles}
              disabled={!isConnected}
              className="flex-1 px-4 py-2 text-sm font-medium transition-all border rounded group bg-red-50 dark:bg-red-500/10 text-red-700 dark:text-red-400 border-red-200 dark:border-red-500/30 hover:bg-red-100 dark:hover:bg-red-500/20 disabled:opacity-50 disabled:cursor-not-allowed"
            >
              <div className="relative">
                Erase Files
              </div>
            </button>
            <button
              onClick={onConfirmFiles}
              disabled={!isConnected}
              className="flex-1 px-4 py-2 text-sm font-medium transition-all border rounded group bg-gray-50 dark:bg-white/5 text-gray-700 dark:text-gray-300 border-gray-200 dark:border-white/10 hover:bg-gray-100 dark:hover:bg-white/10 disabled:opacity-50 disabled:cursor-not-allowed"
            >
              <div className="relative">
                Confirm Files
              </div>
            </button>
          </>
        )}
      </div>

    </div>
  );
};

export default FileUploadSection;
