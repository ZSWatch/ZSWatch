import useDocusaurusContext from '@docusaurus/useDocusaurusContext';

export const imageNameMapping = {
  0: "App Internal",
  1: "Net Core", 
  2: "App External (XIP)",
};

export const binaryFileNameToImageId = {
  "app.internal.bin": 0,
  "ipc_radio.bin": 1,
  "app.external.bin": 2,
};

// Hook to get GitHub config with token from Docusaurus context
export const useGithubConfig = () => {
  const { siteConfig } = useDocusaurusContext();
  
  return {
    owner: "ZSWatch",
    repo: "ZSWatch", 
    token: siteConfig.customFields.githubToken || ""
  };
};

export const SCREENS = {
  INITIAL: "initial",
  CONNECTING: "connecting", 
  CONNECTED: "connected",
};
