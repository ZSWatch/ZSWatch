# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

import tkinter as tk
import time, sys, argparse
import _thread
import threading
import time
from enum import Enum

from zswatch_ble_control import (
    zsw_connect_nus,
    zsw_send_nus_commands,
    zsw_disconnect,
    zsw_list_uart_devices,
)


class Buttons(Enum):
    ENTER = 0
    UP = 1
    BACK = 2
    DOWN = 3


NOT_USED = [1]


class UIController:
    def __init__(self, clients):
        self.is_enabled = False
        self.window = tk.Tk()
        self.window.title("ZSWatch controller")
        self.window.config(bg="#202124")
        self.clients = clients

        self.create_ui()

    def simulate_button_press(self, button):
        button.config(relief="sunken")

    def simulate_button_idle(self, button):
        button.config(relief="raised")

    def left(self, event=None):
        if event != None:
            self.simulate_button_press(self.left_btn)
            self.window.after(100, self.simulate_button_idle, self.left_btn)
            self.left_btn.invoke()
        else:
            zsw_send_nus_commands(
                self.clients, [("Control:{}".format(Buttons.BACK.value), 0)]
            )
        pass

    def right(self, event=None):
        if event != None:
            self.simulate_button_press(self.right_btn)
            self.window.after(100, self.simulate_button_idle, self.right_btn)
            self.right_btn.invoke()
        else:
            zsw_send_nus_commands(
                self.clients, [("Control:{}".format(Buttons.ENTER.value), 0)]
            )
        pass

    def up(self, event=None):
        if event != None:
            self.simulate_button_press(self.up_btn)
            self.window.after(100, self.simulate_button_idle, self.up_btn)
            self.up_btn.invoke()
        else:
            zsw_send_nus_commands(
                self.clients, [("Control:{}".format(Buttons.UP.value), 0)]
            )
        pass

    def down(self, event=None):
        if event != None:
            self.simulate_button_press(self.down_btn)
            self.window.after(100, self.simulate_button_idle, self.down_btn)
            self.down_btn.invoke()
        else:
            zsw_send_nus_commands(
                self.clients, [("Control:{}".format(Buttons.DOWN.value), 0)]
            )
        pass

    def enable(self, event=None):
        if event != None:
            self.simulate_button_press(self.enable_button)
            self.window.after(100, self.simulate_button_idle, self.enable_button)
            self.enable_button.invoke()
        else:
            if self.is_enabled:
                self.enable_button["bg"] = "green"
                self.enable_button["text"] = "▶"
            else:
                self.enable_button["bg"] = "red"
                self.enable_button["text"] = "⛌"

            self.is_enabled = not self.is_enabled
            if self.is_enabled:
                notify = 'GB({t:"notify",id:15,src:"Gmail",title:"jakob@mail.se",sender:"Jakob",body:"This is cool!"})'
                print(self.clients)
                zsw_send_nus_commands(self.clients, [(notify, 0)])
                pass
            else:
                # Send command to ZSWatch
                pass

    def reset_watch_state(self, event=None):
        if event != None:
            self.simulate_button_press(self.reset_state_btn)
            self.window.after(100, self.simulate_button_idle, self.reset_state_btn)
            self.reset_state_btn.invoke()
        else:
            # Send command to ZSWatch
            zsw_send_nus_commands(self.clients, [("Control:4", 1)])
            pass

    def create_ui(self):
        self.window.geometry("350x275")
        paddings = {"padx": 5, "pady": 5}

        self.left_btn = tk.Button(self.window, text="←", command=self.left)
        self.left_btn.grid(row=2, column=1, padx=2, pady=2)

        self.right_btn = tk.Button(self.window, text="→", command=self.right)
        self.right_btn.grid(row=2, column=3, padx=2, pady=2)

        self.up_btn = tk.Button(self.window, text="↑", command=self.up)
        self.up_btn.grid(row=1, column=2, padx=2, pady=2)

        self.down_btn = tk.Button(self.window, text="↓", command=self.down)
        self.down_btn.grid(row=3, column=2, padx=2, pady=2)

        self.enable_button = tk.Button(
            self.window, text="▶", height=1, width=2, command=self.enable
        )
        self.enable_button.grid(row=2, column=2)
        self.enable_button.config(bg="green", fg="white")

        self.reset_state_btn = tk.Button(
            self.window, text="State Reset", height=1, command=self.reset_watch_state
        )
        self.reset_state_btn.grid(row=2, column=5)
        self.reset_state_btn.config(bg="#1887AB", fg="white")

        self.rotation_amount = tk.StringVar(self.window)
        self.rotation_amount.set(NOT_USED[0])  # default value

        self.todo_label = tk.Label(self.window, text="TODO use")
        self.todo_label.grid(row=4, column=5)
        self.todo_label.config(bg="#202124", fg="white")

        self.dropdown = tk.OptionMenu(self.window, self.rotation_amount, *NOT_USED)
        self.dropdown.config(bg="dark gray", width=5)
        self.dropdown.grid(row=5, column=5, **paddings)

        col_count, row_count = self.window.grid_size()

        for col in range(col_count):
            self.window.grid_columnconfigure(col, minsize=20)

        for row in range(row_count):
            self.window.grid_rowconfigure(row, minsize=20)

        self.window.bind("<Left>", self.left)
        self.window.bind("<Right>", self.right)
        self.window.bind("<Up>", self.up)
        self.window.bind("<Down>", self.down)
        self.window.bind("<space>", self.enable)
        self.window.mainloop()


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="ZSWatch Controller over BLE NUS")
    parser.add_argument(
        "--addresses",
        dest="addresses",
        nargs="+",
        # default=["E1:D8:5E:37:D6:BE" "E8:F4:FB:CE:C7:83" "CB:70:60:77:8D:26" "F8:C2:75:80:21:CF" "F2:33:CD:87:D5:AD" "DA:7D:BD:21:90:38" "EF:93:74:5C:E7:B1" ],
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

    addresses = args.addresses
    if addresses == None:
        addresses = zsw_list_uart_devices(int(args.timeout))
        addresses = list(addresses.keys())

    clients = zsw_connect_nus(addresses)
    print("Send reset state command to all devices")
    weather = 'GB({t:"weather",temp:296,hum:55,code:802,txt:"slightly cloudy",wind:2.0,wdir:14,loc:"MALMO"})'
    zsw_send_nus_commands(
        clients,
        [
            ("Control:4", 1),
            ("GB(setTime({}))".format(int(time.time())), 0),
            (weather, 1),
        ],
    )
    print("Connected to {0} devices".format(len(clients)), clients)

    ui = UIController(clients)
