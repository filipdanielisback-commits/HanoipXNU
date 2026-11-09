# HanoipXNU Wi-Fi Architecture

**Status:** Architecture proposal plus portable lifecycle state machine and fake-backend host tests. No XNU Wi-Fi driver, firmware loader, network interface, or hardware backend is implemented.
**Target:** Motorola Moto G60 XT2135-2 (`hanoip`)
**Review date:** 2026-11-09

## 1. Purpose and scope

This document defines a hardware-independent architecture for investigating Wi-Fi support in HanoipXNU. It separates Darwin/XNU-facing networking from 802.11 policy, chipset protocol, transport, and platform resources.

The architecture is intentionally backend-neutral. It does not select PCIe, SDIO, or another host interface; identify a final WLAN chipset; prescribe register access; or assume that Linux driver code can be reused. Those decisions require exact board evidence and a pinned XNU source/API target.

This is an XNU/Darwin research design, not an iOS implementation claim. No device-side experiment or flashing is part of this milestone.

## 2. Evidence and confidence boundary

### 2.1 Evidence currently available

- The inspected Hanoip Android device-tree build lists `qca_cld3_wlan.ko` and maps the Android module alias `wlan.ko` to it. This establishes a Qualcomm WLAN Linux-module dependency in that build, not the exact silicon or bus.
- The shared SM6150 Linux device-tree source contains a `qcom,icnss` WLAN platform node with memory regions, CE0–CE11 interrupt entries, IOMMU/SMMU references, WLAN MSA memory, supply dependencies, and SMP2P signaling.
- Qualcomm's ICNSS binding describes an integrated WLAN path in which WLAN firmware runs on the Q6 subsystem and the host platform driver communicates with WLAN firmware over QMI. That binding explains the meaning of this particular ICNSS architecture; it does not prove every detail of the XT2135-2 production board's final enabled DTB or establish a discrete radio's application-processor bus.
- A third-party schematic transcription has a provisional WCN3980 label. The label conflicts with the Snapdragon 732G / FastConnect 6200 family-level references and is not authoritative enough to lock a chip-specific design.

### 2.2 Unknowns that remain blockers

| Item | Status | Consequence |
|---|---|---|
| Exact WLAN silicon and board revision | Unresolved; WCN3980 is provisional | No chip-specific protocol or firmware assumptions |
| Exact board-final WLAN topology | Not yet established from matching board DTS/DTB | Shared SoC nodes are clues, not proof of enabled wiring |
| Host/control transport | Not confirmed for the target | No PCIe/SDIO/QMI/other transport selection in code |
| Firmware and calibration artifacts | Identity, format, loading flow, provenance and redistribution rights unknown | No firmware loader or bundled firmware |
| DMA/IOMMU contract | Generic SM6150 resources seen; target-specific ownership and mappings not verified | No hardware DMA implementation |
| Power/reset/clock/wake sequence | Not fully verified for exact board revision | No power sequencing implementation |
| XNU integration target | XNU revision and networking integration point not pinned | No framework-specific adapter |

**Rule:** never convert a family-level clue, generic SoC DTSI property, Android module name, or provisional schematic transcription into a confirmed board fact.

## 3. Architectural overview

```text
Future network configuration service / userland
                    |
             Darwin-facing adapter
                    |
        Generic Wi-Fi interface coordinator
             /                  \
   802.11 control plane      Packet data plane
   scan / join / state      TX queues / RX delivery
             \                  /
              Chipset backend contract
                       |
       Verified protocol + transport backend
                       |
        Platform resource and DMA services
                       |
       Verified Hanoip WLAN hardware (TBD)
```

The diagram shows intended boundaries only. These components do not currently exist as a working stack.

### Layer A — Darwin-facing network adapter

Responsibilities:
- Represent a network interface only after a real backend has initialized successfully.
- Translate interface lifecycle, link state, packet delivery, and statistics to the networking integration selected for the pinned XNU revision.
- Advertise only capabilities the backend has positively reported and the integration layer can support.
- Propagate failures and link transitions; never synthesize an associated or connected state.

The precise XNU/IOKit/networking APIs must be selected after pinning the XNU source revision and reviewing its actual interfaces. Do not invent a modern Apple private API or assume an archived API remains suitable.

### Layer B — Generic Wi-Fi coordinator

Responsibilities:
- Own interface lifecycle and coordinate control and data paths.
- Track radio, scan, authentication/association, link, and power states.
- Validate requests and backend events.
- Enforce bounded queues, cancellation, timeouts, and explicit error propagation.
- Publish capabilities only after backend negotiation.

This layer must not contain chip registers, bus-specific descriptors, firmware offsets, or board GPIO values.

### Layer C — 802.11 control plane

The control plane represents operations and state transitions, not a complete supplicant implementation. Its abstract operations include:
- enumerate supported capabilities;
- request/cancel scan and deliver bounded scan results;
- request connection using opaque credential references supplied by an authorized higher-level service;
- report authentication/association progress and final outcome only from backend-confirmed events;
- request disconnect;
- report radio availability, link state, and reason-coded failures;
- apply regulatory constraints from a trusted policy source.

