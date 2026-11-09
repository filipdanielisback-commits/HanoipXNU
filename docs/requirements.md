# Project Requirements and Known Unknowns

This document records the research target. It is not a validated compatibility statement.

## Target
- Motorola Moto G60
- Model: XT2135-2 (target supplied for this project)
- Codename: hanoip (to be verified against authoritative device sources)
- RAM configuration: 6 GB target

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
