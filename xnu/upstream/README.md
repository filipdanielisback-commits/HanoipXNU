# Upstream XNU checkout

The upstream source is maintained by Apple:

- https://github.com/apple-oss-distributions/xnu

## Pinned research baseline

- Tag: `xnu-11215.1.10`
- Commit: `8d741a5de7ff4191bf97d57b9f54c2f6d4a15585`
- Role: public source snapshot in the iOS 18 / Darwin 24 XNU 11215 version family
- Exact iOS 18 source equivalence: **not established**

This checkout is deliberately kept separate from the HanoipXNU repository. Do not copy the full upstream tree into this repository or edit upstream files as if they were project-owned. Keep HanoipXNU-specific changes as reviewed patches under `xnu/patches/`.

From the HanoipXNU repository root:

```sh
sh tools/checkout-xnu.sh
```

By default, this creates `../xnu-upstream-11215.1.10`. Override the destination with `XNU_CHECKOUT_DIR=/absolute/path`.

The script verifies the immutable commit after cloning. If verification fails, stop and investigate rather than building an unexpected revision.

## Build experiment

On macOS with Xcode command-line tools and an available macOS SDK:

```sh
sh tools/build-xnu-reference.sh
```

The script checks the pinned commit, a clean source tree, host OS, and SDK before invoking Apple's documented ARM64 DEVELOPMENT build target. This is a host reference build only; it is not a Moto G60 kernel build and does not test device bootability.

See [the revision decision and audit](../REVISION.md) and [the iOS 18 compatibility audit](../../docs/ios18-xnu-audit.md).
