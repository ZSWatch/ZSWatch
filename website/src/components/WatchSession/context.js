import { createContext, useContext } from "react";

export const WatchSessionContext = createContext(null);
export function useWatchSession() {
  const session = useContext(WatchSessionContext);
  if (!session) throw new Error("Watch tools must be inside WatchSessionProvider.");
  return session;
}
