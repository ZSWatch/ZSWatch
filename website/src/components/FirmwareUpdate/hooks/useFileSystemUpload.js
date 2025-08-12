import { useState, useRef } from "react";

export const useFileSystemUpload = (mcumgr) => {
  const [fileSystemUploadProgress, setFileSystemUploadProgress] = useState(null);
  const [fileSystemUploadStatus, setFileSystemUploadStatus] = useState("Select a filesystem file");
  const [selectedFile, setSelectedFile] = useState(null);
  
  const fileFsInputRef = useRef(null);

  // Set up filesystem upload listeners
  const setupFileSystemListeners = () => {
    if (!mcumgr) return;
    
    mcumgr.onFsUploadProgress(({ percentage }) => {
      setFileSystemUploadProgress(percentage);
    });

    mcumgr.onFsUploadFinished((ok) => {
      if (ok) {
        setFileSystemUploadStatus("Upload complete");
      } else {
        setFileSystemUploadStatus("Upload failed");
      }
      setFileSystemUploadProgress(null);
    });
  };

  const handleFileSystemSelection = (event) => {
    const file = event.target.files[0];
    if (!file) {
      setFileSystemUploadStatus("No file selected");
      setSelectedFile(null);
      return;
    }

    console.log("Selected file:", file);
    setSelectedFile(file);
    setFileSystemUploadStatus(`File selected: ${file.name} - Ready to upload`);
  };

  const startFileSystemUpload = async () => {
    if (!selectedFile) {
      setFileSystemUploadStatus("No file selected");
      return;
    }

    const reader = new FileReader();
    reader.onload = async (e) => {
      const filePath = "/S/full_fs";

      setFileSystemUploadStatus("Uploading...");
      setFileSystemUploadProgress(0);

      try {
        await mcumgr.cmdUploadFileSystemImage(reader.result, filePath);
        setFileSystemUploadStatus("Upload started");
      } catch (error) {
        console.error("Filesystem upload failed:", error);
        setFileSystemUploadStatus("Upload failed");
        setFileSystemUploadProgress(null);
      }
      fileFsInputRef.current.value = null;
      setSelectedFile(null);
    };

    reader.readAsArrayBuffer(selectedFile);
  };

  return {
    fileSystemUploadProgress,
    fileSystemUploadStatus,
    fileFsInputRef,
    selectedFile,
    handleFileSystemSelection,
    startFileSystemUpload,
    setupFileSystemListeners,
  };
};
