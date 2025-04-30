import Layout from "@theme/Layout";
import React, { useState, useEffect, useRef } from "react";
import Admonition from "@theme/Admonition";
import "./mcumgr.css";
import MCUManager from "../mcumgr-web/js/mcumgr";
import JSZip from "jszip";

import {
  MGMT_GROUP_ID_IMAGE,
  IMG_MGMT_ID_STATE,
  MGMT_GROUP_ID_OS,
  OS_MGMT_ID_ECHO,
  OS_MGMT_ID_TASKSTAT,
  OS_MGMT_ID_MPSTAT,
} from "../mcumgr-web/js/mcumgr";

const imageNameMapping = {
  0: "App Internal",
  1: "Net Core",
  2: "App External (XIP)",
};

const binaryFileNameToImageId = {
  "app.internal.bin": 0,
  "ipc_radio.bin": 1,
  "app.external.bin": 2,
};

const hashArrayToString = (hash) => {
  return Array.from(hash)
    .map((byte) => byte.toString(16).padStart(2, "0"))
    .join("");
};

const owner = "ZSWatch"; // Replace with the repository owner
const repo = "ZSWatch"; // Replace with the repository name
const token = null;

const fetchArtifacts = async (numRuns = 5) => {
  let firmwares = [];
  try {
    // Step 1: Get the list of workflow runs
    const runsResponse = await fetch(
      `https://api.github.com/repos/${owner}/${repo}/actions/runs`,
      token
        ? {
            headers: {
              Authorization: `Bearer ${token}`,
            },
          }
        : {},
    );
    const runsData = await runsResponse.json();

    if (!runsData.workflow_runs || runsData.workflow_runs.length === 0) {
      console.log("No workflow runs found.");
      return;
    }

    console.log("Found", runsData, runsData.workflow_runs.length, "workflow runs.");

    // Step 2: Get the latest X workflow runs3
    const latestRuns = runsData.workflow_runs
      .filter((run) => run.conclusion === "success" && run.head_branch !== "gh-pages")
      .slice(0, numRuns);
    console.log(`Fetching artifacts from the latest ${numRuns} workflow runs...`);

    for (const run of latestRuns) {
      console.log(`Processing workflow run: ${run.id} (${run.name})`, run);

      // Step 3: Get the artifacts for the current workflow run
      const artifactsResponse = await fetch(
        `https://api.github.com/repos/${owner}/${repo}/actions/runs/${run.id}/artifacts`,
        token
          ? {
              headers: {
                Authorization: `Bearer ${token}`,
              },
            }
          : {},
      );
      const artifactsData = await artifactsResponse.json();

      if (!artifactsData.artifacts || artifactsData.artifacts.length === 0) {
        console.log(`No artifacts found for workflow run: ${run.id}`);
        continue;
      }

      const fwRun = {
        branch: run.head_branch,
        user: run.actor.login,
        runId: run.id,
        artifacts: artifactsData.artifacts.filter(
          (artifact) => artifact.name.includes("@4") || artifact.name.includes("@5"),
        ),
      };

      firmwares.push(fwRun);
    }
  } catch (error) {
    console.error("Error fetching artifacts:", error);
  }
  return firmwares;
};

