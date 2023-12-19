#!/usr/bin/env bash

JLINK_VERSION=JLink_Linux_V794b_x86_64
TEMP_DIR="/tmp/downloads"
CURRENT_DIR=${PWD}

RED="\e[31m"
GREEN="\e[32m"
ENDCOLOR="\e[0m"

mkdir ${TEMP_DIR}

echo -e "${GREEN}Install dependencies for POSIX GUI${ENDCOLOR}"
dpkg --add-architecture i386
apt update
apt -y install pkg-config libsdl2-dev:i386
export PKG_CONFIG_PATH=/usr/lib/i386-linux-gnu/pkgconfig

if [ ! -f "`which nrfjprog`" ]; then
	echo -e "${GREEN}Install nRF Command Line Tools${ENDCOLOR}"
	NRF_CMD_LINE_DEB=${TEMP_DIR}/JLink_Linux_x86_64.deb
	wget https://nsscprodmedia.blob.core.windows.net/prod/software-and-other-downloads/desktop-software/nrf-command-line-tools/sw/versions-10-x-x/10-23-2/nrf-command-line-tools_10.23.2_amd64.deb -O ${NRF_CMD_LINE_DEB}
	dpkg -i ${NRF_CMD_LINE_DEB}
else
	echo -e "${GREEN}nRF Command Line Tools already installed!${ENDCOLOR}"
	nrfjprog --version 
fi

echo -e "${GREEN}Initialize project${ENDCOLOR}"
cd ${CURRENT_DIR}
git config --global --add safe.directory '*'
git submodule update --init --recursive
cd ${CURRENT_DIR}/app
west init -l .
west update

echo -e "${GREEN}Add build configurations${ENDCOLOR}"
west build --build-dir ${PWD}/debug_rev4 ${PWD}/app --pristine --board zswatch_nrf5340_cpuapp@4 --no-sysbuild -- -DNCS_TOOLCHAIN_VERSION=NONE -DBOARD_ROOT=/workspaces/ZSWatch/app -DCONF_FILE=/workspaces/ZSWatch/app/prj.conf -DEXTRA_CONF_FILE=${PWD}/app/boards/debug.conf
west build --build-dir ${PWD}/release_rev4 ${PWD}/app --pristine --board zswatch_nrf5340_cpuapp@4 --no-sysbuild -- -DNCS_TOOLCHAIN_VERSION=NONE -DBOARD_ROOT=/workspaces/ZSWatch/app -DCONF_FILE=/workspaces/ZSWatch/app/prj.conf -DEXTRA_CONF_FILE=${PWD}/app/boards/release.conf
west build --build-dir ${PWD}/native_posix ${PWD}/app --pristine --board native_posix -- -DOVERLAY_CONFIG=boards/native_posix.conf

echo -e "${GREEN}Clean up${ENDCOLOR}"
touch /tmp/initialized
rm -rf ${TEMP_DIR}