FROM ghcr.io/zephyrproject-rtos/ci:v0.26.2

ENV PKG_CONFIG_PATH=/usr/lib/i386-linux-gnu/pkgconfig

RUN sudo apt-get -y update
RUN sudo apt-get -y install astyle dos2unix nano curl

# Install the dependencies for POSIX build
RUN sudo apt-get -y install pkg-config libsdl2-dev:i386

# Install the dependencies for Segger J-Link
RUN sudo apt-get -y install libxcb-icccm4 libxcb-image0 libxcb-keysyms1 libxcb-randr0 libxcb-render-util0 libxcb-shape0 libxcb-util1 libxcb-xkb1 libxkbcommon-x11-0

RUN pip install --upgrade pip
RUN pip install codechecker

RUN chmod +x init.sh