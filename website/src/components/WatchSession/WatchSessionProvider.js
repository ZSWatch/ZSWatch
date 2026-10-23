import React, { useRef } from "react";
import WatchToolsManager from "../WatchTools/WatchToolsManager";
import { useMCUConnection } from "./useMCUConnection";
import { useWatchToolsConnection } from "../WatchTools/useWatchTools";
import useDiagnostics from "../WatchTools/useDiagnostics";
import { WatchSessionContext } from "./context";

export default function WatchSessionProvider({ children }) {
  const manager = useRef(null);
  if (!manager.current) manager.current = new WatchToolsManager({ transport: "serial", keepSmpEnabled: true });
  const firmware = useMCUConnection(manager.current);
  const tools = useWatchToolsConnection(firmware);
  const diagnostics = useDiagnostics(tools);
  return <WatchSessionContext.Provider value={{ firmware, tools, diagnostics }}>{children}</WatchSessionContext.Provider>;
}
