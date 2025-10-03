import { useState, useRef } from "react";

export const useFileSystemUpload = (mcumgr) => {
  const [fileSystemUploadProgress, setFileSystemUploadProgress] = useState(null);
  const [fileSystemUploadSpeed, setFileSystemUploadSpeed] = useState(null);
  const [fileSystemUploadStatus, setFileSystemUploadStatus] = useState("Select a filesystem file");
  const [selectedFile, setSelectedFile] = useState(null);
  
  const fileFsInputRef = useRef(null);
  const fsLastUpdateTimeRef = useRef(null);
  const fsFileSizeRef = useRef(null);
  const fsLastPercentageRef = useRef(0);

  // Set up filesystem upload listeners
  const setupFileSystemListeners = () => {
    if (!mcumgr) return;
    
    mcumgr.onFsUploadProgress(({ percentage }) => {
      setFileSystemUploadProgress(percentage);
      setFileSystemUploadStatus(`Uploading... ${percentage}%`);

      const currentTime = Date.now();
      const elapsedTime = (currentTime - fsLastUpdateTimeRef.current) / 1000;
      const percentageDelta = percentage - fsLastPercentageRef.current;

      if (percentageDelta > 0 && fsFileSizeRef.current) {
        const bytesTransferred = (percentageDelta / 100) * fsFileSizeRef.current;
        const speed = (bytesTransferred / 1024) / elapsedTime;
        setFileSystemUploadSpeed(speed.toFixed(2));
        fsLastUpdateTimeRef.current = currentTime;
        fsLastPercentageRef.current = percentage;
      }
    });

    mcumgr.onFsUploadFinished((ok) => {
      if (ok) {
        setFileSystemUploadStatus("Upload complete");
      } else {
        setFileSystemUploadStatus("Upload failed");
      }
      setFileSystemUploadProgress(null);
      setFileSystemUploadSpeed(null);
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
      setFileSystemUploadSpeed(null);

      fsFileSizeRef.current = selectedFile.size;
      fsLastUpdateTimeRef.current = Date.now();
      fsLastPercentageRef.current = 0;

      try {
        //mcumgr.setChunkTimeout(5000); // Set timeout to 5 seconds
        await mcumgr.cmdUploadFileSystemImage(reader.result, filePath);
        setFileSystemUploadStatus("Upload started");
      } catch (error) {
        console.error("Filesystem upload failed:", error);
        setFileSystemUploadStatus("Upload failed");
        setFileSystemUploadProgress(null);
        setFileSystemUploadSpeed(null);
      }
      fileFsInputRef.current.value = null;
      setSelectedFile(null);
    };

    reader.readAsArrayBuffer(selectedFile);
  };

  return {
    fileSystemUploadProgress,
    fileSystemUploadSpeed,
    fileSystemUploadStatus,
    fileFsInputRef,
    selectedFile,
    handleFileSystemSelection,
    startFileSystemUpload,
    setupFileSystemListeners,
  };
};