Credential handling belongs in a higher-level security-conscious service. The kernel-facing design should not log passwords, passphrases, or other secrets. Do not invent regulatory domains or transmit-power rules.

### Layer D — Chipset backend contract

This is the boundary between generic Wi-Fi logic and the eventual hardware-specific implementation.

The backend must declare:
- supported hardware identity/revision and evidence;
- transport and protocol version;
- firmware requirements and exact artifact identity;
- supported bands, modes, channel widths, and optional features;
- maximum frame sizes and queue limits;
- DMA constraints and interrupt/event model;
- power-management capabilities;
- recovery and reset capabilities that are actually documented.

The generic coordinator must tolerate a backend that supports only a subset of the eventual feature set.

### Layer E — Transport and platform resources

Transport is a replaceable implementation boundary. Candidate transport names may be used in research notes, but none is selected for Hanoip until the board-final topology is verified.

Platform services must own resource acquisition and release for verified memory regions, interrupts, DMA mappings, clocks, regulators, reset, and wake signaling. The backend receives scoped resource handles; it must not independently guess physical addresses or manipulate undocumented resources.

## 4. Control-plane lifecycle

The proposed abstract state machine is:

```text
ABSENT -> DISCOVERED -> INITIALIZING -> READY
                                  READY -> SCANNING -> READY
                                  READY -> CONNECTING -> LINK_UP
                                  LINK_UP -> DISCONNECTING -> READY
                                  READY/LINK_UP -> SUSPENDING -> SUSPENDED
                                  SUSPENDED -> RESUMING -> READY or LINK_UP
Any active state -> FAILED -> STOPPING -> ABSENT
```

Rules:
1. `DISCOVERED` means a matching device was enumerated by a verified transport; it does not mean firmware is running.
2. `READY` requires successful initialization and a valid backend-ready indication.
3. `LINK_UP` requires a confirmed association result and successful network-interface configuration at the appropriate layers. A scan result is not a connection.
4. Every asynchronous request has an identifier, bounded lifetime, cancellation behavior, and exactly defined completion semantics.
5. Stale events from a prior initialization generation must be discarded.
6. A fatal backend/transport failure transitions to `FAILED`; the interface must not continue to report healthy connectivity.
7. Suspend/resume transitions are enabled only when the backend declares and passes tests for the required behavior.

## 5. Packet data path

### Transmit

1. The network adapter submits a packet using the pinned XNU integration contract.
2. The generic coordinator validates packet length and interface state.
3. A bounded TX queue accepts ownership or rejects the packet with explicit back-pressure.
4. The backend maps or copies data according to a verified DMA/transport contract.
5. A completion returns ownership and a status exactly once.
6. Queue accounting and interface statistics are updated.

### Receive

1. The backend receives and validates transport-level metadata.
2. It checks all lengths, offsets, and buffer bounds before exposing packet data.
3. The coordinator applies queue limits and transfers packet ownership to the network adapter.
4. The adapter delivers the packet through the selected XNU networking path.
5. Every buffer is released exactly once, including error, cancellation, and teardown paths.

No zero-copy behavior is assumed. Start with the simplest safe ownership model compatible with the selected XNU APIs; optimize only after correctness and DMA constraints are proven.

## 6. Firmware and calibration boundary

Firmware is a separately audited dependency, not an arbitrary file copied from an Android image.

Before implementing loading, document:
- exact chip/revision compatibility;
- filename, format, version, and cryptographic hash;
- source and provenance;
- required authentication/signature mechanism, if any;
- calibration and board-data dependencies;
- load order and transport path;
- license and redistribution permissions;
- behavior for missing, mismatched, or rejected firmware.

Do not bypass firmware authentication or device security checks. Do not include proprietary firmware in the repository without a documented legal basis. The first architecture milestone defines only this boundary; it does not implement a loader.

## 7. Concurrency, memory safety, and teardown

- Define one owner for each queue and each buffer at every lifecycle stage.
- Bound command, event, scan-result, TX, and RX queues.
- Validate all backend-originated lengths, identifiers, enum values, and state transitions.
- Keep interrupt context minimal; defer operations that may block or allocate according to the pinned XNU execution model.
- Never wait for firmware or transport completion while holding a lock needed by the completion path.
- Use generation numbers to reject stale callbacks after stop/restart.
- Teardown must prevent new submissions, cancel or drain outstanding work, quiesce interrupts, release DMA mappings and buffers, then release platform resources in reverse acquisition order.
- A timeout is an error, not proof that hardware stopped. Resource release must follow a verified quiescence strategy.

The exact locking primitives, work-loop model, interrupt API, and DMA APIs remain undecided until the XNU revision is pinned.

## 8. Error model and observability

The generic API should distinguish at least:
- unsupported capability;
- invalid request or malformed event;
- device absent or identity mismatch;
- firmware missing/rejected/initialization failure;
- transport unavailable or timed out;
- queue full/back-pressure;
- scan or connection failure;
- radio unavailable;
- suspend/resume failure;
- cancellation;
- fatal backend failure.

