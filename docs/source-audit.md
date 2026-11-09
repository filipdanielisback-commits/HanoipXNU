# Source and Feasibility Audit

**Status:** Research draft  
**Device target:** Motorola Moto G60, XT2135-2 (`hanoip`)  
**Last reviewed:** 2026-11-09

This document separates verified source availability from assumptions. It does not claim that XNU boots on this phone, that iOS can be installed, or that any image is safe to flash.

## 1. What the public sources establish

| Area | Evidence found | What it does—and does not—establish |
|---|---|---|
| Hanoip Android device configuration | [motorola-sm6150-devs/device_motorola_hanoip](https://github.com/motorola-sm6150-devs/device_motorola_hanoip) identifies itself as an Android device tree for the Moto G40 Fusion / Moto G60 family. | Useful reference material for Android board configuration. It is not an XNU port or a complete hardware specification for every XT2135-2 variant. |
| Older Hanoip source collection | [moto-hanoip organization](https://github.com/moto-hanoip) is marked archived by its administrator. | Treat its repositories as historical references; do not assume they are maintained or current. |
| XNU source | [Apple's open-source XNU repository](https://github.com/apple-oss-distributions/xnu) describes XNU as the kernel used in Darwin and identifies Mach, BSD, IOKit, and platform-specific code in its source tree. | Public kernel source is only one part of an operating system. It does not provide a complete iOS installation, proprietary platform services, or Moto G60 drivers. |
| Third-party non-Apple porting work | [UniXNU porting overview](https://unixnu.com/porting/) describes a multi-layer approach involving boot firmware, a boot manager, an XNU loader, a kernel, and userland. | This is an external project's description of its own approach, not proof of Moto G60 support. Any claimed support must be checked against its device list, source history, reproducible build evidence, and actual test reports before being relied on. |

## 2. Important unknowns for XT2135-2

The following are **not yet verified for this exact handset variant**:

- Exact board revision and SoC/platform identifier.
- CPU feature set and the precise XNU architecture/profile requirements.
- Memory map, reserved regions, interrupt controller configuration, and device-tree details.
- Boot firmware behavior and documented handoff interface.
- Display, touch, storage, USB, audio, camera, Wi-Fi, Bluetooth, and power-management support.
- Availability and licensing of the firmware and operating-system components needed for a complete system.

Some community references use inconsistent platform labels or display specifications for the broader hanoip family. Do not resolve those disagreements by guessing. Record the exact source, device variant, and confidence level for every hardware fact.

## 3. Feasibility boundary

A successful XNU source build would prove only that a kernel was built for a selected configuration. It would **not** prove that it can boot on the Moto G60.

A genuine device port would require, at minimum:

1. A verified hardware and firmware inventory for XT2135-2.
2. A documented, compatible early-boot environment and kernel handoff.
3. Platform-specific XNU support and working drivers for essential hardware.
4. Compatible userland and system components, with lawful access and licensing.
5. Reproducible tests and logs showing what actually works.

The availability of Apple's XNU source does not make iOS 18 open source. This project must not describe a generic Darwin build, emulator, mock interface, or Android customization as iOS.

## 4. Safe research policy

- Keep the phone on its existing operating system while research is documentation-only.
- Do not publish or recommend a flashable image unless its origin, contents, compatibility, and recovery path have been independently verified.
- Do not claim a feature is working without a reproducible test on the exact target variant.
- Label every statement as **verified**, **reported**, **inferred**, or **unknown**.
- Keep third-party source trees separate and preserve their original licenses and attribution.

## 5. Next research tasks

- Obtain authoritative identification for the XT2135-2 board and platform.
- Read the Hanoip device tree and extract only facts that can be tied to a source file and revision.
- Compare the documented XNU architecture/platform configuration with the verified device CPU and platform facts.
- Check whether any external porting project explicitly lists XT2135-2 / hanoip as supported; do not infer support from generic ARM64 claims.
- Maintain a gap list rather than attempting a device build before the prerequisites are known.

## References

- Apple Open Source, XNU: https://github.com/apple-oss-distributions/xnu
- Hanoip Android device tree: https://github.com/motorola-sm6150-devs/device_motorola_hanoip
- Archived Hanoip source organization: https://github.com/moto-hanoip
- UniXNU porting overview (independent third-party project): https://unixnu.com/porting/
- UniXNU device-porting checklist (third-party guidance): https://unixnu.com/porting/steps
