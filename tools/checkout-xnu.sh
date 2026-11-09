#!/bin/sh
set -eu

TAG="xnu-11215.1.10"
EXPECTED_COMMIT="8d741a5de7ff4191bf97d57b9f54c2f6d4a15585"
URL="https://github.com/apple-oss-distributions/xnu.git"

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
PROJECT_ROOT=$(CDPATH= cd -- "$SCRIPT_DIR/.." && pwd)
DEST="${XNU_CHECKOUT_DIR:-$PROJECT_ROOT/../xnu-upstream-11215.1.10}"

if [ -e "$DEST" ]; then
    echo "Refusing to overwrite existing path: $DEST" >&2
    echo "Choose a new XNU_CHECKOUT_DIR or inspect/remove that path yourself." >&2
    exit 2
fi

command -v git >/dev/null 2>&1 || {
    echo "git is required." >&2
    exit 2
}

echo "Cloning Apple XNU tag $TAG into $DEST"
git clone --depth 1 --branch "$TAG" "$URL" "$DEST"

ACTUAL_COMMIT=$(git -C "$DEST" rev-parse HEAD)
if [ "$ACTUAL_COMMIT" != "$EXPECTED_COMMIT" ]; then
    echo "Pinned revision mismatch." >&2
    echo "Expected: $EXPECTED_COMMIT" >&2
    echo "Actual:   $ACTUAL_COMMIT" >&2
    exit 1
fi

echo "Verified upstream commit: $ACTUAL_COMMIT"
echo "Upstream checkout is separate from the HanoipXNU working tree."
