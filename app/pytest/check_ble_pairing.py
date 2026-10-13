"""Exercise the pairing window through BLE-over-TCP and the real Settings UI.

Build native_sim with boards/test_mode.conf and BLE_DISABLE_PAIRING_REQUIRED=n.
Install requirements-pairing.txt, then run this script with --exe pointing to
the resulting zephyr.exe. No radio hardware or root access is required.

The virtual HCI command shims below come from ROS's pair_notify.py PoC.
"""

import argparse
import asyncio
import json
import os
from pathlib import Path
import tempfile

from bumble import att, hci
from bumble.controller import Controller
from bumble.core import UUID
from bumble.device import Device, Peer
from bumble.hci import Address, OwnAddressType
from bumble.host import Host
from bumble.link import LocalLink
from bumble.pairing import PairingConfig
from bumble.transport import open_transport

from native_sim_runner import NativeSimDevice

RX_UUID = UUID('6e400002-b5a3-f393-e0a9-e50e24dcca9e')
PAYLOAD = b'GB({"t":"notify","id":1234,"src":"test","title":"Pairing regression","body":"Authorized"})'


def set_phy(self, command):
    self._send_hci_command_status(hci.HCI_COMMAND_STATUS_PENDING, command.op_code)
    self.send_hci_packet(hci.HCI_LE_PHY_Update_Complete_Event(
        status=hci.HCI_ErrorCode.SUCCESS, connection_handle=command.connection_handle,
        tx_phy=hci.HCI_LE_1M_PHY, rx_phy=hci.HCI_LE_1M_PHY))


def set_data_length(self, command):
    return hci.HCI_StatusAndConnectionHandleReturnParameters(
        status=hci.HCI_ErrorCode.SUCCESS, connection_handle=command.connection_handle)


def connection_update(self, command):
    self._send_hci_command_status(hci.HCI_COMMAND_STATUS_PENDING, command.op_code)
    self.send_hci_packet(hci.HCI_LE_Connection_Update_Complete_Event(
        status=hci.HCI_ErrorCode.SUCCESS, connection_handle=command.connection_handle,
        connection_interval=command.connection_interval_max,
        peripheral_latency=command.max_latency, supervision_timeout=command.supervision_timeout))


Controller.on_hci_le_set_phy_command = set_phy
Controller.on_hci_le_set_data_length_command = set_data_length
Controller.on_hci_host_number_of_completed_packets_command = lambda self, command: None
Controller.on_hci_le_connection_update_command = connection_update


async def wait_log(firmware, text, timeout=10):
    assert await asyncio.to_thread(firmware.wait_for_log, text, timeout), firmware.get_logs()


async def open_window(firmware, initial=True):
    if initial:
        firmware.shell_command('app launch Settings')
        await asyncio.sleep(0.8)
        assert 'Launching app: Settings' in firmware.get_shell_output()
        firmware.shell_command('touch 100 85')  # Settings > Bluetooth
        await asyncio.sleep(0.4)
    before = len(firmware.get_logs())
    firmware.shell_command('touch 185 48')  # Pairable > Enable
    await asyncio.sleep(0.5)
    assert 'Enable Pairable' in firmware.get_logs()[before:], firmware.get_logs()


async def find_rx(connection):
    peer = Peer(connection)
    await asyncio.wait_for(peer.discover_services(), 10)
    await asyncio.wait_for(peer.discover_characteristics(), 10)
    matches = peer.get_characteristics_by_uuid(RX_UUID)
    assert len(matches) == 1, 'BLE transport RX characteristic not found'
    return peer, matches[0]


async def require_rejection(connection, firmware, expected_log):
    before = len(firmware.get_logs())
    try:
        await asyncio.wait_for(connection.pair(), 10)
    except (Exception, asyncio.CancelledError) as error:
        print(f'Pairing stopped: {type(error).__name__}: {error}', flush=True)
    else:
        raise AssertionError('Pairing unexpectedly succeeded')
    await asyncio.sleep(0.3)
    logs = firmware.get_logs()[before:]
    assert expected_log in logs, logs
    assert 'Pairing Failed' in logs and 'Disconnected:' in logs, logs
    assert firmware.is_alive() and not firmware.has_crash(), firmware.get_logs()


