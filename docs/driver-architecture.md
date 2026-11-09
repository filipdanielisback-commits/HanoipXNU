# HanoipXNU Driver Architecture (Phase 2)

**Status:** Architecture proposal — not implemented, compiled, or tested on hardware.

This document defines a source-only architecture for investigating audio, Wi-Fi, and Bluetooth support on the Motorola Moto G60 (XT2135-2, `hanoip`). It does not claim that the exact hardware components or transports are known.

## Goals

- Define stable XNU-facing subsystem boundaries before implementation.
- Separate hardware-independent policy from device-specific transport and power control.
- Keep uncertain hardware facts isolated behind capability descriptions.
- Make unsupported operations fail explicitly rather than silently pretending to work.
- Require evidence and tests before changing the hardware-support status.

## Non-goals

- Reusing Android HALs, Linux kernel modules, or proprietary Android services as if they were XNU drivers.
- Claiming iOS, iOS 18, or Apple framework compatibility.
- Writing firmware loaders or touching device partitions in this phase.
- Guessing register maps, bus addresses, GPIOs, clocks, interrupts, or firmware formats.

## Proposed layering

```text
Applications / future userland
          |
Darwin-facing system services (future; not specified here)
          |
Generic subsystem interfaces
  +-------+----------+
  |                  |
Audio service     Network / Bluetooth services
  |                  |
XNU device-facing adapters and queues
  |                  |
Device-specific transport / codec / controller layer
  |                  |
Platform resources: interrupts, DMA, memory, clocks, power
  |                  |
Moto G60 hardware (exact parts and wiring still under audit)
```

The diagram is conceptual. It is not a claim that these services or adapters currently exist in this repository.

## Common driver contract

Each driver proposal should define:

1. **Identity:** supported hardware IDs/revisions and evidence source.
2. **Resources:** required memory regions, interrupts, DMA constraints, clocks, regulators, GPIOs, and shared-memory regions. Unknown values remain unknown.
3. **Lifecycle:** probe, initialize, start, stop, suspend, resume, detach.
4. **Concurrency:** execution context, locking, queue ownership, and callback rules.
5. **Data path:** buffer ownership, format, maximum sizes, back-pressure, and error propagation.
6. **Power:** explicit dependencies and safe failure behavior.
7. **Recovery:** timeout handling, reset strategy only when documented, and clean teardown.
8. **Observability:** structured logs and counters without exposing private device data.
9. **Tests:** host-side unit tests, mocked-resource tests, and later device tests with reproducible evidence.

No driver may assume that a resource exists merely because a related Android configuration references it.

## Subsystem boundaries

| Subsystem | Generic XNU-facing role | Hardware-specific boundary | Current confidence |
|---|---|---|---|
| Audio | PCM stream and device lifecycle abstraction | Codec/amp, DSP, serial audio links, clocks and routing | Android routing evidence only; physical parts unconfirmed |
| Wi-Fi | Network interface, packet queues, scan/state events | WLAN chipset, host bus, firmware, DMA and power | Exact chipset/host interface unresolved |
| Bluetooth | Controller transport plus host protocol integration | Controller, UART/USB/other transport, wake and power signals | UART signal names are provisional evidence only |

## Repository organization proposal

```text
drivers/
  README.md
  audio/
    README.md
    interface.md
  wifi/
    README.md
    interface.md
  bluetooth/
    README.md
    interface.md
```

These files are architecture specifications, not compilable driver implementations. Keep device-specific evidence in `devices/motorola/hanoip/`.

## Implementation gates

- **Gate A — evidence:** exact board revision, component identity, transport, and required resources are documented with reliable sources.
- **Gate B — interface:** API, lifecycle, data ownership, error model, and mocked tests are reviewed.
- **Gate C — implementation:** code is written against verified XNU APIs and reviewed for concurrency and memory safety.
- **Gate D — validation:** tests run in a controlled environment; hardware support is claimed only after repeatable target-device tests.

A subsystem stays **Investigating** until its evidence gate is met. A design document alone does not change the hardware-support matrix.

## Current decision

Proceed with interface specifications for audio, Wi-Fi, and Bluetooth. Do not implement hardware register access, firmware loading, or target-device changes until Gate A is satisfied.

## Related research

- [Hardware support matrix](hardware-support.md)
- [Audio hardware audit](../devices/motorola/hanoip/audio-hardware-audit.md)
- [Wireless interface audit](../devices/motorola/hanoip/wireless-interface-audit.md)
- [Wi-Fi and Bluetooth feasibility plan](../devices/motorola/hanoip/wifi-bluetooth-driver-plan.md)
