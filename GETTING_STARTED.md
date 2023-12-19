# Getting Started

- [Getting Started](#getting-started)
  - [Setting up the environment](#setting-up-the-environment)
    - [Using the development container (recommended)](#using-the-development-container-recommended)
    - [Without using the development container](#without-using-the-development-container)

## Setting up the environment

### Using the development container (recommended)

The repository contains a ready to use development container which can be used with the [Windows Subsystem for Linux 2](https://learn.microsoft.com/de-de/windows/wsl/install) or Linux in Visual Studio Code. Please follow the next steps to use this environment.

1. Download and install WSL2 if you use a Windows host
2. Download and install [Visual Studio Code](https://code.visualstudio.com/)
3. Download and install [J-Link Software](https://www.segger.com/downloads/jlink/)
4. Configure the IP adress for the host <> container communication in `.devcontainer/.env`
5. Open Visual Studio Code and install the [Dev Containers](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers) plugin
6. Open a Linux shell (WSL) or a regular shell (Linux) in the project directory and enter `code .` to open the project
7. Visual Studio Code is started now and load the directory. Now you should see a `Reopen in Container` window in the bottom right corner
8. Reopen the project in the container by clicking `Reopen in Container`
9. Wait until the container is started

### Without using the development container

Please follow the [`Start Developing`](https://github.com/jakkra/ZSWatch/wiki/Start-Developing) chapter in the Wiki when you don´t want to use the prepared development container.
