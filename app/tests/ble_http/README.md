# HTTP terminal callback contract

Run `python3 app/tests/ble_http/test_ble_http.py`. The host test compiles the
actual HTTP controller and HTTP parser entrypoint against the real cJSON library,
with deterministic kernel/BLE adapters and address/undefined-behavior sanitizers.
It checks exactly one callback for success/error/empty/timeout, duplicate and
late replies, delayed timeout work after a new request, a callback starting the
next request, synchronous fast replies, send/allocation failures, ID wrap and
JSON string escaping. Oversized or malformed bodies produce an error rather
than overflowing/truncating the consumer's response.

Responses are decoded once by cJSON and borrowed during the consumer callback.
Callbacks run outside the request spinlock. A shared deadline guard makes old
queued work harmless without synchronously waiting for the timeout worker.

This verifies deterministic event orderings, not real Zephyr scheduling or BLE.
Build the pairing-enforced WatchDK target and separately check the native phone
relay and watch consumer before claiming hardware acceptance.
