import Layout from "@theme/Layout";
import React, { useState, useEffect, useRef } from "react";
import Admonition from '@theme/Admonition';
import "./mcumgr.css";
import MCUManager from "../mcumgr-web/js/mcumgr";

import {
  MGMT_GROUP_ID_IMAGE,
  IMG_MGMT_ID_STATE,
  MGMT_GROUP_ID_OS,
  OS_MGMT_ID_ECHO,
  OS_MGMT_ID_TASKSTAT,
  OS_MGMT_ID_MPSTAT,
} from "../mcumgr-web/js/mcumgr";

const imageNameMapping = {
  0: 'App Internal',
  1: 'Net Core',
  2: 'App External (XIP)',
}

const ZSWatchApp = () => {
  const [deviceName, setDeviceName] = useState(
    localStorage.getItem("deviceName") || "Connect your device",
  );
  const [screen, setScreen] = useState("initial");
  const [images, setImages] = useState([]);
  const [file, setFile] = useState(null);
  const [fileData, setFileData] = useState(null);
  const [fileInfo, setFileInfo] = useState(null);
  const [fileStatus, setFileStatus] = useState("Select image file (.img)");
  const [bluetoothAvailable, setBluetoothAvailable] = useState(false);

  // Use useRef to create a stable mcumgr instance
  const mcumgrRef = useRef(new MCUManager());
  const mcumgr = mcumgrRef.current;

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
    });

    mcumgr.onImageUploadFinished(() => {
      setFileStatus("Upload complete");
      setFileInfo(null);
      setFile(null);
      mcumgr.cmdImageState();
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
              break;
          }
          break;
        default:
          console.log("Unknown group");
          break;
      }
    });
  }, [mcumgr]);

  const handleDeviceNameChange = (e) => {
    const name = e.target.value;
    setDeviceName(name);
    localStorage.setItem("deviceName", name);
  };

  const handleFileChange = (e) => {
    const selectedFile = e.target.files[0];
    setFile(selectedFile);
    const reader = new FileReader();
    reader.onload = async () => {
      console.log("File loaded:", reader.result);
      setFileData(reader.result);
      try {
        const info = await mcumgr.imageInfo(reader.result);
        setFileInfo({
          version: info.version,
          hash: info.hash,
          fileSize: reader.result.byteLength,
        });
        setFileStatus("Ready to upload");
      } catch (error) {
        setFileStatus("Invalid image file");
      }
    };
    reader.readAsArrayBuffer(selectedFile);
  };

  const handleFileUpload = async () => {
    setFileStatus("Uploading...");
    const imageNumber = parseInt(prompt("Enter the image number:", "0"), 10);
    if (isNaN(imageNumber) || imageNumber < 0) {
      alert("Invalid image number. Please enter a non-negative integer.");
      setFileStatus("Ready to upload");
      return;
    }
    try {
      await mcumgr.cmdUpload(fileData, imageNumber);
    } catch (error) {
      setFileStatus("Upload failed");
    }
  };

  const handleConnect = async () => {
    const filters = deviceName ? [{ namePrefix: deviceName }] : null;
    await mcumgr.connect(filters);
  };

  const handleDisconnect = async () => {
    await mcumgr.disconnect();
  };

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
      default:
        return null;
    }

    function renderConnectedImageManagement() {
      return <div className="content">
        <div className="container">
          <div style={{ display: "flex", gap: "10px", marginBottom: "10px" }}>
            <button
              id="button-echo"
              type="submit"
              className="btn btn-primary"
              onClick={() => {
                const message = prompt("Enter a text message to send", "Hello World!");
                mcumgr.smpEcho(message);
              } }
            >
              <i className="bi-soundwave"></i> Echo
            </button>
            <button
              id="button-disconnect"
              type="submit"
              className="btn btn-secondary"
              onClick={handleDisconnect}
            >
              <i className="bi-x-square"></i> Disconnect
            </button>
            <button
              id="button-reset"
              type="submit"
              className="btn btn-info"
              onClick={() => mcumgr.cmdReset()}
            >
              <i className="bi-arrow-clockwise"></i> Reset
            </button>
          </div>
          <Admonition type="tip" icon="💡" title="How to...">
            - Extract the files in the FW .zip. Upload one by one to ZSWatch.<br></br>
            - Once done you should have 3 files marked "standby". <br></br>
            - Next press the "Confirm button and input the image index of each image.<br></br>
            - Now press reset and ZSWatch will reboot with the new firmware (will take some time).<br></br>
          </Admonition>
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
                <h2>{image.active ? "active" : "standby"} {imageNameMapping[image.image]} Slot {image.slot}</h2>
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
                        title={Array.from(image.hash)
                          .map((byte) => byte.toString(16).padStart(2, "0"))
                          .join("")}
                        onClick={() => {
                          navigator.clipboard.writeText(
                            Array.from(image.hash)
                              .map((byte) => byte.toString(16).padStart(2, "0"))
                              .join("")
                          );
                          alert("Hash copied to clipboard!");
                        } }
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
              id="button-erase"
              type="submit"
              className="btn btn-warning"
              onClick={() => mcumgr.cmdImageErase()}
            >
              <i className="bi-eraser-fill"></i> Erase
            </button>
            <button
              id="button-test"
              type="submit"
              className="btn btn-primary"
              onClick={() => {
                const imageIndex = parseInt(prompt("Enter the image index:", "0"), 10);
                if (!isNaN(imageIndex) && images[imageIndex]?.pending === false) {
                  mcumgr.cmdImageTest(images[imageIndex].hash);
                }
              } }
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
              } }
            >
              <i className="bi-check2-square"></i> Confirm
            </button>
          </div>
          <hr />
          <h3>Image Upload</h3>
          <div className="form-group" style={{ marginBottom: "10px" }}>
            <input
              type="file"
              className="form-control"
              id="file-image"
              onChange={handleFileChange} />
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
              {fileInfo && (
                <table id="file-info">
                  <tbody>
                    <tr>
                      <th>Version</th>
                      <td>{fileInfo.version}</td>
                    </tr>
                    <tr>
                      <th>Hash</th>
                      <td>{fileInfo.hash}</td>
                    </tr>
                    <tr>
                      <th>File Size</th>
                      <td>{fileInfo.fileSize} bytes</td>
                    </tr>
                  </tbody>
                </table>
              )}
            </div>
            <button
              className="btn btn-primary"
              id="file-upload"
              disabled={!file}
              onClick={handleFileUpload}
            >
              <i className="bi-upload"></i> Upload
            </button>
          </div>
        </div>
      </div>;
    }

    function renderBluetoothConnect() {
      return <div className="content">
        <div
          className={`alert ${
            bluetoothAvailable ? "alert-success" : "alert-danger"
          }`}
          role="alert"
        >
          <b>
            {bluetoothAvailable
              ? "Bluetooth is available in your browser."
              : `This tool is compatible with desktops (or laptops) with the
                  <b>latest Chrome, Opera and Edge</b> browsers, and working Bluetooth connection (most
                  laptops have them these days). You can also try it from Chrome on Android, or Bluefy
                  on iOS/iPadOS.`}
          </b>
        </div>
        <div id="connect-block">
          <div className="form-group form-inline">
            <div className="col-auto">
              <input
                id="device-name-input"
                type="text"
                className="form-control"
                placeholder="Device name (optional)"
                value={deviceName}
                onChange={handleDeviceNameChange} />
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
      </div>;
    }
  };

  return (
    <Layout>
      <div>
        <main role="main" className="container">
          <div className="starter-template">
            <h1>FW Update over BLE</h1>
            {renderScreen()}
          </div>
        </main>
      </div>
    </Layout>
  );
};

export default ZSWatchApp;
