const headersFor = ({ token }) => ({ Accept: "application/vnd.github+json", ...(token ? { Authorization: `Bearer ${token}` } : {}) });

async function githubJson(config, path) {
  const response = await fetch(`https://api.github.com/repos/${config.owner}/${config.repo}/${path}`, { headers: headersFor(config) });
  if (!response.ok) {
    if (response.status === 403 || response.status === 429) throw new Error("GitHub rate limit reached. Try refreshing later.");
    throw new Error(`GitHub request failed (${response.status}). Try refreshing again.`);
  }
  return response.json();
}

export const isFirmwareAsset = (name = "") => /(?:watchdk@1|zswatch@1|@5)/i.test(name) && !/\.sha256$/i.test(name);
export const boardFromAsset = (name = "") => name.match(/(?:watchdk|zswatch(?:_legacy)?)@\d+/i)?.[0] || "";
export const compatibleAssets = (assets, board) => board ? assets.filter((asset) => boardFromAsset(asset.name) === board) : assets;

export async function fetchReleases(config) {
  const releases = await githubJson(config, "releases?per_page=20");
  return releases.filter((release) => !release.draft).map((release) => ({
    ...release,
    assets: release.assets.filter((asset) => isFirmwareAsset(asset.name) && asset.name.toLowerCase().endsWith(".zip")),
  })).filter((release) => release.assets.length);
}

export async function fetchArtifacts(config, numRuns = 10) {
  const [recent, main] = await Promise.all([
    githubJson(config, "actions/runs?per_page=50&status=success"),
    githubJson(config, "actions/runs?branch=main&status=success&per_page=10"),
  ]);
  const seenRuns = new Set();
  const seenBranches = new Set();
  const firmwares = [];
  for (const run of [...(main.workflow_runs || []), ...(recent.workflow_runs || [])]) {
    if (run.conclusion !== "success" || run.head_branch === "gh-pages" || seenRuns.has(run.id) || seenBranches.has(run.head_branch)) continue;
    seenRuns.add(run.id);
    const data = await githubJson(config, `actions/runs/${run.id}/artifacts?per_page=100`);
    const artifacts = (data.artifacts || []).filter((artifact) => !artifact.expired && isFirmwareAsset(artifact.name));
    if (!artifacts.length) continue;
    seenBranches.add(run.head_branch);
    firmwares.push({ branch: run.head_branch, user: run.actor?.login || "", runId: run.id, sha: run.head_sha || "", commitMessage: run.head_commit?.message || run.display_title || "", createdAt: run.created_at, artifacts });
    if (firmwares.length >= numRuns) break;
  }
  return firmwares;
}

export const artifactUrl = (config, runId, artifactId) => `https://github.com/${config.owner}/${config.repo}/actions/runs/${runId}/artifacts/${artifactId}`;

// Release downloads may be blocked by GitHub's CORS policy. Always offer the
// original link and local package picker as a fallback without leaving /update.
export async function downloadRelease(asset, signal, onProgress) {
  const response = await fetch(asset.browser_download_url, { signal });
  if (!response.ok) throw new Error(`Release download failed (${response.status}).`);
  const total = Number(response.headers.get("content-length")) || asset.size;
  if (!response.body) return new File([await response.blob()], asset.name);
  const reader = response.body.getReader();
  const chunks = [];
  let received = 0;
  while (true) {
    const { done, value } = await reader.read();
    if (done) break;
    chunks.push(value);
    received += value.length;
    onProgress(total ? Math.min(100, Math.floor(received / total * 100)) : null);
  }
  return new File(chunks, asset.name);
}