const FirmwareUpdateApp = () => {
  const [deviceName, setDeviceName] = useState('ZSWatch');
  const [screen, setScreen] = useState("initial");
  const [fileUploadPercentage, setFileUploadPercentage] = useState(null);
  const [fileUploadSpeed, setFileUploadSpeed] = useState(null); // New state for speed
  const [isFileUploadInProgress, setIsFileUploadInProgress] = useState(false);
  const [images, setImages] = useState([]);
  const [fileInfos, setFileInfos] = useState([]);
  const [fileStatus, setFileStatus] = useState("Select image files (.bin or .zip)");
  const [bluetoothAvailable, setBluetoothAvailable] = useState(false);
  const [isFetchingFirmwares, setIsFetchingFirmwares] = useState(false);

  const [fileSystemUploadProgress, setFileSystemUploadProgress] = useState(null); // Progress for filesystem upload
  const [fileSystemUploadStatus, setFileSystemUploadStatus] = useState("Select a filesystem file"); // Status for filesystem upload

  // Use useRef to create a stable mcumgr instance
  const mcumgrRef = useRef(new MCUManager());
  const mcumgr = mcumgrRef.current;

  const fileInputRef = useRef(null);
  const fileFsInputRef = useRef(null);

  const uploadStartTimeRef = useRef(null); // Start time of the upload
  const lastUpdateTimeRef = useRef(null); // Time of the last percentage update
  const fileSizeRef = useRef(null); // File size in bytes
  const lastPercentageRef = useRef(0); // Last percentage value

  const [firmwares, setFirmwares] = useState([]);

  useEffect(() => {
    const isBluetoothAvailable = navigator?.bluetooth?.getAvailability() || false;
    setBluetoothAvailable(isBluetoothAvailable);

    // Set up mcumgr event listeners
    mcumgr.onConnecting(() => {
      setScreen("connecting");
    });

    mcumgr.onConnect(() => {
      setDeviceName(mcumgr.name);
      setScreen("connected");
      mcumgr.cmdImageState();
    });

    mcumgr.onDisconnect(() => {
      setScreen("initial");
    });

    mcumgr.onImageUploadProgress(({ percentage }) => {
      setFileStatus(`Uploading... ${percentage}%`);
      setFileUploadPercentage(percentage);
      const currentTime = Date.now();
      const elapsedTime = (currentTime - lastUpdateTimeRef.current) / 1000; // Time in seconds
      const percentageDelta = percentage - lastPercentageRef.current;

      if (percentageDelta > 0 && fileSizeRef.current) {
        const bytesTransferred = (percentageDelta / 100) * fileSizeRef.current;
        const speed = (bytesTransferred / 1024) / elapsedTime; // Speed in kbps

        setFileUploadSpeed(speed.toFixed(2)); // Update speed state

        // Update cached values
        lastUpdateTimeRef.current = currentTime;
        lastPercentageRef.current = percentage;
      }
    });

    mcumgr.onImageUploadFinished(() => {
      setFileStatus("Upload complete");
      setFileUploadSpeed(null); // Reset speed
      console.log("File upload finished", fileInfos);
      const fileInfo = fileInfos.find((file) => !file.isUploaded && !file.isUploading);
      if (fileInfo) {
        doFwUpload(fileInfo);
      } else {
        console.log("All files uploaded");
        mcumgr.cmdImageState();
        promptForConfirmationOfFwImages();
      }

      setFileInfos((prevFileInfos) =>
        prevFileInfos.map((file) =>
          file.isUploading ? { ...file, isUploaded: true, isUploading: false } : file,
        ),
      );
    });

    mcumgr.onFsUploadFinished((ok) => {
      if (ok) {
        setFileSystemUploadStatus("Upload complete");
      } else {
        setFileSystemUploadStatus("Upload failed");
      }
      setFileSystemUploadProgress(null);
    });

    mcumgr.onMessage(({ group, id, data }) => {
      switch (group) {
        case MGMT_GROUP_ID_OS:
          switch (id) {
            case OS_MGMT_ID_ECHO:
              alert(data.r);
              break;
            case OS_MGMT_ID_TASKSTAT:
              console.table(data.tasks);
              break;
            case OS_MGMT_ID_MPSTAT:
              console.log(data);
              break;
          }
          break;
        case MGMT_GROUP_ID_IMAGE:
          switch (id) {
            case IMG_MGMT_ID_STATE:
              setImages(data.images || []);
              if (!data.images) {
                return;
              }
              fileInfos.forEach((fileInfo) => {
                const matchingImage = data.images.find(
                  (image) =>
                    image.image === fileInfo.imageNumber &&
                    hashArrayToString(image.hash) === fileInfo.hash,
                );
                if (matchingImage) {
                  setFileInfos((prevFileInfos) =>
                    prevFileInfos.map((file) =>
                      file === fileInfo
                        ? {
                            ...file,
                            isUploaded: false,
                            mcuMgrImageStatus: `Active: ${matchingImage.active}, Pending: ${matchingImage.pending}, Slot: ${matchingImage.slot}`,
                          }
                        : file,
                    ),
                  );
                }
              });
              break;
          }
          break;
        default:
          console.log("Unknown group");
          break;
      }
    });
  }, [mcumgr, fileInfos]);

  const handleDeviceNameChange = (e) => {
    const name = e.target.value;
    setDeviceName(name);
    localStorage.setItem("deviceName", name);
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
      setFileStatus("Ready to upload");
    } catch (error) {
      console.error("Error reading file:", error);
      setFileStatus("Invalid image file");
    }
  };

  const fetchAndSetFirmwares = async (numToFetch) => {
    setIsFetchingFirmwares(true);
    const fetchedFirmwares = await fetchArtifacts(numToFetch);
    console.log("Fetched firmwares:", fetchedFirmwares);
    if (fetchedFirmwares) {
      setFirmwares(fetchedFirmwares);
    }
    setIsFetchingFirmwares(false);
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
      setFileStatus("Ready to upload");
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

    const reader = new FileReader();
    reader.onload = async () => {
      if (selectedUploadFile.type === "application/zip") {
        await handleZipFileUpload(selectedUploadFile, reader);
        setFileStatus("Using zip file");
      } else {
        await handleBinFileUpload(selectedUploadFile, reader);
      }
      fileInputRef.current.value = null;
      mcumgr.cmdImageState();
    };
    reader.readAsArrayBuffer(selectedUploadFile);
  };

  const promptForConfirmationOfFwImages = async () => {
    const userConfirmed = window.confirm(
      "Do you want to confirm the images and restart the device? After confirming, you need to restart the device to apply the changes.",
    );
    if (userConfirmed) {
      for (const fileInfo of fileInfos) {
        const hashArray = Uint8Array.from(
          fileInfo.hash.match(/.{1,2}/g).map((byte) => parseInt(byte, 16)),
        );
        await mcumgr.cmdImageTest(hashArray);
      }
    }
  };

  const doFwUpload = async (fileInfo) => {
    if (!fileInfo) {
      setFileStatus("Ready to upload");
      return;
    }

    setIsFileUploadInProgress(true);
    setFileUploadPercentage(0);
    setFileUploadSpeed(null); // Reset speed
    setFileStatus("Uploading...");

    // Cache file size and start time
    fileSizeRef.current = fileInfo.fileSize;
    uploadStartTimeRef.current = Date.now();
    lastUpdateTimeRef.current = Date.now();
    lastPercentageRef.current = 0;

    try {
      await mcumgr.cmdUpload(fileInfo.fileData, fileInfo.imageNumber);
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

  const handleFileSystemUpload = async (event) => {
    const file = event.target.files[0];
    if (!file) {
      setFileSystemUploadStatus("No file selected");
      return;
    }

    console.log("Selected file:", file);

    const reader = new FileReader();
    reader.onload = async (e) => {
      const filePath = "/S/full_fs";

      setFileSystemUploadStatus("Uploading...");
      setFileSystemUploadProgress(0);

      const mcumgr = mcumgrRef.current;

      mcumgr.onFsUploadProgress(({ percentage }) => {
        setFileSystemUploadProgress(percentage);
      });

      try {
        await mcumgr.cmdUploadFileSystemImage(reader.result, filePath);
        setFileSystemUploadStatus("Upload started");
      } catch (error) {
        console.error("Filesystem upload failed:", error);
        setFileSystemUploadStatus("Upload failed");
        setFileSystemUploadProgress(null);
      }
      fileFsInputRef.current.value = null;
    };

    reader.readAsArrayBuffer(file);
  };

  const handleConnect = async () => {
    const filters = deviceName ? [{ namePrefix: deviceName }] : null;
    await mcumgr.connect(filters);
  };

  const handleDisconnect = async () => {
    await mcumgr.disconnect();
  };

  const renderFileSystemUpload = () => (
    <div>
      <h2>Upload Filesystem</h2>
      <input
        type="file"
        ref={fileFsInputRef}
        onChange={handleFileSystemUpload}
      />
      <p>Status: {fileSystemUploadStatus}</p>
      {fileSystemUploadProgress !== null && (
        <p>Progress: {fileSystemUploadProgress}%</p>
      )}
    </div>
  );

  const renderScreen = () => {
    switch (screen) {
      case "initial":
        return renderBluetoothConnect();
      case "connecting":
        return (
          <div className="content">
            <div className="alert alert-primary" role="alert">
              Connecting...
            </div>
          </div>
        );
      case "connected":
        return renderConnectedImageManagement();
      case "connected_advanced":
        return renderAdvancedConnectedImageManagement();
      default:
        return null;
    }

    function renderTopButtonRow() {
      return (
        <div style={{ display: "flex", gap: "10px", marginBottom: "10px" }}>
          <button
            className="btn btn-primary"
            onClick={() => {
              const message = prompt("Enter a text message to send", "Hello World!");
              mcumgr.smpEcho(message);
            }}
          >
            <i className="bi-soundwave"></i> Echo
          </button>
          <button className="btn btn-danger" onClick={handleDisconnect}>
            <i className="bi-x-square"></i> Disconnect
          </button>
          <button className="btn btn-warning" onClick={() => mcumgr.cmdReset()}>
            <i className="bi-arrow-clockwise"></i> Reset
          </button>
          <button className="btn btn-info" onClick={() => setScreen("connected_advanced")}>
            <i className="bi-arrow-clockwise"></i> Use advanzed mode
          </button>
        </div>
      );
    }

    function renderUploadImageSection(onFileUploiadCallback) {
      return (
        <div>
          <div className="form-group" style={{ marginBottom: "10px" }}>
            <input
              type="file"
              className="form-control"
              id="file-image"
              onChange={handleFileChange}
              disabled={isFileUploadInProgress}
              ref={fileInputRef}
            />
          </div>
          <div
            className="image"
            style={{
              padding: "10px",
              border: "1px solid #ccc",
              borderRadius: "5px",
            }}
          >
            <div className="form-group" style={{ marginBottom: "10px" }}>
              <div id="file-status">{fileStatus}</div>
              <div style={{ display: "flex", gap: "10px", flexWrap: "wrap" }}>
                {fileInfos &&
                  fileInfos.map((fileInfo, index) => (
                    <table
                      id="file-info"
                      key={index}
                      style={{
                        backgroundColor: "#1B1B1D",
                        border: "1px solid #ccc",
                        borderRadius: "5px",
                        padding: "10px",
                      }}
                    >
                      <tbody>
                        <tr>
                          <th>Name</th>
                          <td>{fileInfo.name}</td>
                        </tr>
                        <tr>
                          <th>Image number</th>
                          <td>{fileInfo.imageNumber}</td>
                        </tr>
                        <tr>
                          <th>Version</th>
                          <td>{fileInfo.version}</td>
                        </tr>
                        <tr>
                          <th>Hash</th>
                          <td>
                            <div
                              style={{
                                whiteSpace: "nowrap",
                                overflow: "hidden",
                                textOverflow: "ellipsis",
                                maxWidth: "150px",
                                cursor: "pointer",
                              }}
                              title={fileInfo.hash}
                              onClick={() => {
                                navigator.clipboard.writeText(fileInfo.hash);
                                alert("Hash copied to clipboard!");
                              }}
                            >
                              {fileInfo.hash}
                            </div>
                          </td>
                        </tr>
                        <tr>
                          <th>File Size</th>
                          <td>{fileInfo.fileSize} bytes</td>
                        </tr>
                        <tr>
                          <th>Upload Status</th>
                          <td>
                            {fileInfo.isUploading && (
                              <div style={{ width: "100%", borderRadius: "5px" }}>
                                <div
                                  style={{
                                    width: `${fileUploadPercentage}%`,
                                    backgroundColor: "#4caf50",
                                    height: "10px",
                                    borderRadius: "5px",
                                  }}
                                ></div>
                                <span style={{ fontSize: "16px" }}>{fileUploadPercentage}% Speed: {fileUploadSpeed} kbps</span>
                              </div>
                            )}
                            {fileInfo.isUploaded && (
                              <div style={{ width: "100%", borderRadius: "5px" }}>
                                <div
                                  style={{
                                    width: "100%",
                                    backgroundColor: "#4caf50",
                                    height: "10px",
                                    borderRadius: "5px",
                                  }}
                                ></div>
                                <span style={{ fontSize: "16px" }}>
                                  {fileInfo.mcuMgrImageStatus}
                                </span>
                              </div>
                            )}
                          </td>
                        </tr>
                      </tbody>
                    </table>
                  ))}
              </div>
            </div>
            <button
              className="btn btn-secondary"
              disabled={fileInfos.length === 0 || isFileUploadInProgress}
              onClick={onFileUploiadCallback}
            >
              <i className="bi-upload"></i> Upload
            </button>
            <button className="btn btn-danger" onClick={() => mcumgr.cmdImageErase()}>
              <i className="bi-upload"></i> Erase files
            </button>
            <button className="btn btn-primary" onClick={() => promptForConfirmationOfFwImages()}>
              <i className="bi-upload"></i> Confirm files
            </button>
          </div>
        </div>
      );
    }

    function Highlight({ children, color }) {
      return (
        <span
          style={{
            backgroundColor: color,
            borderRadius: "2px",
            color: "#fff",
            padding: "0.2rem",
          }}
        >
          {children}
        </span>
      );
    }

    function renderPrebuiltFirmwares() {
      return (
        <div>
          <details>
            <summary>Help</summary>
            <Admonition type="note" icon="" title="">
              If you don't have a firmware, you can fetch one from here.<br></br>
              You will see the last builds in CI. Pick the build you want, for example last build on
              the <strong>main branch.</strong>
              <br></br>
              Then choose the artifact corresponding to your watch version (4 or 5) and click on it
              to download the firmware.<br></br>
            </Admonition>
          </details>
          <Admonition type="info" icon="" title="">
            Once downloaded you need to unzip it and upload the <strong>dfu_application.zip</strong>{" "}
            file below.<br></br>
          </Admonition>
          <button
            className="btn btn-success"
            onClick={() => fetchAndSetFirmwares(6)}
            style={{ marginBottom: "10px" }}
          >
            Fetch Prebuilt Firmwares
          </button>
          {isFetchingFirmwares ? (
            <p>Loading...</p>
          ) : firmwares.length === 0 ? (
            <p>No prebuilt firmwares fetched.</p>
          ) : (
            <div>
              {firmwares.map((firmware, index) => (
                <details key={index} style={{ marginBottom: "10px" }}>
                  <summary>
                    Branch <Highlight color="#1877F2">{firmware.branch}</Highlight> by user{" "}
                    <Highlight color="green">{firmware.user}</Highlight>
                  </summary>
                  <div style={{ padding: "10px", border: "1px solid #ccc", borderRadius: "5px" }}>
                    <p>
                      <strong>Artifacts:</strong>
                    </p>
                    <ul>
                      {firmware.artifacts.map((artifact, artifactIndex) => (
                        <li key={artifactIndex}>
                          <button
                            className="btn btn-primary"
                            onClick={() => downloadAndFlashFirmware(firmware.runId, artifact.id)}
                            style={{ width: "50%", marginBottom: "10px" }}
                          >
                            {artifact.name}
                          </button>
                        </li>
                      ))}
                    </ul>
                  </div>
                </details>
              ))}
            </div>
          )}
        </div>
      );
    }

    async function downloadAndFlashFirmware(runId, artifactId) {
      try {
        setFileStatus("Downloading firmware...");
        const downloadUrl = `https://github.com/ZSWatch/ZSWatch/actions/runs/${runId}/artifacts/${artifactId}`;
        console.log("URL", downloadUrl);

        setFileStatus("Firmware download started. Please check your browser's downloads.");

        // Due to API Token is required to download the artifact, we need to let the browser download it manually
        // And then ask user to upload the zip file
        // Redirect the browser to the download URL
        window.location.href = downloadUrl;
      } catch (error) {
        console.error("Error downloading or flashing firmware:", error);
        setFileStatus("Failed to download firmware.");
      }
    }

    function renderConnectedImageManagement() {
      return (
        <div className="content">
          <div className="container">
            {renderTopButtonRow()}
            <hr />
            <h3>Prebuilt firmwares</h3>
            {renderPrebuiltFirmwares()}
            <h3>Image Upload</h3>
            <Admonition type="tip" icon="💡" title="How to...">
              Upload the <strong>dfu_application.zip</strong> previosuly downloaded.<br></br>
              Then press <strong>Upload</strong> button to upload the firmware.<br></br>
            </Admonition>
            {renderUploadImageSection(handleFileUpload)}
            <details>
              <summary>Troubleshooting</summary>
              <Admonition type="info" icon="" title="">
                If upload hangs, or doesn't start then try again after resetting the watch.
                If the watch FW is not working this way of uploading will not work as it relies on Bluetooth and application working.
                To fix this you need to use the <strong>mcumgr over USB</strong> method.
              </Admonition>
            </details>
{renderFileSystemUpload()}
          </div>
        </div>
      );
    }

    function renderAdvancedConnectedImageManagement() {
      return (
        <div className="content">
          <div className="container">
            <Admonition type="tip" icon="💡" title="How to...">
              Upload the all binary files or .zip containing all binary files<br></br>
            </Admonition>
            {renderTopButtonRow()}
            <hr />
            <h3>Images</h3>
            <div
              id="image-list"
              style={{
                display: "flex",
                flexWrap: "wrap",
                gap: "10px",
                marginBottom: "10px",
              }}
            >
              {images.map((image, index) => (
                <div key={index} className={`image ${image.active ? "active" : "standby"}`}>
                  <h2>
                    {image.active ? "active" : "standby"} {imageNameMapping[image.image]} Slot{" "}
                    {image.slot}
                  </h2>
                  <table>
                    <tr>
                      <th>Image index (for actions)</th>
                      <td>{index}</td>
                    </tr>
                    <tr>
                      <th>Image number</th>
                      <td>{image.image}</td>
                    </tr>
                    <tr>
                      <th>slot</th>
                      <td>{image.slot}</td>
                    </tr>
                    <tr>
                      <th>Version</th>
                      <td>v{image.version}</td>
                    </tr>
                    <tr>
                      <th>Bootable</th>
                      <td>{image.bootable ? "true" : "false"}</td>
                    </tr>
                    <tr>
                      <th>Confirmed</th>
                      <td>{image.confirmed ? "true" : "false"}</td>
                    </tr>
                    <tr>
                      <th>Pending</th>
                      <td>{image.pending ? "true" : "false"}</td>
                    </tr>
                    <tr>
                      <th>Hash</th>
                      <td>
                        <div
                          style={{
                            whiteSpace: "nowrap",
                            overflow: "hidden",
                            textOverflow: "ellipsis",
                            maxWidth: "150px",
                            cursor: "pointer",
                          }}
                          title={hashArrayToString(image.hash)}
                          onClick={() => {
                            navigator.clipboard.writeText(hashArrayToString(image.hash));
                            alert("Hash copied to clipboard!");
                          }}
                        >
                          {Array.from(image.hash)
                            .map((byte) => byte.toString(16).padStart(2, "0"))
                            .join("")}
                        </div>
                      </td>
                    </tr>
                  </table>
                </div>
              ))}
            </div>
            <div style={{ display: "flex", gap: "10px", marginBottom: "10px" }}>
              <button
                id="button-image-state"
                type="submit"
                className="btn btn-info"
                onClick={() => mcumgr.cmdImageState()}
              >
                <i className="bi-arrow-down-circle"></i> Refresh
              </button>
              <button
                type="submit"
                className="btn btn-warning"
                onClick={() => mcumgr.cmdImageErase()}
              >
                <i className="bi-eraser-fill"></i> Erase
              </button>
              <button
                type="submit"
                className="btn btn-success"
                onClick={() => {
                  const imageIndex = parseInt(prompt("Enter the image index:", "0"), 10);
                  if (!isNaN(imageIndex) && images[imageIndex]?.pending === false) {
                    mcumgr.cmdImageTest(images[imageIndex].hash);
                  }
                }}
              >
                <i className="bi-question-square"></i> Test
              </button>
              <button
                id="button-confirm"
                type="submit"
                className="btn btn-success"
                onClick={() => {
                  const imageIndex = parseInt(prompt("Enter the image index:", "0"), 10);
                  if (!isNaN(imageIndex) && images[imageIndex]?.confirmed === false) {
                    mcumgr.cmdImageConfirm(images[imageIndex].hash);
                  }
                }}
              >
                <i className="bi-check2-square"></i> Confirm
              </button>
            </div>
            <hr />
            <h3>Image Upload</h3>
            {renderUploadImageSection(handleFileUpload)}
            {renderFileSystemUpload()}
          </div>
        </div>
      );
    }

    function renderBluetoothConnect() {
      return (
        <div className="content">
          <Admonition type={bluetoothAvailable ? "tip" : "warning"} icon="💡">
            {bluetoothAvailable
              ? "Bluetooth is available in your browser."
              : `This tool is compatible with desktops (or laptops) with the
                  latest Chrome, Opera and Edge browsers, and working Bluetooth connection (most
                  laptops have them these days). You can also try it from Chrome on Android, or Bluefy
                  on iOS/iPadOS.`}
          </Admonition>
          <div id="connect-block">
            <div className="form-group form-inline">
              <div className="col-auto">
                <input
                  style={{ padding: "5px", fontSize: "1em", marginBottom: "10px" }}
                  id="device-name-input"
                  type="text"
                  className="form-control"
                  placeholder="Device name (optional)"
                  value={deviceName}
                  onChange={handleDeviceNameChange}
                />
              </div>
              <div className="col-auto">
                <button
                  id="button-connect"
                  type="submit"
                  className="btn btn-primary"
                  onClick={handleConnect}
                >
                  <i className="bi-bluetooth"></i> Connect
                </button>
              </div>
            </div>
          </div>
        </div>
      );
    }
  };

  return (
    <Layout>
      <div>
        <div className="container">
          <h1>FW Update over BLE</h1>
          {renderScreen()}
        </div>
      </div>
    </Layout>
  );
};

export default FirmwareUpdateApp;
