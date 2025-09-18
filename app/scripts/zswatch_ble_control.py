# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

"""
Commands over Nordic UART Service
"""

import asyncio

from bleak import BleakScanner, BleakClient, BleakError
import numpy as np
import time

import platform

if platform.system() == "Windows":
    asyncio.set_event_loop_policy(asyncio.WindowsSelectorEventLoopPolicy())

# Nordic UART Service UUIDs
UART_SERVICE_UUID = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
UART_RX_CHAR_UUID = "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
UART_TX_CHAR_UUID = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"


def handle_disconnect(device: BleakClient):
    print("______ZSWatch disconnected", device.address)
    pass


async def list_uart_devices(timeout=5.0):
    devices = {}

    def detection_callback(device, advertisement_data):
        if UART_SERVICE_UUID.lower() in advertisement_data.service_uuids:
            devices[device.address] = device

    scanner = BleakScanner()
    scanner.register_detection_callback(detection_callback)
    await scanner.start()
    await asyncio.sleep(timeout)
    await scanner.stop()
    print("Done", devices)
    return devices


async def send_nus_commands(clients, command_list):
    def handle_rx(_: int, data: bytearray):
        print(client, "received:", data)

    # Remove clients that are not connected using filter
    clients = list(filter(lambda item: item.is_connected, clients))
    print(clients)
    try:
        for client in clients:
            await client.start_notify(UART_TX_CHAR_UUID, handle_rx)
        for command in command_list:
            print("Send:", command)
            for client in clients:
                await client.write_gatt_char(UART_RX_CHAR_UUID, str.encode(command[0]))
            await asyncio.sleep(command[1])
        return True
    except Exception as e:
        print(e)
    return False


async def connect_to_device(address):
    print("Connecting", address)
    if isinstance(address, list):
        connected = []
        for addr in address:
            try:
                client = BleakClient(
                    addr, timeout=30.0, disconnected_callback=handle_disconnect
                )
                await client.connect()
                connected.append(client)
            except Exception as e:
                print(e)
        return connected
    else:
        for i in range(4):
            try:
                client = BleakClient(
                    address, timeout=10.0, disconnected_callback=handle_disconnect
                )
                await client.connect()
                await asyncio.sleep(2)
                print("ASDF", client.is_connected)
                if client.is_connected:
                    return client
                else:
                    print("Got quick disconnect", address)
            except Exception as e:
                print(e)
    return None


def zsw_connect_nus(devices):
    if not isinstance(devices, list):
        devices = [devices]

    loop = asyncio.new_event_loop()
    asyncio.set_event_loop(loop)
    if 1:
        group = asyncio.gather(*(connect_to_device(device) for device in devices))
        clients = loop.run_until_complete(group)
        loop.close()
        clients = list(filter(lambda item: item is not None, clients))
    else:
        clients = asyncio.run(connect_to_device(devices))
    return clients


def zsw_send_nus_commands(clients, command_list):
    loop = asyncio.new_event_loop()
    asyncio.set_event_loop(loop)
    asyncio.run(send_nus_commands(clients, command_list))
    # group = asyncio.gather(
    #    *(send_nus_commands(client, command_list) for client in clients)
    # )
    # results = loop.run_until_complete(group)
    # loop.close()


async def disconnect(client):
    await client.disconnect()


async def zsw_disconnect(client):
    await client.disconnect()


# def zsw_disconnect(clients):
#    loop = asyncio.new_event_loop()
#    asyncio.set_event_loop(loop)
#    group = asyncio.gather(
#        *(disconnect(client) for client in clients)
#    )
#    results = loop.run_until_complete(group)
#    loop.close()


def zsw_list_uart_devices(timeout):
    return asyncio.run(list_uart_devices(timeout))
