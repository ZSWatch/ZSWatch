export const fetchArtifacts = async (githubConfig, numRuns = 5) => {
  let firmwares = [];
  try {
    const { owner, repo, token } = githubConfig;
    
    // Step 1: Get the list of workflow runs
    const runsResponse = await fetch(
      `https://api.github.com/repos/${owner}/${repo}/actions/runs`,
      token
        ? {
            headers: {
              Authorization: `Bearer ${token}`,
            },
          }
        : {},
    );
    const runsData = await runsResponse.json();

    if (!runsData.workflow_runs || runsData.workflow_runs.length === 0) {
      console.log("No workflow runs found.");
      return [];
    }

    console.log("Found", runsData, runsData.workflow_runs.length, "workflow runs.");

    // Step 2: Get the latest X workflow runs
    const latestRuns = runsData.workflow_runs
      .filter((run) => run.conclusion === "success" && run.head_branch !== "gh-pages")
      .slice(0, numRuns);
    console.log(`Fetching artifacts from the latest ${numRuns} workflow runs...`);

    for (const run of latestRuns) {
      console.log(`Processing workflow run: ${run.id} (${run.name})`, run);

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
        artifacts: artifactsData.artifacts.filter(
          (artifact) => artifact.name.includes("@4") || artifact.name.includes("@5"),
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
