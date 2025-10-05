import { useState, useEffect, useRef, useCallback } from "react";
import MCUManager from "../../../mcumgr-web/js/mcumgr";
import {
  MGMT_GROUP_ID_IMAGE,
  IMG_MGMT_ID_STATE,
  MGMT_GROUP_ID_OS,
  OS_MGMT_ID_ECHO,
  OS_MGMT_ID_TASKSTAT,
  OS_MGMT_ID_MPSTAT,
  MGMT_GROUP_ID_SHELL,
} from "../../../mcumgr-web/js/mcumgr";
import { SCREENS } from "../constants";

export const useMCUManager = () => {
  const [deviceName, setDeviceName] = useState("ZSWatch");
  const [screen, setScreen] = useState(SCREENS.INITIAL);
  const [images, setImages] = useState([]);
  const [bluetoothAvailable, setBluetoothAvailable] = useState(false);
  const [serialAvailable, setSerialAvailable] = useState(!!navigator.serial);
  const [transport, setTransport] = useState("serial");
  const [isSerialRecoveryMode, setIsSerialRecoveryMode] = useState(true);

  // Use useRef to create a stable mcumgr instance
  const mcumgrRef = useRef(new MCUManager({ transport: "serial" }));
  const mcumgr = mcumgrRef.current;

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
      // Reset mode detection when starting a new connection
      setIsSerialRecoveryMode(true);
    });

    mcumgr.onConnect(() => {
      setDeviceName(prevName => mcumgr.name || prevName);
      mcumgr.cmdImageState();
    });

    mcumgr.onDisconnect(() => {
      setScreen(SCREENS.INITIAL);
      // Reset mode detection on disconnect
      setIsSerialRecoveryMode(true);
      // Clear images on disconnect
      setImages([]);
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
              setScreen(prevScreen =>
                prevScreen === SCREENS.CONNECTING ? SCREENS.CONNECTED : prevScreen
              );

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
    if (value !== "ble" && value !== "serial") {
      return;
    }
    setTransport(value);
    mcumgr.setTransport(value);
  };

  const handleDeviceNameChange = name => {
    setDeviceName(name);
  };

  const handleConnect = async () => {
    if (transport === "serial") {
      await mcumgr.connect({ transport: "serial" });
      return;
    }

    const filters = deviceName ? [{ namePrefix: deviceName }] : null;
    await mcumgr.connect({ transport: "ble", filters });
  };

  const handleDisconnect = async () => {
    await mcumgr.disconnect();
  };

  return {
    mcumgr,
    deviceName,
    screen,
    images,
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
  };
};
