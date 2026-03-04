# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20)
project(capwrap C)

add_executable(capwrap capwrap.c)
target_link_libraries(capwrap PRIVATE cap)
