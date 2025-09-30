const BluetoothConnect = ({ 
  bluetoothAvailable, 
  deviceName, 
  onDeviceNameChange, 
  onConnect,
  onPreviewDemo,
  isConnected = false
}) => {
  return (
    <div className="grid grid-cols-1 lg:grid-cols-3 gap-6">
      {/* Main Content */}
      <div className="lg:col-span-2 space-y-6">
        {/* Device Connection Section */}
        <div className="bg-white/5 backdrop-blur-sm rounded-xl p-6 border border-white/10">
          <h2 className="text-xl font-semibold text-white mb-2">Device</h2>
          <p className="text-gray-300 mb-4">Choose your target and manage the connection.</p>
          
          <div className="space-y-4">
            <div>
              <label className="block text-sm font-medium text-gray-300 mb-2">Device name</label>
              <input 
                type="text" 
                value={deviceName}
                onChange={(e) => onDeviceNameChange(e.target.value)}
                placeholder="ZSWatch"
                className="w-full bg-black/30 border border-white/20 rounded-lg px-4 py-3 text-white placeholder-gray-400 focus:border-zswatch-primary focus:outline-none focus:ring-2 focus:ring-zswatch-primary/50 transition-all"
              />
              <p className="text-gray-400 text-sm mt-1">Leave empty to scan for any compatible device</p>
            </div>
            
            <div className="flex flex-wrap gap-3">
              <button 
                onClick={onConnect}
                disabled={!bluetoothAvailable}
                className={`px-4 py-2 rounded-lg font-medium transition-all ${
                  bluetoothAvailable
                    ? isConnected
                      ? 'bg-green-500 text-white border border-green-500 hover:bg-green-600'
                      : 'bg-zswatch-primary text-black border border-zswatch-primary hover:bg-zswatch-primary/90'
                    : 'bg-gray-500/20 text-gray-400 border border-gray-500/30 cursor-not-allowed'
                }`}
              >
                {!bluetoothAvailable 
                  ? 'Bluetooth Not Available' 
                  : isConnected 
                    ? 'Connected' 
                    : 'Connect'
                }
              </button>
              
              <button 
                onClick={onPreviewDemo}
                className="flex items-center gap-2 px-4 py-2 bg-zswatch-secondary/20 text-zswatch-secondary border border-zswatch-secondary/30 hover:bg-zswatch-secondary/30 transition-all rounded-lg"
              >
                <span className="text-sm">👁️</span>
                Preview Demo
              </button>
            </div>
          </div>
        </div>

        {/* Instructions */}
        <div className="bg-white/5 backdrop-blur-sm rounded-xl p-6 border border-white/10">
          <h2 className="text-xl font-semibold text-white mb-2">Connection Instructions</h2>
          <p className="text-gray-300 mb-4">Follow these steps to connect your ZSWatch.</p>
          
          <div className="space-y-3">
            <div className="flex items-center text-gray-300">
              <span className="w-6 h-6 bg-zswatch-secondary text-white rounded-full flex items-center justify-center mr-3 text-xs font-bold">1</span>
              <span>Make sure your ZSWatch is powered on and has Bluetooth enabled</span>
            </div>
            <div className="flex items-center text-gray-300">
              <span className="w-6 h-6 bg-zswatch-secondary text-white rounded-full flex items-center justify-center mr-3 text-xs font-bold">2</span>
              <span>Click the "Connect" button above</span>
            </div>
            <div className="flex items-center text-gray-300">
              <span className="w-6 h-6 bg-zswatch-secondary text-white rounded-full flex items-center justify-center mr-3 text-xs font-bold">3</span>
              <span>Select your ZSWatch from the browser's device list</span>
            </div>
            <div className="flex items-center text-gray-300">
              <span className="w-6 h-6 bg-zswatch-secondary text-white rounded-full flex items-center justify-center mr-3 text-xs font-bold">4</span>
              <span>Wait for the connection to establish</span>
            </div>
          </div>
        </div>
      </div>

      {/* Right Sidebar */}
      <div className="space-y-6">
        {/* Bluetooth Status */}
        <div className={`backdrop-blur-sm rounded-xl p-6 border ${
          bluetoothAvailable 
            ? 'bg-green-500/10 border-green-500/30' 
            : 'bg-yellow-500/10 border-yellow-500/30'
        }`}>
          <h3 className="text-lg font-semibold text-white mb-3 flex items-center gap-2">
            <span className="text-xl">{bluetoothAvailable ? '✅' : '⚠️'}</span>
            Bluetooth Status
          </h3>
          <p className={`text-sm leading-relaxed ${
            bluetoothAvailable ? 'text-green-200' : 'text-yellow-200'
          }`}>
            {bluetoothAvailable
              ? "Your browser supports Web Bluetooth. You can connect to your ZSWatch."
              : "This tool requires a compatible browser with Web Bluetooth support. Please use Chrome, Edge, or Opera on desktop/laptop, Chrome on Android, or Bluefy on iOS/iPadOS."}
          </p>
        </div>

        {/* Browser Compatibility */}
        <div className="bg-white/5 backdrop-blur-sm rounded-xl p-6 border border-white/10">
          <h3 className="text-lg font-semibold text-white mb-4 flex items-center gap-2">
            <span className="text-xl">🌐</span>
            Browser Support
          </h3>
          <ul className="space-y-0 text-sm text-gray-300 !pl-0 !ml-0" style={{ paddingLeft: '0 !important', paddingInlineStart: '0 !important', margin: '0 !important', listStyle: 'none' }}>
            <li className="flex items-center">
              <span className="w-2 h-2 bg-green-500 rounded-full mr-3"></span>
              Chrome, Edge, Opera (Desktop)
            </li>
            <li className="flex items-center">
              <span className="w-2 h-2 bg-green-500 rounded-full mr-3"></span>
              Chrome (Android)
            </li>
            <li className="flex items-center">
              <span className="w-2 h-2 bg-yellow-500 rounded-full mr-3"></span>
              Bluefy (iOS/iPadOS)
            </li>
          </ul>
        </div>

        {/* Features */}
        <div className="bg-white/5 backdrop-blur-sm rounded-xl p-6 border border-white/10">
          <h3 className="text-lg font-semibold text-white mb-4 flex items-center gap-2">
            <span className="text-xl">🔧</span>
            Features
          </h3>
          <ul className="space-y-2 text-sm text-gray-300">
            <li className="flex items-center">
              <span className="w-2 h-2 bg-zswatch-primary rounded-full mr-3"></span>
              Wireless updates via BLE
            </li>
            <li className="flex items-center">
              <span className="w-2 h-2 bg-zswatch-primary rounded-full mr-3"></span>
              Progress monitoring
            </li>
            <li className="flex items-center">
              <span className="w-2 h-2 bg-zswatch-primary rounded-full mr-3"></span>
              Automatic firmware detection
            </li>
          </ul>
        </div>
      </div>
    </div>
  );
};

export default BluetoothConnect;
