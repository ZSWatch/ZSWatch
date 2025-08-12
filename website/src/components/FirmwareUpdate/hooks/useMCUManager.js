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
import { hashArrayToString } from "../utils/helpers";

export const useMCUManager = () => {
  const [deviceName, setDeviceName] = useState('ZSWatch');
  const [screen, setScreen] = useState(SCREENS.INITIAL);
  const [images, setImages] = useState([]);
  const [bluetoothAvailable, setBluetoothAvailable] = useState(false);

  // Use useRef to create a stable mcumgr instance
  const mcumgrRef = useRef(new MCUManager());
  const mcumgr = mcumgrRef.current;

  useEffect(() => {
    const isBluetoothAvailable = navigator?.bluetooth?.getAvailability() || false;
    setBluetoothAvailable(isBluetoothAvailable);

    // Load saved device name
    const savedDeviceName = localStorage.getItem("deviceName");
    if (savedDeviceName) {
      setDeviceName(savedDeviceName);
    }

    // Set up mcumgr event listeners
    mcumgr.onConnecting(() => {
      setScreen(SCREENS.CONNECTING);
    });

    mcumgr.onConnect(() => {
      setDeviceName(mcumgr.name);
      setScreen(SCREENS.CONNECTED);
      mcumgr.cmdImageState();
    });

    mcumgr.onDisconnect(() => {
      setScreen(SCREENS.INITIAL);
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

  const handleDeviceNameChange = (name) => {
    setDeviceName(name);
    localStorage.setItem("deviceName", name);
  };

  const handleConnect = async () => {
    const filters = deviceName ? [{ namePrefix: deviceName }] : null;
    await mcumgr.connect(filters);
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
    setScreen,
    handleDeviceNameChange,
    handleConnect,
    handleDisconnect,
  };
};
