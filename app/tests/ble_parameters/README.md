# SMP connection-parameter ownership

Run `python3 app/tests/ble_parameters/test_ble_parameters.py`.

The host test compiles the production BLE connection/parameter callbacks and SMP
cleanup functions with deterministic Bluetooth/work adapters. It checks that SMP
registration surviving a service-change reconnect does not arm the ordinary idle
timer, queued idle and sensor requests cannot override SMP parameters, successful
explicit/inactivity disable restores idle parameters, failed unregister retains
ownership, and disconnect cancels both deferred parameter requests.

This is an event-order regression. Real negotiated intervals, PHY, data length
and firmware upload throughput are checked separately on hardware.
