# Research and build tools

HanoipXNU-owned scripts for source provenance, revision checks, configuration validation, host tests, and reproducible builds belong here.

## Pinned upstream XNU checkout

- `checkout-xnu.sh` clones the Apple-published `xnu-11215.1.10` tag into a sibling directory outside this repository and verifies commit `8d741a5de7ff4191bf97d57b9f54c2f6d4a15585`.
- `build-xnu-reference.sh` performs host, SDK, source revision, and clean-tree checks before invoking the upstream ARM64 DEVELOPMENT build.

Run from the repository root:

```sh
sh tools/checkout-xnu.sh
sh tools/build-xnu-reference.sh
```

See [the XNU revision record](../xnu/REVISION.md), [the iOS 18 source audit](../docs/ios18-xnu-audit.md), and [build environment notes](../build/README.md).

Every tool must document inputs, outputs, dependencies, and whether it reads or changes a connected device. These scripts operate on local source trees only. They do not access or modify a phone.

No phone-writing, partition-modifying, or firmware extraction tool is authorized by this directory.
