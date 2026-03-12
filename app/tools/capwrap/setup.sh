# Build capwrap and set required file capabilities.
# Run once after cloning (or whenever capwrap.c changes).
#
# Usage:  ./app/tools/capwrap/setup.sh
#
# Requires: gcc, libcap-dev (Debian/Ubuntu) or libcap-devel (Fedora)
#           sudo for setcap

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
SRC="$SCRIPT_DIR/capwrap.c"
OUT="$SCRIPT_DIR/capwrap"

echo "-- Building capwrap..."
gcc -O2 -o "$OUT" "$SRC" -lcap

echo "-- Setting file capabilities (requires sudo)..."
sudo setcap cap_net_admin,cap_net_raw,cap_setpcap+ep "$OUT"

echo "-- Verifying..."
getcap "$OUT"

echo ""
echo "Done. You can now run the native simulator without sudo:"
echo "  $OUT app/build/app/zephyr/zephyr.exe --bt-dev=hci0"
