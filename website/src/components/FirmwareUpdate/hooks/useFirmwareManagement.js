import { useState } from "react";
import { fetchArtifacts, downloadFirmware } from "../utils/githubApi";

export const useFirmwareManagement = (githubConfig) => {
  const [firmwares, setFirmwares] = useState([]);
  const [isFetchingFirmwares, setIsFetchingFirmwares] = useState(false);

  const fetchAndSetFirmwares = async (numToFetch) => {
    setIsFetchingFirmwares(true);
    const fetchedFirmwares = await fetchArtifacts(githubConfig, numToFetch);
    console.log("Fetched firmwares:", fetchedFirmwares);
    if (fetchedFirmwares) {
      setFirmwares(fetchedFirmwares);
    }
    setIsFetchingFirmwares(false);
  };

  const downloadAndFlashFirmware = async (runId, artifactId, setFileStatus) => {
    try {
      setFileStatus("Downloading firmware...");
      setFileStatus("Firmware download started. Please check your browser's downloads.");
      downloadFirmware(githubConfig, runId, artifactId);
    } catch (error) {
      console.error("Error downloading or flashing firmware:", error);
      setFileStatus("Failed to download firmware.");
    }
  };

  return {
    firmwares,
    isFetchingFirmwares,
    fetchAndSetFirmwares,
    downloadAndFlashFirmware,
  };
};
