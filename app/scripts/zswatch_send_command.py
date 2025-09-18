# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

from zswatch_ble_control import (
    zsw_send_nus_commands,
    zsw_disconnect,
    zsw_list_uart_devices,
    zsw_connect_nus,
    zsw_disconnect,
)
import argparse, sys
from enum import Enum
import asyncio


class Buttons(Enum):
    TOP_RIGHT = 0
    TOP_LEFT = 1
    BOTTOM_RIGHT = 2
    BOTTOM_LEFT = 3


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Tool to send commands to ZSWatch with the NUS Service. If no address specified it will look for all BLE devices with the NUS service."
    )

    parser.add_argument(
        "--address",
        dest="address",
        nargs="+",
        # default=["E8:F4:FB:CE:C7:83", "F2:33:CD:87:D5:AD"],
        required=False,
        help="Macs of ZSWatch to send command to",
    )

    parser.add_argument(
        "--timeout",
        dest="timeout",
        default=10,
        required=False,
        help="Scan timeout for ZSWatch discovery",
    )

    args = parser.parse_args()

    commands = [
        ("Control:{}".format(int(Buttons.BOTTOM_LEFT.value)), 2),  # Wakeup
        ("Control:{}".format(Buttons.TOP_LEFT.value), 1),
        ("Control:{}".format(Buttons.BOTTOM_LEFT.value), 3),
        ("Control:{}".format(Buttons.BOTTOM_LEFT.value), 0.5),
        ("Control:{}".format(Buttons.BOTTOM_LEFT.value), 0.5),
        ("Control:{}".format(Buttons.BOTTOM_LEFT.value), 0.5),
        ("Control:{}".format(Buttons.BOTTOM_LEFT.value), 0.5),
        ("Control:{}".format(Buttons.BOTTOM_LEFT.value), 0.5),
        ("Control:{}".format(Buttons.BOTTOM_LEFT.value), 0.5),
        ("Control:{}".format(Buttons.BOTTOM_LEFT.value), 0.5),
        ("Control:{}".format(Buttons.BOTTOM_LEFT.value), 0.5),
        ("Control:{}".format(Buttons.BOTTOM_LEFT.value), 0.5),
        ("Control:{}".format(Buttons.BOTTOM_LEFT.value), 0.5),
        ("Control:{}".format(Buttons.BOTTOM_LEFT.value), 0.5),
        ("Control:{}".format(Buttons.BOTTOM_LEFT.value), 0.5),
        ("Control:{}".format(Buttons.BOTTOM_LEFT.value), 0.5),
        ("Control:{}".format(Buttons.BOTTOM_LEFT.value), 0.5),
        ("Control:{}".format(Buttons.BOTTOM_LEFT.value), 0.5),
        ("Control:{}".format(Buttons.BOTTOM_LEFT.value), 1),
        ("Control:{}".format(Buttons.BOTTOM_RIGHT.value), 1),
        # ("Control:{}".format(Buttons.TOP_RIGHT.value), 0.5),
        # ("Control:{}".format(Buttons.BOTTOM_RIGHT.value), 2),
        # ("Control:{}".format(Buttons.BOTTOM_RIGHT.value), 1),
    ]

    if args.address == None:
        devices = zsw_list_uart_devices(timeout=int(args.timeout))
        print("Found {0} ZSWatch:es".format(len(devices)))
        print("Note, not all of these might be ZSWatch:es, TODO")
        print(devices)
        # if len(devices) > 0:
        #    clients = asyncio.run(zsw_connect_nus(list(devices.values())))
        #    print("Connected to {0} devices".format(len(clients)))
        #    zsw_send_nus_commands(clients, commands)
    else:
        clients = zsw_connect_nus(args.address)
        print("Connected to {0} devices".format(len(clients)))
        zsw_send_nus_commands(clients, commands)

    print("Disconnecting...")
    sys.exit(0)
    # for client in clients:
    #    asyncio.run(zsw_disconnect(client))
    # zsw_disconnect(clients)
    print("All commands sent!")

# asyncio.set_event_loop_policy(asyncio.WindowsSelectorEventLoopPolicy())
# asyncio.run(main())
