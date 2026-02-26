import React from "react";
import BrowserOnly from "@docusaurus/BrowserOnly";

export default function UpdatePage() {
  return (
    <BrowserOnly fallback={<div style={{ padding: 40 }}>Loading...</div>}>
      {() => {
        const FirmwareUpdateApp = require("../components/FirmwareUpdate/FirmwareUpdateApp").default;
        return <FirmwareUpdateApp />;
      }}
    </BrowserOnly>
  );
}
