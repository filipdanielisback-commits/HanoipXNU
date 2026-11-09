# Driver Architecture

This directory contains proposed XNU-facing driver interface specifications for HanoipXNU.

**Nothing here is a working driver.** The documents define boundaries and requirements only. Hardware-specific facts must be verified for the exact Moto G60 XT2135-2 board revision before implementation.

## Subsystems

- [End-to-end audio architecture](../docs/audio-architecture.md)
- [Audio interface proposal](audio/interface.md)
- [Wi-Fi interface proposal](wifi/interface.md)
- [Bluetooth interface proposal](bluetooth/interface.md)
- [Overall architecture and implementation gates](../docs/driver-architecture.md)

## Rules

- Do not copy Android HALs or Linux modules and label them XNU drivers.
- Do not invent register maps, firmware formats, GPIOs, clocks, interrupts, or bus topology.
- Unknown hardware facts must remain explicitly unknown.
- Keep implementation source-only until evidence and interface reviews are complete.
- Update the hardware-support matrix only when repeatable tests justify it.
