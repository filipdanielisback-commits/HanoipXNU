# iOS 18 XNU target and public-source audit

**Audit date:** 2026-11-09  
**Project:** HanoipXNU / Motorola Moto G60 XT2135-2  
**Status:** public research baseline pinned; exact iOS 18 match not established; reference build blocked by current host

## 1. Target identity

The concrete kernel identifier used for this comparison is from iOS 18 beta build `22A5316k`:

- Darwin kernel family: `24.0.0`
- Reported XNU build: `xnu-11215.0.165.502.1~2`

This is a specific beta build reference, not proof that every iOS 18 release has an identical kernel build. The identifier is reported in a contemporaneous beta-build record; it is not an Apple-published source archive or a source commit.

## 2. Public Apple source comparison

| Candidate | Public provenance | Comparison | Decision |
|---|---|---|---|
| `xnu-11215.1.10` | Apple OSS XNU repository; imported 2024-09-24; commit `8d741a5de7ff4191bf97d57b9f54c2f6d4a15585` | Same `11215` version family as the iOS 18 reference; the version string differs from `11215.0.165.502.1~2` | **Selected as public research baseline only** |
| `xnu-12377.1.9` | Apple OSS XNU repository; imported 2025-10-16; commit `f6217f891ac0bb64f3d375211650a4c1ff8ca1ea` | Later version generation; not a close version-family match to the iOS 18 reference | Not selected |

Apple's current public XNU README describes XNU as part of Darwin for macOS and iOS and lists ARM64 support. That establishes that the public source is relevant to Darwin kernel research; it does not establish that the public repository contains the exact iOS 18 product source, all iOS-specific changes, or the proprietary platform dependencies.

## 3. Selection

Pin `xnu-11215.1.10` at commit `8d741a5de7ff4191bf97d57b9f54c2f6d4a15585` for reproducible source review and a reference-build experiment.

**Important distinction:** this is the closest public candidate identified in this audit, not a verified exact source match for iOS 18 beta build `22A5316k`. The target and candidate are in the same version family but have different version strings. The project must preserve that distinction in documentation and future claims.

The later `xnu-12377.1.9` candidate is rejected for this baseline because it is a newer generation, not because its code is known to be unusable in all contexts.

## 4. Separate upstream checkout preparation

Two project-owned scripts have been added:

- `tools/checkout-xnu.sh`: clones the selected tag into a sibling directory outside the HanoipXNU Git tree and verifies the full commit SHA.
- `tools/build-xnu-reference.sh`: checks for macOS, an SDK, the exact pinned commit, and a clean checkout before invoking the ARM64 DEVELOPMENT build.

The upstream tree remains separate. HanoipXNU patches belong under `xnu/patches/`; do not edit the upstream checkout as the project patch store.

## 5. Clean reference-build attempt

The available execution environment was inspected on 2026-11-09:

- Host: Linux x86_64
- Git: 2.47.3
- Make: available
- Clang: 17.0.0
- `xcodebuild`: unavailable
- macOS SDK: unavailable

A shallow clone attempt for tag `xnu-11215.1.10` was made, but failed before downloading source because the environment could not resolve `github.com` (DNS/network failure). The build itself could not be invoked: the host is Linux rather than macOS and has no Xcode/macOS SDK.

**Result: build not run; no build success or compiler failure is claimed.** This is an environment limitation, not evidence that the pinned source fails to build.

On a macOS host, run:

```sh
sh tools/checkout-xnu.sh
sh tools/build-xnu-reference.sh
```

The build script defaults to the public macOS SDK returned by `xcrun --sdk macosx --show-sdk-path`. Apple's upstream README also shows `SDKROOT=macosx.internal` in its examples. That internal SDK may not be available to ordinary external developers; do not assume it is. Record the exact SDK, Xcode/Clang versions, command, log, and result for any future attempt.

## 6. Scope and limitations

This reference build is for an untouched public XNU ARM64 source tree on its supported host environment. It is not:

- a build for the Moto G60's Qualcomm SM6150 platform;
- a kernel boot test;
- a device-tree or bootloader integration test;
- proof of display, Wi-Fi, Bluetooth, audio, power, storage, or sensor support;
- an iOS build or a complete Darwin userland build.

No phone partitions or firmware are touched by this audit. No device flashing is part of the milestone.

## 7. Sources

- Apple XNU repository: https://github.com/apple-oss-distributions/xnu
- Selected tag: https://github.com/apple-oss-distributions/xnu/tree/xnu-11215.1.10
- Selected immutable commit: https://github.com/apple-oss-distributions/xnu/commit/8d741a5de7ff4191bf97d57b9f54c2f6d4a15585
- Later candidate commit: https://github.com/apple-oss-distributions/xnu/commit/f6217f891ac0bb64f3d375211650a4c1ff8ca1ea
- Apple XNU README and build instructions: https://github.com/apple-oss-distributions/xnu/blob/xnu-11215.1.10/README.md
- iOS 18 beta 4 build/kernel identifier record: https://forums.macrumors.com/threads/ios-18-beta-4-bug-fixes-changes-and-improvements.2432049/
