#!/usr/bin/env bash
# This script must be run from the project root!

JLINK_VERSION=JLink_Linux_V794b_x86_64

RED="\e[31m"
GREEN="\e[32m"
ENDCOLOR="\e[0m"
TEMP_DIR="tmp"

mkdir ${TEMP_DIR}

echo -e "${GREEN}Enable udev daemon${ENDCOLOR}"
apt -y install udev
/lib/systemd/systemd-udevd --daemon

echo -e "${GREEN}Install J-Link driver${ENDCOLOR}"
echo -e "${RED}Please note: You automatically accept the Segger license terms when you run this installation!${ENDCOLOR}"
JLINK_DEB=${TEMP_DIR}/JLink_Linux_x86_64.deb
curl -s -X POST https://www.segger.com/downloads/jlink/${JLINK_VERSION}.deb H "Content-Type: application/x-www-form-urlencoded" -d "accept_license_agreement=accepted" >$JLINK_DEB
dpkg -i  $JLINK_DEB

echo -e "${GREEN}Initialize project${ENDCOLOR}"
git config --global --add safe.directory '*'
git submodule update --init --recursive
cd app
west init -l .
west update

echo -e "${GREEN}Add build configurations${ENDCOLOR}"
west build --build-dir /workspaces/ZSWatch/app/debug_rev4 /workspaces/ZSWatch/app --pristine --board zswatch_nrf5340_cpuapp@4 --no-sysbuild -- -DNCS_TOOLCHAIN_VERSION=NONE -DBOARD_ROOT=/workspaces/ZSWatch/app;. -DCONF_FILE=/workspaces/ZSWatch/app/prj.conf -DEXTRA_CONF_FILE=/workspaces/ZSWatch/app/boards/debug.conf
west build --build-dir /workspaces/ZSWatch/app/release_rev4 /workspaces/ZSWatch/app --pristine --board zswatch_nrf5340_cpuapp@4 --no-sysbuild -- -DNCS_TOOLCHAIN_VERSION=NONE -DBOARD_ROOT=/workspaces/ZSWatch/app;. -DCONF_FILE=/workspaces/ZSWatch/app/prj.conf -DEXTRA_CONF_FILE=/workspaces/ZSWatch/app/boards/release.conf

echo -e "${GREEN}Clean up${ENDCOLOR}"

rm -rf ${PWD}/${TEMP_DIR}