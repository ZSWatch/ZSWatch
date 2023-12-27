#!/usr/bin/env bash

JLINK_VERSION=JLink_Linux_V794b_x86_64
TEMP_DIR="/tmp"
CURRENT_DIR=${PWD}

RED="\e[31m"
GREEN="\e[32m"
ENDCOLOR="\e[0m"

mkdir ${TEMP_DIR}/downloads

if [ -z "${BUILD_ASTYLE}" ]; then
    echo -e "${GREEN}Install AStyle 3.4${ENDCOLOR}"
    cd ${TEMP_DIR}/downloads
    wget https://sourceforge.net/projects/astyle/files/astyle/astyle%203.4/astyle-3.4.10.tar.bz2/download -O astyle.tar.bz2
    tar -xf astyle.tar.bz2
    cd astyle-3.4.10
    mkdir as-gcc-exe
    cd as-gcc-exe
    cmake ../
    make
    make install
else
    echo -e "${GREEN}Install AStyle with apt${ENDCOLOR}"
    apt install -y astyle
fi

echo -e "${GREEN}Install dependencies for POSIX GUI${ENDCOLOR}"
dpkg --add-architecture i386
apt update
apt -y install pkg-config libsdl2-dev:i386
export PKG_CONFIG_PATH=/usr/lib/i386-linux-gnu/pkgconfig

if [ ! -f "`which nrfjprog`" ]; then
	echo -e "${GREEN}Install nRF Command Line Tools${ENDCOLOR}"
	NRF_CMD_LINE_DEB=${TEMP_DIR}/downloads/JLink_Linux_x86_64.deb
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
cd ${CURRENT_DIR}

if [ ! -z "${INIT_POSIX}" ];
then
    echo -e "${GREEN}   Add POSIX${ENDCOLOR}"
    west build --build-dir ${PWD}/app/native_posix ${PWD}/app --pristine --board native_posix -- -DOVERLAY_CONFIG=boards/native_posix.conf
else
    echo -e "${GREEN}   Skip POSIX${ENDCOLOR}"
fi

if [ ! -z "${INIT_REV_4}" ];
then
    echo -e "${GREEN}   Add hardware revision 4${ENDCOLOR}"
    west build --build-dir ${PWD}/app/debug_rev4 ${PWD}/app --pristine --board zswatch_nrf5340_cpuapp@4 --no-sysbuild -- -DNCS_TOOLCHAIN_VERSION=NONE -DBOARD_ROOT=/workspaces/ZSWatch/app -DCONF_FILE=/workspaces/ZSWatch/app/prj.conf -DEXTRA_CONF_FILE=${PWD}/app/boards/debug.conf
    west build --build-dir ${PWD}/app/release_rev4 ${PWD}/app --pristine --board zswatch_nrf5340_cpuapp@4 --no-sysbuild -- -DNCS_TOOLCHAIN_VERSION=NONE -DBOARD_ROOT=/workspaces/ZSWatch/app -DCONF_FILE=/workspaces/ZSWatch/app/prj.conf -DEXTRA_CONF_FILE=${PWD}/app/boards/release.conf
else
    echo -e "${GREEN}   Skip revision 4${ENDCOLOR}"
fi

if [ ! -z "${INIT_REV_3}" ];
then
    echo -e "${GREEN}   Add revision 3${ENDCOLOR}"
    west build --build-dir ${PWD}/app/debug_rev3 ${PWD}/app --pristine --board zswatch_nrf5340_cpuapp@3 --no-sysbuild -- -DNCS_TOOLCHAIN_VERSION=NONE -DBOARD_ROOT=/workspaces/ZSWatch/app -DCONF_FILE=/workspaces/ZSWatch/app/prj.conf -DEXTRA_CONF_FILE=${PWD}/app/boards/debug.conf
    west build --build-dir ${PWD}/app/release_rev3 ${PWD}/app --pristine --board zswatch_nrf5340_cpuapp@3 --no-sysbuild -- -DNCS_TOOLCHAIN_VERSION=NONE -DBOARD_ROOT=/workspaces/ZSWatch/app -DCONF_FILE=/workspaces/ZSWatch/app/prj.conf -DEXTRA_CONF_FILE=${PWD}/app/boards/release.conf
else
    echo -e "${GREEN}   Skip hardware revision 3${ENDCOLOR}"
fi

if [ ! -z "${INIT_REV_2}" ];
then
    echo -e "${GREEN}   Add revision 2${ENDCOLOR}"
    west build --build-dir ${PWD}/app/debug_rev2 ${PWD}/app --pristine --board zswatch_nrf5340_cpuapp@2 --no-sysbuild -- -DNCS_TOOLCHAIN_VERSION=NONE -DBOARD_ROOT=/workspaces/ZSWatch/app -DCONF_FILE=/workspaces/ZSWatch/app/prj.conf -DEXTRA_CONF_FILE=${PWD}/app/boards/debug.conf
    west build --build-dir ${PWD}/app/release_rev2 ${PWD}/app --pristine --board zswatch_nrf5340_cpuapp@2 --no-sysbuild -- -DNCS_TOOLCHAIN_VERSION=NONE -DBOARD_ROOT=/workspaces/ZSWatch/app -DCONF_FILE=/workspaces/ZSWatch/app/prj.conf -DEXTRA_CONF_FILE=${PWD}/app/boards/release.conf
else
    echo -e "${GREEN}   Skip hardware revision 2${ENDCOLOR}"
fi

if [ ! -z "${INIT_REV_1}" ];
then
    echo -e "${GREEN}   Add revision 1${ENDCOLOR}"
    west build --build-dir ${PWD}/app/debug_rev1 ${PWD}/app --pristine --board zswatch_nrf5340_cpuapp@1 --no-sysbuild -- -DNCS_TOOLCHAIN_VERSION=NONE -DBOARD_ROOT=/workspaces/ZSWatch/app -DCONF_FILE=/workspaces/ZSWatch/app/prj.conf -DEXTRA_CONF_FILE=${PWD}/app/boards/debug.conf
    west build --build-dir ${PWD}/app/release_rev1 ${PWD}/app --pristine --board zswatch_nrf5340_cpuapp@1 --no-sysbuild -- -DNCS_TOOLCHAIN_VERSION=NONE -DBOARD_ROOT=/workspaces/ZSWatch/app -DCONF_FILE=/workspaces/ZSWatch/app/prj.conf -DEXTRA_CONF_FILE=${PWD}/app/boards/release.conf
else
    echo -e "${GREEN}   Skip hardware revision 1${ENDCOLOR}"
fi

echo -e "${GREEN}Clean up${ENDCOLOR}"
touch ~/.zswatch_initialized
rm -rf ${TEMP_DIR}/downloads