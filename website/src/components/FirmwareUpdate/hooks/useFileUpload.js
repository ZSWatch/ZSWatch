import { useState, useRef } from "react";
import JSZip from "jszip";
import { binaryFileNameToImageId, binaryFileNameToImageIdSerialRecovery } from "../constants";

export const useFileUpload = (mcumgr, isSerialRecoveryMode) => {
  const [fileUploadPercentage, setFileUploadPercentage] = useState(null);
  const [fileUploadSpeed, setFileUploadSpeed] = useState(null);
  const [isFileUploadInProgress, setIsFileUploadInProgress] = useState(false);
  const [fileInfos, setFileInfos] = useState([]);
  const [fileStatus, setFileStatus] = useState("Select image files (.bin or .zip)");
  const [showConfirmModal, setShowConfirmModal] = useState(false);

  const fileInputRef = useRef(null);
  const uploadStartTimeRef = useRef(null);
  const lastUpdateTimeRef = useRef(null);
  const fileSizeRef = useRef(null);
  const lastPercentageRef = useRef(0);

  // Set up MCU Manager upload event listeners
  const setupUploadListeners = () => {
    if (!mcumgr) return;
    
    mcumgr.onImageUploadProgress(({ percentage }) => {
      setFileStatus(`Uploading... ${percentage}%`);
      setFileUploadPercentage(percentage);
      const currentTime = Date.now();
      const elapsedTime = (currentTime - lastUpdateTimeRef.current) / 1000;
      const percentageDelta = percentage - lastPercentageRef.current;

      if (percentageDelta > 0 && fileSizeRef.current) {
        const bytesTransferred = (percentageDelta / 100) * fileSizeRef.current;
        const speed = (bytesTransferred / 1024) / elapsedTime;
        setFileUploadSpeed(speed.toFixed(2));
        lastUpdateTimeRef.current = currentTime;
        lastPercentageRef.current = percentage;
      }
    });

    mcumgr.onImageUploadFinished(async () => {
      setFileStatus("Upload complete");
      setFileUploadSpeed(null);
      console.log("File upload finished", fileInfos);
      
      // Find the file that was just uploaded
      const uploadedFile = fileInfos.find((file) => file.isUploading);

      // Update file status first
      setFileInfos((prevFileInfos) =>
        prevFileInfos.map((file) =>
          file.isUploading ? { ...file, isUploaded: true, isUploading: false } : file,
        ),
      );

      // Check if we just uploaded net_core in serial recovery mode
      if (isSerialRecoveryMode && uploadedFile && uploadedFile.name === "ipc_radio.bin") {
        console.log("Net core uploaded, waiting 30 seconds for device to load it...");

        for (let i = 30; i > 0; i--) {
          setFileStatus(`Net core uploaded - waiting ${i}s for device to load...`);
          await new Promise(resolve => setTimeout(resolve, 1000));
        }

        setFileStatus("Net core loaded, ready for next upload");
      }

      const fileInfo = fileInfos.find((file) => !file.isUploaded && !file.isUploading);
      if (fileInfo) {
        doFwUpload(fileInfo);
      } else {
        console.log("All files uploaded");
        setFileStatus("Upload complete");
        mcumgr.cmdImageState();
        promptForConfirmationOfFwImages();
      }
    });
  };

  const handleBinFileUpload = async (selectedFile, reader) => {
    let imageNumber = binaryFileNameToImageId[selectedFile.name];
    if (imageNumber == undefined) {
      imageNumber = parseInt(prompt("Enter the image number:", "0"), 10);
      if (isNaN(imageNumber) || imageNumber < 0) {
        alert("Invalid image number. Please enter a non-negative integer.");
        setFileStatus("Ready to upload");
        return;
      }
    }
    try {
      const info = await mcumgr.imageInfo(reader.result);
      setFileInfos((prevFileInfos) => {
        const updatedFileInfos = prevFileInfos.filter(
          (fileInfo) => fileInfo.imageNumber !== imageNumber,
        );
        return [
          ...updatedFileInfos,
          {
            version: info.version,
            hash: info.hash,
            fileSize: reader.result.byteLength,
            imageNumber: imageNumber,
            fileData: reader.result,
            name: selectedFile.name,
            isUploaded: false,
            isUploading: false,
            isSetPending: false,
            mcuMgrImageStatus: "",
          },
        ];
      });
      setFileStatus("Binary file processed - Ready to upload");
    } catch (error) {
      console.error("Error reading file:", error);
      setFileStatus("Invalid image file");
    }
  };

  const handleZipFileUpload = async (selectedFile, reader) => {
    const zip = new JSZip();
    try {
      const zipContent = await zip.loadAsync(reader.result);
      const fwFiles = await Promise.all(
        Object.keys(zipContent.files)
          .filter((filename) => filename.endsWith(".bin"))
          .map(async (filename) => {
            const fileData = await zipContent.files[filename].async("arraybuffer");
            const imageNumber = binaryFileNameToImageId[filename];
            if (imageNumber == undefined) {
              alert(`Invalid file: ${filename}. Please use a valid binary file name.`);
              return null;
            }
            const info = await mcumgr.imageInfo(fileData);
            return {
              version: info.version,
              hash: info.hash,
              fileSize: fileData.byteLength,
              imageNumber: imageNumber,
              fileData: fileData,
              name: filename,
              isUploaded: false,
              isUploading: false,
              isSetPending: false,
              mcuMgrImageStatus: "",
            };
          }),
      );
      setFileInfos((prevFileInfos) => [
        ...prevFileInfos.filter(
          (fileInfo) =>
            !fwFiles.some((fwFile) => fwFile && fwFile.imageNumber === fileInfo.imageNumber),
        ),
        ...fwFiles.filter((fileInfo) => fileInfo !== null),
      ]);
      setFileStatus("Zip file processed - Ready to upload");
    } catch (error) {
      console.error("Error reading zip file:", error);
      setFileStatus("Invalid zip file");
    }
  };

  const handleFileChange = (e) => {
    const selectedUploadFile = e.target.files[0];
    if (!selectedUploadFile) {
      setFileStatus("No file selected");
      return;
    }

    setFileStatus(`Processing ${selectedUploadFile.name}...`);

    const reader = new FileReader();
    reader.onload = async () => {
      if (selectedUploadFile.type === "application/zip") {
        await handleZipFileUpload(selectedUploadFile, reader);
      } else {
        await handleBinFileUpload(selectedUploadFile, reader);
      }
    };
    reader.readAsArrayBuffer(selectedUploadFile);
  };

  const doFwUpload = async (fileInfo) => {
    if (!fileInfo) {
      setFileStatus("Ready to upload");
      return;
    }

    // Determine the actual image number to use based on device mode
    // In serial recovery mode (MCUBoot), we need to remap the image numbers
    let actualImageNumber = fileInfo.imageNumber;
    if (isSerialRecoveryMode && binaryFileNameToImageIdSerialRecovery[fileInfo.name] !== undefined) {
      actualImageNumber = binaryFileNameToImageIdSerialRecovery[fileInfo.name];
      console.log(`Serial recovery mode: Remapping ${fileInfo.name} from slot ${fileInfo.imageNumber} to slot ${actualImageNumber}`);
    }

    console.log("Starting upload for", fileInfo, "to image slot", actualImageNumber);

    setIsFileUploadInProgress(true);
    setFileUploadPercentage(0);
    setFileUploadSpeed(null);
    setFileStatus("Uploading...");

    fileSizeRef.current = fileInfo.fileSize;
    uploadStartTimeRef.current = Date.now();
    lastUpdateTimeRef.current = Date.now();
    lastPercentageRef.current = 0;

    try {
      await mcumgr.cmdUpload(fileInfo.fileData, actualImageNumber);
      setIsFileUploadInProgress(false);
      setFileInfos((prevFileInfos) =>
        prevFileInfos.map((file) => (file === fileInfo ? { ...file, isUploading: true } : file)),
      );
    } catch (error) {
      setFileStatus("Upload failed");
      setIsFileUploadInProgress(false);
    }
  };

  const handleFileUpload = async () => {
    if (fileInfos.length <= 0) {
      setIsFileUploadInProgress(false);
    } else if (fileInfos[0].fileData) {
      const fileInfo = fileInfos.find((file) => !file.isUploaded);
      if (fileInfo) {
        doFwUpload(fileInfo);
      } else {
        setFileStatus("All files uploaded");
        setIsFileUploadInProgress(false);
        promptForConfirmationOfFwImages();
      }
    }
  };

  const promptForConfirmationOfFwImages = async () => {
    // Show modal after upload completes
    setShowConfirmModal(true);
  };

  const handleConfirmationResponse = async (confirmed) => {
    setShowConfirmModal(false);
    if (!confirmed) return;

    // Only confirm by hash if device is in full application mode (not serial recovery)
    if (!isSerialRecoveryMode) {
      for (const fileInfo of fileInfos) {
        const hashArray = Uint8Array.from(
          fileInfo.hash.match(/.{1,2}/g).map((byte) => parseInt(byte, 16)),
        );
        await mcumgr.cmdImageConfirm(hashArray);
      }
      // Wait a bit for the confirm command to take effect
      await new Promise(resolve => setTimeout(resolve, 2000));
      // Reset device after confirming images to use them
      await mcumgr.cmdReset();
    }
  };

  const clearFiles = () => {
    setFileInfos([]);
    setFileStatus("Select image files (.bin or .zip)");
    if (fileInputRef.current) fileInputRef.current.value = '';
  };

  return {
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
    setFileInfos, // For updating file status from parent
    showConfirmModal,
    handleConfirmationResponse,
  };
};
