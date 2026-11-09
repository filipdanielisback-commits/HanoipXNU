# HanoipXNU Driver Architecture (Phase 2)

**Status:** Audio and Wi-Fi architecture proposals documented; portable audio data-path utilities exist and have limited host-side test evidence. No XNU driver adapter or Moto G60 hardware driver has been implemented.

This document defines the architecture for investigating audio, Wi-Fi, and Bluetooth support on the Motorola Moto G60 (XT2135-2, `hanoip`). Exact hardware components and some transports remain unknown.

## Goals

- Define stable XNU-facing subsystem boundaries before hardware-specific implementation.
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

The diagram is conceptual. It is not a claim that these services or adapters currently exist in the repository.

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
| Audio | PCM stream and device lifecycle abstraction | Codec/amp, DSP, serial audio links, clocks and routing | Portable byte-ring core exists; physical parts and XNU adapter unconfirmed |
| Wi-Fi | Network interface, control plane, bounded packet queues and link state | WLAN chipset, verified host transport, firmware, DMA and power | Architecture documented; exact chipset/host interface unresolved |
| Bluetooth | Controller transport plus host protocol integration | Controller, UART/USB/other transport, wake and power signals | UART signal names are provisional evidence only |

## Implementation status

- **Audio:** `drivers/audio/pcm_ring.c` implements a portable bounded byte ring using caller-owned storage. It does not allocate memory, interpret PCM formats, synchronize concurrent access, access hardware, or call XNU APIs. Host tests are in `tests/audio/`.
- **Wi-Fi:** generic architecture documented in [docs/wifi-architecture.md](wifi-architecture.md); interface proposal in [drivers/wifi/interface.md](../drivers/wifi/interface.md). No generic Wi-Fi code, mock tests, XNU adapter, firmware loader, transport backend, or hardware driver exists.
- **Bluetooth:** interface design only; no code or hardware backend.
- **XNU integration:** not started. The repository has not yet pinned a specific XNU source revision, target SDK/build environment, or driver integration model.

## Implementation gates

- **Gate A — evidence:** exact board revision, component identity, transport, and required resources are documented with reliable sources.
- **Gate B — interface:** API, lifecycle, data ownership, error model, and mocked tests are reviewed.
- **Gate C — implementation:** code is written against a pinned, verified XNU source/API target and reviewed for concurrency and memory safety.
- **Gate D — validation:** tests run in a controlled environment; hardware support is claimed only after repeatable target-device tests.

A subsystem stays **Investigating** until its evidence gate is met. A portable utility or successful host build does not establish XNU or hardware support.

## Related research

- [Hardware support matrix](hardware-support.md)
- [Audio implementation notes](../drivers/audio/README.md)
- [Audio hardware audit](../devices/motorola/hanoip/audio-hardware-audit.md)
- [Wi-Fi architecture](wifi-architecture.md)
- [Wireless interface audit](../devices/motorola/hanoip/wireless-interface-audit.md)
- [Wi-Fi and Bluetooth feasibility plan](../devices/motorola/hanoip/wifi-bluetooth-driver-plan.md)