async def run_case(args, name, bonding):
    firmware = NativeSimDevice(exe_path=str(args.exe), display=':97',
                               extra_args=['--bt-dev=127.0.0.1:9000'])
    found = asyncio.get_running_loop().create_future()
    link = LocalLink()
    controller = Controller('test-peer', link=link, public_address=Address('F0:F1:F2:F3:F4:F5'))
    attacker = Device(address=Address('F0:F1:F2:F3:F4:F5'), host=Host(controller, controller))
    attacker.pairing_config_factory = lambda connection: PairingConfig(sc=True, mitm=False, bonding=bonding)

    @attacker.on('advertisement')
    def advertised(advertisement):
        if advertisement.is_connectable and not found.done():
            found.set_result(advertisement.address)

    await attacker.power_on()
    await attacker.start_scanning()
    transport = await open_transport('tcp-server:127.0.0.1:9000')
    sim_controller = Controller('sim', host_source=transport.source, host_sink=transport.sink, link=link)
    result_dir = args.output_dir / f'{name}-bonding-{bonding}'
    result_dir.mkdir(parents=True, exist_ok=True)
    previous_cwd = Path.cwd()
    try:
        with tempfile.TemporaryDirectory(prefix='zswatch-pairing-') as workdir:
            # The existing runner erases simulated flash in its working directory.
            os.chdir(workdir)
            await asyncio.to_thread(firmware.start)
            await wait_log(firmware, 'ZSWatch application started')
            address = await asyncio.wait_for(found, 15)
            connection = await asyncio.wait_for(attacker.connect(address, own_address_type=OwnAddressType.RANDOM), 15)
            peer, rx = await find_rx(connection)

            # A failed pairing must not be masked by a debug build allowing plaintext writes.
            try:
                await asyncio.wait_for(peer.write_value(rx, PAYLOAD, with_response=True), 5)
            except att.ATT_Error as error:
                assert error.error_code in (att.ATT_INSUFFICIENT_AUTHENTICATION_ERROR,
                                            att.ATT_INSUFFICIENT_ENCRYPTION_ERROR), str(error)
            else:
                raise AssertionError('Plaintext write accepted; check BLE_DISABLE_PAIRING_REQUIRED=n')

            if name in ('open', 'expired'):
                await open_window(firmware)
            if name == 'expired':
                before = len(firmware.get_logs())
                print('Waiting for the real 60-second pairing window to expire...', flush=True)
                await asyncio.sleep(61)
                assert 'Disable Pairable' in firmware.get_logs()[before:]

            if name != 'open':
                await require_rejection(connection, firmware, 'Pairing rejected: pairing window closed')
                assert 'notification_mgr: zsw_notification_manager_add' not in firmware.get_logs()
            else:
                await asyncio.wait_for(connection.pair(), 10)
                await asyncio.sleep(0.5)
                assert connection.encryption and connection.sc
                await wait_log(firmware, 'Pairing accepted: pairing window open')
                assert firmware.get_logs().rfind('Disable Pairable') > firmware.get_logs().rfind('Enable Pairable')

                if bonding:
                    # Closing the window must still allow encryption using existing bond keys.
                    await connection.disconnect()
                    await asyncio.sleep(0.5)
                    connection = await asyncio.wait_for(attacker.connect(address, own_address_type=OwnAddressType.RANDOM), 15)
                    before = len(firmware.get_logs())
                    await asyncio.wait_for(connection.encrypt(), 10)
                    peer, rx = await find_rx(connection)
                    assert connection.encryption
                    assert 'Pairing accepted:' not in firmware.get_logs()[before:]
                    print('Existing bond reconnected with window closed', flush=True)

                if bonding:
                    # Verify protected writes after re-encryption without adding
                    # another popup on top of Settings and the pairing popup.
                    await asyncio.wait_for(peer.write_value(rx, b'GB({"t":"ver"})', with_response=True), 5)
                    await asyncio.sleep(6)
                    firmware.shell_command('power wake')
                    firmware.shell_command('app close')
                    await asyncio.sleep(0.5)
                    await open_window(firmware)
                    await require_rejection(connection, firmware, 'Pairing Failed (4). Disconnecting.')
                    print('Unauthenticated replacement of existing bond rejected with window open', flush=True)
                else:
                    await asyncio.wait_for(peer.write_value(rx, PAYLOAD, with_response=True), 5)
                    await wait_log(firmware, 'notification_mgr: zsw_notification_manager_add')
                assert firmware.is_alive() and not firmware.has_crash()

            print(json.dumps({'case': name, 'bonding': bonding, 'result': 'PASS'}), flush=True)
    finally:
        firmware.screenshot(str(result_dir / 'screen.png'))
        (result_dir / 'firmware.log').write_text(firmware.get_logs())
        (result_dir / 'shell.log').write_text(firmware.get_shell_output())
        await asyncio.to_thread(firmware.stop)
        os.chdir(previous_cwd)
        transport.server.close()
        await transport.server.wait_closed()
        await transport.close()


async def main(args):
    # Check generated configuration before running, so a wrong variant cannot pass.
    config = (args.exe.parent / '.config').read_text()
    for required in ('CONFIG_BT_SMP_APP_PAIRING_ACCEPT=y', 'CONFIG_BT_SMP_SC_PAIR_ONLY=y',
                     '# CONFIG_BT_SMP_ALLOW_UNAUTH_OVERWRITE is not set',
                     '# CONFIG_BLE_DISABLE_PAIRING_REQUIRED is not set', 'CONFIG_ZSW_TEST_SKIP_ONBOARDING=y'):
        assert required in config, f'Missing test configuration: {required}'
    for name in args.scenarios:
        for bonding in (False, True):
            await run_case(args, name, bonding)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--exe', type=Path, required=True)
    parser.add_argument('--output-dir', type=Path, default=Path('/tmp/zswatch-pairing-results'))
    parser.add_argument('--scenarios', nargs='+', choices=('closed', 'open', 'expired'), default=['closed', 'open', 'expired'])
    arguments = parser.parse_args()
    arguments.exe = arguments.exe.resolve()
    arguments.output_dir = arguments.output_dir.resolve()
    asyncio.run(main(arguments))
