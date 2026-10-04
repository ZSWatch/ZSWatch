"""Exercise production parameter callbacks with deterministic Bluetooth/work adapters."""
import subprocess
import tempfile
import unittest
from pathlib import Path


def function(source, signature):
    start = source.index(signature + "\n{")
    end = source.index("\n}", start) + 2
    return source[start:end]


class ConnectionParametersTests(unittest.TestCase):
    def test_smp_reconnect_and_idle_policy(self):
        here = Path(__file__).resolve().parent
        app = here.parents[1]
        comm = (app / "src/ble/ble_comm.c").read_text()
        manager = (app / "src/managers/zsw_smp_manager.c").read_text()
        signatures = [
            "int ble_comm_set_short_connection_interval(void)",
            "int ble_comm_set_default_connection_interval(void)",
            "static void update_conn_interval_slow_handler(struct k_work *item)",
            "static void ble_connected(struct bt_conn *conn, uint8_t err)",
            "static void ble_disconnected(struct bt_conn *conn, uint8_t reason)",
        ]
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "callbacks.inc").write_text("\n\n".join(function(comm, s) for s in signatures)
                + "\n" + function(manager, "int zsw_smp_manager_disable(void)")
                + "\n" + function(manager, "static void smp_auto_disable_work_handler(struct k_work *work)"))
            binary = root / "ble-parameters-test"
            build = subprocess.run(["cc", "-std=c11", "-Wall", "-Wextra", "-Werror",
                "-Wno-unused-parameter", "-I", directory, str(here / "test_ble_parameters.c"),
                "-o", str(binary)], capture_output=True, text=True, timeout=30)
            self.assertEqual(build.returncode, 0, build.stderr)
            run = subprocess.run([str(binary)], capture_output=True, text=True, timeout=10)
            self.assertEqual(run.returncode, 0, run.stderr)


if __name__ == "__main__":
    unittest.main()