Logs should use stable event codes and redact credentials, packet payloads, and device-unique secrets. Counters may include queue drops, TX/RX completions, malformed events, timeouts, state transitions, and recovery attempts.

## 9. Security and privacy boundaries

- Treat firmware, scan results, device events, and transport data as untrusted input until validated.
- Keep credentials out of kernel logs and persistent debug output.
- Do not expose raw firmware or calibration data through ordinary diagnostic interfaces.
- Enforce regulatory constraints using a trusted policy source; do not let an untrusted client widen permitted operation.
- Make radio availability and user-requested disable state explicit.
- Avoid collecting or retaining nearby network identifiers beyond what is necessary for the requested operation.

## 10. Mock-first test plan

Tests can be developed without target hardware by using a deterministic fake backend. These tests validate generic logic only; they do not establish XNU or radio compatibility.

| Test group | Required cases |
|---|---|
| Lifecycle | probe/initialize success and failure, repeated start/stop, detach while idle |
| Control plane | scan request/cancel, out-of-order events, malformed event, connection failure, disconnect |
| State machine | illegal transitions, stale generation, duplicate completion, timeout |
| TX/RX queues | bounds, back-pressure, ownership, malformed lengths, completion exactly once |
| Firmware boundary | absent/mismatched/rejected artifact represented as errors; no bypass path |
| Power | supported/unsupported suspend, resume failure, cancellation during transition |
| Teardown | outstanding TX/RX, pending commands, late callbacks, resource-release ordering |
| Security | credential redaction, untrusted event validation, invalid capability claims |

A host-side mock test does not prove that the XNU adapter compiles or that the target hardware works. XNU-facing tests require a pinned source revision and reproducible build environment.

## 11. Implementation gates

| Gate | Work | Exit criteria |
|---|---|---|
| A — board evidence | Identify exact board revision, final WLAN node, chip, host interface, resources | Device-specific evidence is consistent and independently reviewable |
| B — firmware and protocol | Identify firmware/calibration and documented initialization protocol | Provenance, compatibility, legal status and protocol requirements recorded |
| C — XNU target | Pin XNU source revision and select supported network/driver integration point | APIs and execution model reviewed against source |
| D — generic core | Implement state machine, queues, backend contract and mock tests | Tests pass with deterministic fake backend |
| E — transport backend | Implement only the verified transport and resource integration | Resource ownership, DMA, interrupts and teardown reviewed |
| F — device validation | Validate in a controlled, recoverable test environment | Reproducible logs demonstrate enumeration, scan, association and traffic |

No gate is considered passed by documentation alone. No phone flashing or device-side experiment is part of the current milestone.

## 12. Portable core implemented

The first hardware-independent slice is implemented in `drivers/wifi/state.h` and `drivers/wifi/state.c`. It models discovery, initialization, ready, scan, connection, link, disconnect, suspend/resume, failure, stop, and detach. Initialization, fatal-error, scan/connect/disconnect, and suspend/resume completions require the current device-generation token; scan/connect/disconnect also require a matching request ID. This rejects stale asynchronous completions from a previous attachment.

The caller must serialize access and call completion functions only after backend confirmation. The module performs no I/O, allocation, locking, firmware loading, packet transfer, or XNU calls. Deterministic tests live in `tests/wifi/` and run with `make -C tests/wifi test`; they test the portable logic only, not a real backend or hardware.

## 13. Current status

| Component | Status |
|---|---|
| Android Qualcomm WLAN module clue | Observed in inspected device-tree build |
| Shared SM6150 ICNSS resources | Observed in shared Linux SoC source |
| Exact target chip | Unresolved |
| Exact target host transport | Unresolved |
| Firmware/calibration contract | Unresolved |
| Generic Wi-Fi architecture | **Documented in this proposal** |
| Generic Wi-Fi lifecycle state machine | Implemented; host tests added |\n| Packet queues and mock data-path tests | Not implemented |
| XNU network adapter | Not implemented |
| Hanoip transport/backend | Not implemented |
| Working Wi-Fi on HanoipXNU | **Not implemented or tested** |

## References

- [Hanoip wireless interface audit](../devices/motorola/hanoip/wireless-interface-audit.md)
- [Hanoip Wi-Fi/Bluetooth feasibility plan](../devices/motorola/hanoip/wifi-bluetooth-driver-plan.md)
- [Wi-Fi interface proposal](../drivers/wifi/interface.md)
- [Overall driver architecture](driver-architecture.md)
- [Qualcomm ICNSS device-tree binding](https://android.googlesource.com/kernel/msm/+/c568eb7aca15625b4afdd922350b9b352ea78c2f/Documentation/devicetree/bindings/cnss/icnss.txt)
- [Shared SM6150 device-tree source](https://github.com/LineageOS/android_kernel_motorola_sm6150/blob/lineage-20/arch/arm64/boot/dts/qcom/sm6150.dtsi)
- [Hanoip Android device tree](https://github.com/motorola-sm6150-devs/device_motorola_hanoip)
- [Apple XNU source](https://github.com/apple-oss-distributions/xnu)

These references support research and architectural boundaries only. They do not demonstrate a working XNU driver or target-device support.
