# Project Requirements and Known Unknowns

This document records the research target. It is not a validated compatibility statement.

## Target
- Motorola Moto G60
- Model: XT2135-2 (target supplied for this project)
- Codename family: `hanoip`
- RAM configuration: 6 GB target

## Preliminary public-source findings

The public TeamWin device configuration for the Moto G40/G60 describes the family as `hanoip` and lists a Qualcomm Snapdragon 732G, Kryo 470 CPU cores, Adreno 618 GPU, 4/6 GB memory configurations, and a 1080 × 2400 display. These are family-level reference specifications, not a verified inventory of this exact XT2135-2 unit.

**Platform identifier needs verification:** public device-tree material is inconsistent. One source labels the chipset/platform as `sm6150`, while another community device-tree description labels it `SM7150`. Do not hard-code either identifier until checked against the exact device's trustworthy build configuration and hardware documentation.

Useful public references (reviewed 2026-11-09):
- [TeamWin Moto G40/G60 hanoip device configuration](https://github.com/TeamWin/android_device_motorola_hanoip)
- [motorola-sm6150-devs hanoip Android device tree](https://github.com/motorola-sm6150-devs/device_motorola_hanoip)
- [Community Moto G60/G40 kernel source tree](https://github.com/Dark-Matter7232/CosmicFresh-Hanoip) — archived in 2023 and is an Android/Linux kernel tree, not XNU.

These references are research leads. Their existence does not establish that their code is current, correct for every XT2135-2 variant, or reusable under the same license.

## Host development environment
The initial host environment may use Windows with WSL2. Exact package versions and commands will be documented only after a reproducible build path is selected.

Potential research tools include Git, a compiler toolchain, source-control utilities, and an emulator. Tool selection depends on the actual target architecture and upstream project requirements.

## Required evidence before device testing
- Exact device revision and firmware baseline
- Verified boot-chain documentation
- A reliable stock recovery path
- Compatibility analysis for the target kernel architecture
- Review of partition and boot-image formats from authoritative sources
- Independent review of any proposed device-side test

## Important constraints
A successful XNU build for another platform does not establish compatibility with the Moto G60. Kernel architecture, boot protocol, memory map, interrupt controller, device tree, drivers, and platform services all matter.

Do not unlock, repartition, or flash the phone as part of this documentation phase.
