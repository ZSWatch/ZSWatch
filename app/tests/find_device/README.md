# Watch-owned Find safety

Run `python3 app/tests/find_device/test_find_device.py` from the repository root.
The host executable compiles the real `ble_find_device.c` with deterministic
kernel/time/device adapters and the actual driver's conditional Find-stop body.
It checks explicit stop, ten-second expiry, duplicate true without deadline
extension, disconnect, late callbacks after a newer activation, failed motor
acquisition and preservation of an unrelated alarm pattern.

The test does not emulate Zephyr interrupt scheduling, GPIO/PWM effects or BLE
supervision latency. Build for WatchDK with pairing enforced before hardware
acceptance. The existing Boolean `find` protocol is unchanged. A successful Find
activation has a ten-second watch deadline; a connected phone normally sends
false sooner. Disconnect is observed after BLE's supervision timeout, not at the
instant radio reception stops. Fresh true after the prior activation ends starts
a new bounded activation; duplicate true during it does not renew the deadline.

The motor guard stops only a currently active Find pattern. Explicit false,
timeout and disconnect cannot cancel another pattern when Find never acquired
the motor or a local action replaced it. Keep-awake work checks current activity;
queued work after Stop cannot revive Find. Hardware testing must independently
confirm the UART consumer and stop reason, then a later normal Find/notification
still works. A host pass or successful build alone does not prove that.
