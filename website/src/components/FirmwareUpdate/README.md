# FirmwareUpdate Component

```
src/components/FirmwareUpdate/
├── FirmwareUpdateApp.js          # Main component that orchestrates everything
├── index.js                      # Export file for easy importing
├── mcumgr.css                    # CSS styles for MCU manager components
├── constants.js                  # All constants and configuration
├── components/                   # UI components
│   ├── BluetoothConnect.js       # Connection interface
│   ├── ConnectingScreen.js       # Loading state component
│   ├── DeviceControls.js         # Device action buttons
│   ├── PrebuiltFirmwares.js      # GitHub artifacts fetching
│   ├── FileUploadSection.js      # Firmware file upload UI
│   └── FileSystemUpload.js       # Filesystem upload UI
├── hooks/                        # Custom React hooks for business logic
│   ├── useMCUManager.js          # MCU Manager connection & state
│   ├── useFileUpload.js          # File upload logic
│   ├── useFileSystemUpload.js    # Filesystem upload logic
│   └── useFirmwareManagement.js  # GitHub firmware fetching
└── utils/                        # Utility functions
    ├── helpers.js                # Generic helper functions
    └── githubApi.js              # GitHub API interactions
```
