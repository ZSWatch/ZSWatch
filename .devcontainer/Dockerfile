FROM ghcr.io/zephyrproject-rtos/ci:v0.26.2

ENV PKG_CONFIG_PATH=/usr/lib/i386-linux-gnu/pkgconfig

RUN apt-get -y update
RUN apt-get -y install astyle dos2unix nano curl

# Install the dependencies for POSIX build
RUN apt-get -y install pkg-config libsdl2-dev:i386

# Install the dependencies for Segger J-Link
RUN apt-get -y install libxcb-icccm4 libxcb-image0 libxcb-keysyms1 libxcb-randr0 libxcb-render-util0 libxcb-shape0 libxcb-util1 libxcb-xkb1 libxkbcommon-x11-0

# Install the dependencies for POSIX GUI
RUN apt-get -y install --allow-downgrades libgl1-mesa-dri=22.0.1-1ubuntu2 \
    libglx-mesa0=22.0.1-1ubuntu2 \
    libgl1-mesa-glx=22.0.1-1ubuntu2 \
    libglapi-mesa=22.0.1-1ubuntu2 \
    libgbm1=22.0.1-1ubuntu2 \
    libegl-mesa0=22.0.1-1ubuntu2 \
    libgl1-mesa-dri=22.0.1-1ubuntu2 \
    libglx-mesa0=22.0.1-1ubuntu2

RUN pip install --upgrade pip
RUN pip install codechecker

COPY init.sh /tmp/init.sh
RUN chmod +x /tmp/init.sh