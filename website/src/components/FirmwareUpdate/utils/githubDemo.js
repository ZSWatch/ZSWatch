// Local development only: sample catalog entries never contain flashable files.
export const githubDemoEnabled = () => process.env.NODE_ENV === "development"
  && typeof window !== "undefined" && new URLSearchParams(window.location.search).get("githubDemo") === "1";

const assets = (prefix) => ["watchdk@1", "zswatch@5", "zswatch@1"].flatMap((board) =>
  ["release", "debug"].map((type) => ({ id: `${prefix}-${board}-${type}`, name: `${board}_nrf5340_cpuapp_${type}.zip`, size: 1843200, demo: true })));
export const demoReleases = [
  { id: "demo-090", name: "ZSWatch 0.9.0", tag_name: "v0.9.0", published_at: "2026-09-28", assets: assets("090") },
  { id: "demo-081", name: "ZSWatch 0.8.1", tag_name: "v0.8.1", published_at: "2026-08-11", assets: assets("081") },
];
export const demoBuilds = [
  { runId: "demo-main", branch: "main", sha: "2d93427e", user: "ZSWatch", createdAt: "2026-09-30", commitMessage: "Improve watch connectivity and diagnostics", artifacts: assets("main") },
  { runId: "demo-logs", branch: "feature/ble-logs", sha: "a036719d", user: "ZSWatch", createdAt: "2026-09-29", commitMessage: "Capture firmware logs over Bluetooth", artifacts: assets("logs") },
];
