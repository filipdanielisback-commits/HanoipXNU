# Build Environment

## XNU public reference build

The current public research baseline is recorded in [`xnu/REVISION.md`](../xnu/REVISION.md). It is an Apple-published XNU snapshot in the iOS 18 / Darwin 24 `11215` version family, not a verified exact iOS 18 source match.

The upstream XNU README documents a Make-based kernel build and uses `macosx.internal` in its example commands. That internal SDK should not be assumed to be available to external developers. The project build script instead defaults to the public macOS SDK path returned by `xcrun --sdk macosx --show-sdk-path`; this is an initial reference-build attempt and may expose missing public SDK dependencies.

### Prerequisites

- macOS host
- Xcode command-line tools (`xcrun`, compiler toolchain)
- `make`
- An installed macOS SDK
- Git

### Steps

From the HanoipXNU repository root:

```sh
sh tools/checkout-xnu.sh
sh tools/build-xnu-reference.sh
```

The checkout script places upstream source in a sibling directory outside this Git repository and verifies its immutable commit SHA. The build script verifies that the checkout is at the expected commit and has no local modifications before invoking the ARM64 DEVELOPMENT target.

To choose a specific SDK directory, set `SDKROOT` before running the build script. To choose a different checkout directory, set `XNU_CHECKOUT_DIR` for both scripts.

### Current environment result (2026-11-09)

The available execution host is Linux x86_64 with Git 2.47.3, Make, and Clang 17. It has no `xcodebuild` or macOS SDK. A shallow clone attempt could not resolve `github.com`, so source download failed before compilation. Therefore no clean build was run and no build result is claimed.

Do not treat a Linux-host preflight failure or network/DNS failure as an XNU compiler failure. Re-run the documented experiment on macOS and preserve the exact host, SDK, compiler, command, and log.

No device flashing, partition modification, or phone-side testing is part of this reference build.
