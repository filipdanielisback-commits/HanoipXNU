# XNU integration

This directory is HanoipXNU's integration layer for Apple's public XNU source. It is not a replacement kernel and does not contain a copied XNU tree.

## Layout

- `upstream/README.md` records how the external, version-pinned upstream checkout is managed.
- `patches/README.md` defines how project-authored changes are kept separate and reviewable.
- `REVISION.md` will record the exact upstream commit, release tag, license files, and source provenance once selected.

The upstream source must be obtained from Apple's public repository and kept as a separate checkout or reproducible external dependency. Do not copy a whole upstream source tree into this repository until its licensing, provenance, and update process have been reviewed.

## Version policy

The project has **not** pinned an upstream XNU revision yet. Do not call the target "iOS 18 XNU" merely because an XNU snapshot is used by a Darwin 24-era Apple operating system. iOS 18's kernel build and a public macOS XNU source tag are not automatically identical.

The version-selection record must compare:
- XNU version/build identifiers from the intended iOS 18 release, when independently documented;
- Apple-published XNU tags and commits;
- source-level differences and build requirements;
- available ARM64 platform code and configuration;
- required SDKs, private headers, and generated dependencies.

Until that audit is complete, use the phrase **public XNU baseline under evaluation**.

## First integration milestones

1. Select and pin a verifiable public XNU commit.
2. Reproduce the upstream source checkout and license inventory.
3. Identify the minimum ARM64 build environment and whether a public SDK is sufficient.
4. Build an unmodified reference configuration where supported.
5. Keep HanoipXNU platform changes in small, documented patches.
6. Add platform code only after its hardware facts are verified.

A folder existing here does not mean a kernel builds, boots, or supports the Moto G60.
