#!/bin/bash
SELF_PATH=$(realpath -s "$0")

if [[ "$SUDO_ASKPASS" = "$SELF_PATH" ]]; then
    zenity --password --title="$1"
else
    exec env SUDO_ASKPASS="$SELF_PATH" sudo -E -A /usr/bin/gdb $@
fi