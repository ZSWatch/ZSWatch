import { useWatchSession } from "../../WatchSession/context";

export const useMCUManager = () => {
  const { firmware } = useWatchSession();
  return firmware;
};
