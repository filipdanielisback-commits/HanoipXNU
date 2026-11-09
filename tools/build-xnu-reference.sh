#!/bin/sh
set -eu

EXPECTED_COMMIT="8d741a5de7ff4191bf97d57b9f54c2f6d4a15585"
SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
PROJECT_ROOT=$(CDPATH= cd -- "$SCRIPT_DIR/.." && pwd)
XNU_SRC="${XNU_CHECKOUT_DIR:-$PROJECT_ROOT/../xnu-upstream-11215.1.10}"

if [ "$(uname -s)" != "Darwin" ]; then
    echo "XNU reference build requires a macOS host; found $(uname -s)." >&2
    exit 2
fi

command -v xcrun >/dev/null 2>&1 || {
    echo "Xcode command-line tools (xcrun) are required." >&2
    exit 2
}
command -v make >/dev/null 2>&1 || {
    echo "make is required." >&2
    exit 2
}

if [ ! -d "$XNU_SRC/.git" ]; then
    echo "Separate upstream checkout not found: $XNU_SRC" >&2
    echo "Run sh tools/checkout-xnu.sh first." >&2
    exit 2
fi

ACTUAL_COMMIT=$(git -C "$XNU_SRC" rev-parse HEAD)
if [ "$ACTUAL_COMMIT" != "$EXPECTED_COMMIT" ]; then
    echo "Unexpected XNU revision: $ACTUAL_COMMIT" >&2
    echo "Expected: $EXPECTED_COMMIT" >&2
    exit 1
fi

if [ -n "$(git -C "$XNU_SRC" status --porcelain)" ]; then
    echo "Upstream checkout is not clean; refusing to build modified source." >&2
    exit 1
fi

if [ -z "${SDKROOT:-}" ]; then
    SDKROOT=$(xcrun --sdk macosx --show-sdk-path)
fi
if [ ! -d "$SDKROOT" ]; then
    echo "SDKROOT does not name an available SDK directory: $SDKROOT" >&2
    exit 2
fi

echo "Building untouched XNU $EXPECTED_COMMIT"
echo "Host: $(sw_vers -productVersion)"
echo "SDKROOT: $SDKROOT"
echo "Architecture: ARM64"
echo "Configuration: DEVELOPMENT"

exec make -C "$XNU_SRC" SDKROOT="$SDKROOT" ARCH_CONFIGS=ARM64 KERNEL_CONFIGS=DEVELOPMENT
