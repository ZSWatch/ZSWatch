import { useState, useEffect, useRef, useCallback } from "react";
import { useLocation } from "@docusaurus/router";
import {
  MGMT_GROUP_ID_IMAGE,
  IMG_MGMT_ID_STATE,
  MGMT_GROUP_ID_OS,
  OS_MGMT_ID_ECHO,
  OS_MGMT_ID_TASKSTAT,
  OS_MGMT_ID_MPSTAT,
  MGMT_GROUP_ID_SHELL,
} from "../../mcumgr-web/js/mcumgr";
import { SCREENS } from "../FirmwareUpdate/constants";

export const useMCUConnection = (mcumgr) => {
  const location = useLocation();
  const [deviceName, setDeviceName] = useState("ZSWatch");
  const [screen, setScreen] = useState(SCREENS.INITIAL);
  const [images, setImages] = useState([]);
  const [bluetoothAvailable, setBluetoothAvailable] = useState(false);
  const [serialAvailable, setSerialAvailable] = useState(false);
  const [transport, setTransport] = useState("serial");
  const [isSerialRecoveryMode, setIsSerialRecoveryMode] = useState(true);
  const [deviceInfo, setDeviceInfo] = useState({});
  const [connectionStatus, setConnectionStatus] = useState("");
  const [connectionError, setConnectionError] = useState("");
  const [preparingFirmware, setPreparingFirmware] = useState(false);
  const preparation = useRef(null);

  useEffect(() => {
    if (!/\/(update|tools)\/?$/.test(location.pathname) || screen !== SCREENS.CONNECTED || transport !== "ble") return;
    let disposed = false;
    const keepAlive = () => {
      if (disposed || document.visibilityState !== "visible" || preparation.current || !mcumgr.shouldKeepSmpAlive()) return;
      setPreparingFirmware(true);
      preparation.current = mcumgr.keepSmpAlive().then(() => {
        if (!disposed) setConnectionError("");
      }).catch((error) => {
        if (!disposed && mcumgr.isConnected()) setConnectionError(error.message);
      }).finally(() => { preparation.current = null; setPreparingFirmware(false); });
    };
    const timer = setInterval(keepAlive, 60000);
    document.addEventListener("visibilitychange", keepAlive);
    keepAlive();
    return () => { disposed = true; clearInterval(timer); document.removeEventListener("visibilitychange", keepAlive); };
  }, [mcumgr, location.pathname, screen, transport]);

  const shellListenersRef = useRef(new Set());

  const notifyShellListeners = useCallback((payload) => {
    shellListenersRef.current.forEach(listener => {
      try {
        listener(payload);
      } catch (error) {
        console.error('Shell listener callback failed', error);
      }
    });
  }, []);

  const registerShellListener = useCallback((listener) => {
    if (typeof listener !== 'function') {
      return () => {};
    }

    shellListenersRef.current.add(listener);
    return () => {
      shellListenersRef.current.delete(listener);
    };
  }, []);

  useEffect(() => {
    setSerialAvailable(!!navigator.serial);
    // Check Bluetooth availability
    const checkBluetoothAvailability = async () => {
      try {
        const available = await navigator.bluetooth?.getAvailability();
        setBluetoothAvailable(!!available);
      } catch {
        setBluetoothAvailable(false);
      }
    };

    checkBluetoothAvailability();
  }, []);

  useEffect(() => {
    // Set up mcumgr event listeners
    mcumgr.onConnecting(() => {
      setScreen(SCREENS.CONNECTING);
      setConnectionError("");
      // Reset mode detection when starting a new connection
      setIsSerialRecoveryMode(true);
    });

    mcumgr.onDeviceInfo(setDeviceInfo);
    mcumgr.onStatus(setConnectionStatus);
    mcumgr.onConnect(async ({ reconnecting } = {}) => {
      if (reconnecting) return; // The update operation requests and verifies images.
      try {
        const result = await mcumgr.request(0, MGMT_GROUP_ID_IMAGE, IMG_MGMT_ID_STATE);
        const first = result.images?.[0];
        const recovery = !first || !("confirmed" in first && "pending" in first && "hash" in first);
        await mcumgr.readDeviceInfo(recovery);
        setScreen(SCREENS.CONNECTED);
        setConnectionStatus("Firmware update service ready.");
      } catch (error) {
        setConnectionError(error.message);
        await mcumgr.disconnect();
      }
    });

    mcumgr.onDisconnect(() => {
      setScreen(SCREENS.INITIAL);
      // Reset mode detection on disconnect
      setIsSerialRecoveryMode(true);
      // Clear images on disconnect
      setImages([]);
      setDeviceInfo({});
    });

    mcumgr.onMessage(({ group, id, data, op }) => {
      console.log("MCU Manager message:", { group, id, data });
      switch (group) {
        case MGMT_GROUP_ID_OS:
          switch (id) {
            case OS_MGMT_ID_ECHO:
              if (data?.rc && data.rc !== 0) {
                alert(`Echo failed: Error code ${data.rc}`);
              } else {
                alert(data?.r ?? "");
              }
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
              const receivedImages = data.images || [];
              setImages(receivedImages);

              // Don't consider connected until we receive image state as that is how we detect
              // if device is in serial recovery mode or full application mode
              console.log("Received image state:", receivedImages);
              if (mcumgr._restarting) setScreen(SCREENS.CONNECTED);

              // Detect if device is in serial recovery mode (MCUBoot) or full application mode
              // Different modes return different levels of information:
              // - Full application mode: provides confirmed, pending, active, hash, slot
              // - Serial recovery mode (MCUBoot): only provides basic image info, different image slots
              if (receivedImages.length > 0) {
                const firstImage = receivedImages[0];
                // Device is in full application mode if it provides confirmed/pending status
                // If false, device is in MCUBoot serial recovery mode
                const isAppMode =
                  firstImage.hasOwnProperty('confirmed') &&
                  firstImage.hasOwnProperty('pending') &&
                  firstImage.hasOwnProperty('hash');
                setIsSerialRecoveryMode(!isAppMode);
                console.log("Device mode detected:", isAppMode ? "Full Application" : "Serial Recovery (MCUBoot)");
              }
              break;
          }
          break;
        case MGMT_GROUP_ID_SHELL:
          notifyShellListeners({ group, id, data, op });
          break;
        default:
          console.log("Unknown group");
          break;
      }
    });
  }, [mcumgr, notifyShellListeners]);

  useEffect(() => {
    mcumgr.setTransport(transport);
  }, [transport, mcumgr]);

  useEffect(() => {
    // Adjust chunk timeout based on device mode and transport
    let timeout;
    if (transport === "serial") {
      // For some reason serial updates in application often times out, even with long timeouts
      // so we use a shorter timeout to not wait too long
      timeout = isSerialRecoveryMode ? 500 : 100;
    } else {
      timeout = 500;
    }
    mcumgr.setChunkTimeout(timeout);
    console.log(`Chunk timeout set to ${timeout}ms for ${transport.toUpperCase()} transport in ${isSerialRecoveryMode ? 'Serial Recovery' : 'Application'} mode`);
  }, [isSerialRecoveryMode, transport, mcumgr]);

  const handleTransportChange = value => {
    if (mcumgr.isConnected()) return;
    if (value !== "ble" && value !== "serial") {
      return;
    }
    setTransport(value);
    mcumgr.setTransport(value);
  };

  const handleDeviceNameChange = name => {
    setDeviceName(name);
  };

  const handleConnect = async ({ transport: requestedTransport = transport, deviceName: requestedName = deviceName } = {}) => {
    if (mcumgr.isConnected()) return true;
    setTransport(requestedTransport);
    setConnectionError("");
    setConnectionStatus("Connecting to watch...");
    setDeviceInfo({});
    try {
      if (requestedTransport === "serial") {
        await mcumgr.connect({ transport: "serial" });
        if (!mcumgr.isConnected()) throw new Error("Could not open the USB serial port. Reconnect the watch and retry.");
      } else {
        const prefix = requestedName.trim();
        await mcumgr.connect({ transport: "ble", filters: prefix ? [{ namePrefix: prefix }] : null });
      }
    } catch (error) {
      setConnectionError(error.message);
      setScreen(SCREENS.INITIAL);
      return false;
    }
    return mcumgr.isConnected();
  };

  const refreshImages = () => {
    if (!mcumgr.isConnected()) return Promise.resolve();
    if (preparation.current) return preparation.current;
    setPreparingFirmware(true);
    setConnectionError("");
    preparation.current = (async () => {
      try {
        await mcumgr.ensureUpdateReady();
        await mcumgr.request(0, MGMT_GROUP_ID_IMAGE, IMG_MGMT_ID_STATE);
      } catch (error) { setConnectionError(error.message); }
      finally { preparation.current = null; setPreparingFirmware(false); }
    })();
    return preparation.current;
  };

  const handleDisconnect = async () => {
    await mcumgr.disconnect();
  };

  return {
    mcumgr,
    deviceName,
    screen,
    images,
    deviceInfo,
    connectionStatus,
    connectionError,
    bluetoothAvailable,
    serialAvailable,
    transport,
    isSerialRecoveryMode,
    setScreen,
    handleTransportChange,
    handleDeviceNameChange,
    handleConnect,
    handleDisconnect,
    registerShellListener,
    refreshImages,
    preparingFirmware,
  };
};
