# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0
#
# ZSWatch LLEXT App Builder — Docker image
#
# Contains: Zephyr SDK (ARM toolchain only) + FastAPI server + Python
#
# Build:
#   docker build -t zswatch-app-builder .
#
# Run:
#   docker run -p 8000:8000 -v ./edks:/opt/edks zswatch-app-builder

FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive
ENV ZEPHYR_SDK_VERSION=0.17.0

# System dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
    python3 \
    python3-pip \
    python3-venv \
    wget \
    xz-utils \
    cmake \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# Install Zephyr SDK (ARM toolchain only — minimal install)
RUN wget -q "https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v${ZEPHYR_SDK_VERSION}/zephyr-sdk-${ZEPHYR_SDK_VERSION}_linux-x86_64_minimal.tar.xz" \
    -O /tmp/zephyr-sdk.tar.xz \
    && tar -xf /tmp/zephyr-sdk.tar.xz -C /opt/ \
    && cd /opt/zephyr-sdk-${ZEPHYR_SDK_VERSION} \
    && ./setup.sh -t arm-zephyr-eabi \
    && rm /tmp/zephyr-sdk.tar.xz

# Add toolchain to PATH
ENV ZEPHYR_SDK_INSTALL_DIR=/opt/zephyr-sdk-${ZEPHYR_SDK_VERSION}
ENV PATH="/opt/zephyr-sdk-${ZEPHYR_SDK_VERSION}/arm-zephyr-eabi/bin:${PATH}"

# Python dependencies
WORKDIR /app
COPY requirements.txt .
RUN pip3 install --no-cache-dir --break-system-packages -r requirements.txt

# Copy server code
COPY server/ ./server/

# EDK mount point
RUN mkdir -p /opt/edks
ENV EDK_BASE_DIR=/opt/edks

# Expose port
EXPOSE 8000

# Run server
CMD ["uvicorn", "server.main:app", "--host", "0.0.0.0", "--port", "8000"]
