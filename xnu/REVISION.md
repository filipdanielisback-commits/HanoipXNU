# Upstream XNU revision record

- Upstream: https://github.com/apple-oss-distributions/xnu
- Research baseline tag: `xnu-11215.1.10`
- Immutable commit: `8d741a5de7ff4191bf97d57b9f54c2f6d4a15585`
- Commit date: 2024-09-24
- iOS 18 reference family: Darwin 24 / XNU 11215
- Exact iOS 18 source match: **not established**
- Clean reference build: **not yet run; host prerequisites unavailable**
- Last reviewed: 2026-11-09

## Selection decision

Pin `xnu-11215.1.10` as the project's **public research baseline** because it is an Apple-published XNU snapshot in the same `11215` version family as the identified iOS 18 reference kernel. This is a source-control pin for reproducible research, not a claim that this snapshot is the exact source used by a particular iOS 18 release.

The iOS 18 beta 4 record reviewed here lists build `22A5316j` and a later revised build `22A5316k`, alongside Darwin `24.0.0` and kernel version `xnu-11215.0.165.502.1~2`. The exact build-to-kernel pairing is not independently established by that record. The public `xnu-11215.1.10` snapshot is nearby in the 11215 generation, but its version string differs. No evidence found in this audit establishes a one-to-one source match between the public tag and an iOS 18 build.

The newer public `xnu-12377.1.9` snapshot is a later version generation and is not selected as the iOS 18 research baseline.

## Provenance

- Tag: https://github.com/apple-oss-distributions/xnu/tree/xnu-11215.1.10
- Immutable commit: https://github.com/apple-oss-distributions/xnu/commit/8d741a5de7ff4191bf97d57b9f54c2f6d4a15585
- Apple XNU source README: https://github.com/apple-oss-distributions/xnu/blob/xnu-11215.1.10/README.md
- iOS 18 beta kernel identifier reference: https://forums.macrumors.com/threads/ios-18-beta-4-bug-fixes-changes-and-improvements.2432049/

## What this pin means

- It identifies a reproducible public XNU source snapshot for source review and reference-build experiments.
- It does not provide Apple's complete iOS userland, proprietary frameworks, signing services, firmware, private headers, device drivers, or platform configuration.
- It does not establish that the snapshot can boot on the Moto G60 or that the target hardware is supported.
- It must not be represented as a working iOS 18 port.

## Separate checkout

Use `sh tools/checkout-xnu.sh` to clone the pinned source into a sibling directory outside this repository. The script verifies the resulting commit SHA. Keep upstream source separate from HanoipXNU-owned patches and documentation.

## Reference build

On a supported macOS development host with Xcode command-line tools and an available macOS SDK, run:

```sh
sh tools/build-xnu-reference.sh
```

The script builds the untouched pinned checkout for ARM64 using the public macOS SDK selected by `xcrun`, unless `SDKROOT` is explicitly set. Apple's README also documents `macosx.internal`; that internal SDK is not assumed to be publicly available. A build failure with the public SDK must be recorded as an environment/source-build limitation, not silently worked around by substituting private files.

## Revisit criteria

Reopen this decision if Apple publishes a verifiable XNU source snapshot matching the exact iOS 18 kernel build, or if a better-provenance source archive becomes available. Record the new tag, full commit SHA, source URL, comparison evidence, and build environment before changing the pin.
