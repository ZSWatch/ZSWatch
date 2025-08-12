import React from "react";

const DeviceControls = ({ onEchoTest, onDisconnect, onReset, onAdvancedMode }) => {
  return (
    <div className="flex flex-wrap gap-3 mb-4">
      <button
        onClick={onEchoTest}
        className="flex items-center px-3 py-2 bg-zswatch-secondary text-white rounded-lg hover:bg-blue-600 transition-colors font-medium text-sm"
      >
        <span className="mr-1">🔊</span>
        Echo Test
      </button>
      <button
        onClick={onDisconnect}
        className="flex items-center px-3 py-2 bg-red-500 text-white rounded-lg hover:bg-red-600 transition-colors font-medium text-sm"
      >
        <span className="mr-1">🔌</span>
        Disconnect
      </button>
      <button
        onClick={onReset}
        className="flex items-center px-3 py-2 bg-orange-500 text-white rounded-lg hover:bg-orange-600 transition-colors font-medium text-sm"
      >
        <span className="mr-1">🔄</span>
        Reset Watch
      </button>
      <button
        onClick={onAdvancedMode}
        className="flex items-center px-3 py-2 bg-zswatch-dark text-white rounded-lg hover:bg-gray-700 transition-colors font-medium text-sm"
      >
        <span className="mr-1">⚙️</span>
        Advanced Mode
      </button>
    </div>
  );
};

export default DeviceControls;
