import { useState, useEffect, useRef } from "react";
import MCUManager from "../../../mcumgr-web/js/mcumgr";
import {
  MGMT_GROUP_ID_IMAGE,
  IMG_MGMT_ID_STATE,
  MGMT_GROUP_ID_OS,
  OS_MGMT_ID_ECHO,
  OS_MGMT_ID_TASKSTAT,
  OS_MGMT_ID_MPSTAT,
} from "../../../mcumgr-web/js/mcumgr";
import { SCREENS } from "../constants";

export const useMCUManager = () => {
  const [deviceName, setDeviceName] = useState("ZSWatch");
  const [screen, setScreen] = useState(SCREENS.INITIAL);
  const [images, setImages] = useState([]);
  const [bluetoothAvailable, setBluetoothAvailable] = useState(false);
  const [serialAvailable, setSerialAvailable] = useState(!!navigator.serial);
  const [transport, setTransport] = useState("ble");

  // Use useRef to create a stable mcumgr instance
  const mcumgrRef = useRef(new MCUManager({ transport: "ble" }));
  const mcumgr = mcumgrRef.current;

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
    });

    mcumgr.onConnect(() => {
      setDeviceName(prevName => mcumgr.name || prevName);
      setScreen(SCREENS.CONNECTED);
      mcumgr.cmdImageState();
    });

    mcumgr.onDisconnect(() => {
      setScreen(SCREENS.INITIAL);
    });

    mcumgr.onMessage(({ group, id, data }) => {
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

  useEffect(() => {
    mcumgr.setTransport(transport);
  }, [transport, mcumgr]);

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
    setScreen,
    handleTransportChange,
    handleDeviceNameChange,
    handleConnect,
    handleDisconnect,
  };
};
