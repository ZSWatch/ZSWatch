# Dev-Container

## Table of Contents

- [Dev-Container](#dev-container)
  - [Table of Contents](#table-of-contents)
  - [About](#about)
  - [Prepare the host](#prepare-the-host)
    - [Additional steps when using WSL](#additional-steps-when-using-wsl)
  - [How to use it?](#how-to-use-it)
  - [Start a debug session](#start-a-debug-session)
  - [Maintainer](#maintainer)

## About

This development container contains all the neccessary tools to start with the developing and debugging of the ZSWatch software.

## Prepare the host

- Install [Visual Studio Code](https://code.visualstudio.com/)
- Install the [Dev Containers Plugin for VS Code](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers)
- Install [Segger J-Link Software](https://www.segger.com/downloads/jlink/) on your host
- Get the host IP address for `Docker` or `WSL`
  - Run `ipconfig` in **Windows** and search for the address from `Ethernet-Adapter vEthernet (WSL (Hyper-V firewall))`
  - Run `ifconfig` in **Linux** and search for the address from `docker0`
- Add the IP address to `TARGET_GDB_IP` in `.env`

### Additional steps when using WSL

- Install [WSL](https://learn.microsoft.com/en-us/windows/wsl/install)
- Install the [WSL Plugin for VS Code](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-wsl)
- Start VS Code on your host system
- Start a WSL session in VS Code to let VS Code install all dependencies

## How to use it?

- Start VS Code by executing `code .` in the project root of your Linux instance
- VS Code is showing a Window with `Reopen in Container` in the bottom right of your screen
- Reopen the directory in the container
- Wait until the initialization of the container is finished

## Start a debug session

- Start `J-Link GDB Server` on your host machine
- Open a new connection with your J-Link. **Make sure that you disable the `Localhost only` option under `Server settings`**
- Start a debug session over the nRF Connect plugin for the target debug build

## Maintainer

- [Daniel Kampert](mailto:daniel.kameprt@kampis-elektroecke.de)
