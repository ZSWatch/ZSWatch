import { useState } from "react";

const PrebuiltFirmwares = ({ 
  firmwares, 
  isFetchingFirmwares, 
  onFetchFirmwares, 
  onDownloadFirmware,
  isConnected = false
}) => {
  const [expandedBranches, setExpandedBranches] = useState({});

  const toggleExpanded = (branch) => {
    setExpandedBranches(prev => ({
      ...prev,
      [branch]: !prev[branch]
    }));
  };

  // Group firmwares by branch
  const firmwaresByBranch = firmwares.reduce((acc, firmware) => {
    if (!acc[firmware.branch]) {
      acc[firmware.branch] = [];
    }
    acc[firmware.branch].push(firmware);
    return acc;
  }, {});

  return (
    <div className="space-y-3">
      {/* Loading/Results */}
      {isFetchingFirmwares ? (
        <div className="text-center py-6">
          <div className="inline-flex items-center justify-center w-6 h-6 bg-zswatch-secondary rounded-full animate-spin mb-2">
            <span className="text-white text-xs">⏳</span>
          </div>
          <p className="text-gray-700 dark:text-white text-sm">Loading firmwares...</p>
        </div>
      ) : firmwares.length === 0 ? (
        <div className="text-center py-6 bg-gray-50 dark:bg-black/20 border border-gray-200 dark:border-white/10 rounded-lg">
          <span className="text-2xl mb-2 block">📦</span>
          <p className="text-gray-700 dark:text-white text-sm">No firmwares fetched yet</p>
          <p className="text-xs text-gray-500 dark:text-gray-400">Click "Fetch Latest" to get builds</p>
        </div>
      ) : (
        <div className="space-y-2">
          {Object.entries(firmwaresByBranch).map(([branch, branchFirmwares]) => (
            <div key={branch} className="border border-gray-200 dark:border-white/10 rounded-lg overflow-hidden">
              {/* Branch Header */}
              <div className="bg-gray-50 dark:bg-black/20 px-3 py-2 border-b border-gray-200 dark:border-white/10">
                <div className="flex items-center justify-between">
                  <div className="flex items-center gap-2">
                    <span className="text-sm font-medium text-gray-900 dark:text-white">{branch}</span>
                    {branch === 'main' && (
                      <span className="text-xs bg-green-100 dark:bg-green-900/30 text-green-700 dark:text-green-300 px-2 py-1 rounded-full">
                        Release
                      </span>
                    )}
                    {branch !== 'main' && (
                      <span className="text-xs bg-blue-100 dark:bg-blue-900/30 text-blue-700 dark:text-blue-300 px-2 py-1 rounded-full">
                        Debug
                      </span>
                    )}
                  </div>
                  <span className="text-xs text-gray-500 dark:text-gray-400">
                    {branchFirmwares.length} build{branchFirmwares.length !== 1 ? 's' : ''}
                  </span>
                </div>
              </div>

              {/* Branch Builds */}
              <div className="space-y-1">
                {branchFirmwares.map((firmware, index) => {
                  const branchKey = `${branch}-${index}`;
                  return (
                    <div key={index} className="border-b border-gray-100 dark:border-white/5 last:border-b-0">
                      <button
                        onClick={() => toggleExpanded(branchKey)}
                        className="w-full flex items-center justify-between p-2 bg-gray-100 dark:bg-white/10 hover:bg-gray-200 dark:hover:bg-white/15 transition-all"
                      >
                        <div className="flex items-center gap-2 text-left">
                          <span className="text-xs text-gray-600 dark:text-gray-300">by {firmware.user}</span>
                          <span className="text-xs text-gray-500 dark:text-gray-400">
                            {firmware.artifacts.length} artifact{firmware.artifacts.length !== 1 ? 's' : ''}
                          </span>
                        </div>
                        <div className="flex items-center gap-2">
                          {branch === 'main' && index === 0 && (
                            <button
                              onClick={(e) => {
                                e.stopPropagation();
                                if (firmware.artifacts.length > 0) {
                                  onDownloadFirmware(firmware.runId, firmware.artifacts[0].id);
                                }
                              }}
                              className="bg-zswatch-primary text-black px-2 py-1 rounded text-xs font-medium hover:bg-zswatch-primary/90 transition-all"
                            >
                              Quick Download
                            </button>
                          )}
                          <span className={`text-xs transition-transform ${expandedBranches[branchKey] ? 'rotate-180' : ''}`}>
                            ▼
                          </span>
                        </div>
                      </button>
                      
                      {expandedBranches[branchKey] && (
                        <div className="px-3 pb-2">
                          <div className="space-y-1">
                            {firmware.artifacts.map((artifact, artifactIndex) => (
                              <div key={artifactIndex} className="flex items-center justify-between p-2 bg-gray-50 dark:bg-white/5 hover:bg-gray-100 dark:hover:bg-white/10 rounded border border-gray-200 dark:border-white/10 transition-colors">
                                <span className="text-xs text-gray-700 dark:text-gray-300 font-mono">{artifact.name}</span>
                                <button 
                                  onClick={() => onDownloadFirmware(firmware.runId, artifact.id)}
                                  className="bg-zswatch-primary text-black px-2 py-1 rounded text-xs font-medium hover:bg-zswatch-primary/90 transition-all"
                                >
                                  Download
                                </button>
                              </div>
                            ))}
                          </div>
                        </div>
                      )}
                    </div>
                  );
                })}
              </div>
            </div>
          ))}
        </div>
      )}
    </div>
  );
};

export default PrebuiltFirmwares;
