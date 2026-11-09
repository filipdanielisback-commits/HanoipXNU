# Kernel and platform configurations

This directory will contain HanoipXNU-owned build configuration fragments, not copied HakiOS configs.

Planned separation:
- common XNU build settings;
- ARM64 architecture settings;
- Qualcomm SoC settings, only after platform identification is verified;
- Moto G60 hanoip device settings;
- host/emulator settings for source-level tests.

A config must state its target, upstream XNU revision, toolchain/SDK assumptions, and validation status. No bootable target configuration is currently available.
