import React from "react";
import WatchSessionProvider from "../components/WatchSession/WatchSessionProvider";

export default function Root({ children }) {
  return <WatchSessionProvider>{children}</WatchSessionProvider>;
}
