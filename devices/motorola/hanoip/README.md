# Motorola Moto G60 — hanoip

This directory is reserved for device-specific research for the Motorola Moto G60 target, model XT2135-2.

## Preliminary identity

Public Android device-tree repositories group the Moto G60 and Moto G40 Fusion under the codename family `hanoip`. A public TeamWin device configuration lists family-level specifications including Snapdragon 732G and Adreno 618. Exact hardware details for this XT2135-2 hardware revision remain to be verified.

## Research references

- [TeamWin Moto G40/G60 device configuration](https://github.com/TeamWin/android_device_motorola_hanoip)
- [motorola-sm6150-devs Android device tree](https://github.com/motorola-sm6150-devs/device_motorola_hanoip)
- [Archived community Android/Linux kernel tree](https://github.com/Dark-Matter7232/CosmicFresh-Hanoip)
- [Audio hardware audit](audio-hardware-audit.md)
- [Wireless interface audit](wireless-interface-audit.md)
- [Wi-Fi and Bluetooth feasibility plan](wifi-bluetooth-driver-plan.md)
- [Proposed audio, Wi-Fi and Bluetooth driver interfaces](../../drivers/README.md)
- [Overall Phase 2 driver architecture](../../docs/driver-architecture.md)

The Android device tree and Linux kernel are references for understanding the existing platform only. They are not XNU source, and this project does not copy or import their code.

## Open verification tasks

- Confirm exact model and hardware revision from non-sensitive device documentation.
- Confirm platform identifier; public sources disagree between `sm6150` and `SM7150`.
- Identify exact display panel, touch controller, storage, wireless, audio, and sensor components from reliable sources.
- Locate authoritative boot-chain documentation for the XT2135-2 variant.
- Record firmware/build family without publishing IMEI, serial number, or other unique identifiers.

## Current state

No device tree, driver, firmware, or boot configuration in this directory is validated or implemented.

## Research record

For each finding, record:
- Exact model and hardware revision
- Firmware/build identifier where relevant (avoid publishing unique device identifiers)
- Source URL and access date
- Whether the claim is verified, inferred, or unknown
- Any compatibility caveats

Do not place proprietary firmware or private device data in this directory.
