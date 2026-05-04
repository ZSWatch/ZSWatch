#!/usr/bin/env python3
"""Boot native_sim and screenshot the watchface."""
import sys, os, time
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from native_sim_runner import NativeSimDevice

exe = os.path.join(os.path.dirname(__file__), '..', 'build', 'app', 'zephyr', 'zephyr.exe')
exe = os.path.abspath(exe)
print(f'Using exe: {exe}')

d = NativeSimDevice(exe_path=exe)
d.start()
print('Started, waiting for boot...')

if d.wait_for_log('UI Controller initialized', timeout=20):
    print('Boot OK, waiting for watchface to render...')
    time.sleep(4)
    crash = d.has_crash()
    if crash:
        print(f'CRASH: {crash}')
        print(d.get_logs()[-1000:])
    else:
        path = '/tmp/zswatch_horizon_watchface.png'
        d.screenshot(path)
        if os.path.isfile(path):
            print(f'Screenshot saved: {path}')
            print(f'Size: {os.path.getsize(path)} bytes')
        else:
            print('Screenshot FAILED - file not created')
else:
    print('Boot FAILED - timeout')
    logs = d.get_logs()
    print(logs[-2000:] if len(logs) > 2000 else logs)

d.stop()
print('Done')
