import React from "react";

const ConnectingScreen = () => {
  return (
    <div className="text-center py-8">
      <div className="inline-flex items-center justify-center w-16 h-16 bg-zswatch-secondary rounded-full mb-4 animate-pulse">
        <span className="text-white text-2xl">📱</span>
      </div>
      <h3 className="text-lg font-semibold text-gray-800 mb-2">Connecting to ZSWatch...</h3>
      <p className="text-gray-600">Please wait while we establish a connection</p>
      <div className="mt-4">
        <div className="w-64 mx-auto bg-gray-200 rounded-full h-2">
          <div className="bg-zswatch-secondary h-2 rounded-full animate-pulse" style={{width: '60%'}}></div>
        </div>
      </div>
    </div>
  );
};

export default ConnectingScreen;
