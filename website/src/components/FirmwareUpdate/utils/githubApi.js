export const fetchArtifacts = async (githubConfig, numRuns = 5) => {
  let firmwares = [];
  try {
    const { owner, repo, token } = githubConfig;
    const fetchOptions = token
      ? {
          headers: {
            Authorization: `Bearer ${token}`,
          },
        }
      : {};

    // Fetch recent runs plus explicitly grab the latest successful run on main
    const [runsResponse, mainRunResponse] = await Promise.all([
      fetch(
        `https://api.github.com/repos/${owner}/${repo}/actions/runs?per_page=${Math.max(
          numRuns * 2,
          10,
        )}`,
        fetchOptions,
      ),
      fetch(
        `https://api.github.com/repos/${owner}/${repo}/actions/runs?branch=main&status=success&per_page=1`,
        fetchOptions,
      ),
    ]);

    const runsData = await runsResponse.json();
    const mainRunsData = await mainRunResponse.json();

    const workflowRuns = runsData.workflow_runs || [];
    const mainBranchRuns = mainRunsData.workflow_runs || [];

    if (workflowRuns.length === 0 && mainBranchRuns.length === 0) {
      console.log("No workflow runs found.");
      return [];
    }

    console.log("Found", workflowRuns.length, "workflow runs.");

    const successfulRuns = workflowRuns.filter(
      (run) => run.conclusion === "success" && run.head_branch !== "gh-pages",
    );
    const latestMainRun = mainBranchRuns.find((run) => run.conclusion === "success");

    // Ensure main is present by prepending it before trimming to numRuns
    const runsWithMain = latestMainRun ? [latestMainRun, ...successfulRuns] : successfulRuns;
    const uniqueRuns = runsWithMain.filter(
      (run, index, arr) => arr.findIndex((other) => other.id === run.id) === index,
    );
    const latestRuns = uniqueRuns.slice(0, numRuns);
    console.log(`Fetching artifacts from ${latestRuns.length} workflow runs (main prioritized)...`);

    for (const run of latestRuns) {
      console.log(`Processing workflow run: ${run.id} (${run.name})`, run);

      const commitSha = run.head_sha || run.head_commit?.id || "";
      const commitMessage = run.head_commit?.message || run.display_title || "";

      // Step 3: Get the artifacts for the current workflow run
      const artifactsResponse = await fetch(
        `https://api.github.com/repos/${owner}/${repo}/actions/runs/${run.id}/artifacts`,
        token
          ? {
              headers: {
                Authorization: `Bearer ${token}`,
              },
            }
          : {},
      );
      const artifactsData = await artifactsResponse.json();

      if (!artifactsData.artifacts || artifactsData.artifacts.length === 0) {
        console.log(`No artifacts found for workflow run: ${run.id}`);
        continue;
      }

      const fwRun = {
        branch: run.head_branch,
        user: run.actor.login,
        runId: run.id,
        sha: commitSha,
        commitMessage,
        createdAt: run.created_at,
        updatedAt: run.updated_at,
        artifacts: artifactsData.artifacts.filter(
          (artifact) => artifact.name.includes("watchdk@1") || artifact.name.includes("@5"),
        ),
      };

      firmwares.push(fwRun);
    }
  } catch (error) {
    console.error("Error fetching artifacts:", error);
  }
  return firmwares;
};

export const downloadFirmware = (githubConfig, runId, artifactId) => {
  const downloadUrl = `https://github.com/${githubConfig.owner}/${githubConfig.repo}/actions/runs/${runId}/artifacts/${artifactId}`;
  console.log("Download URL:", downloadUrl);
  window.location.href = downloadUrl;
};
