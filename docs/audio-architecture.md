# HanoipXNU Audio Architecture

**Status:** Proposed system architecture; design only.  
**Target:** Motorola Moto G60 XT2135-2 (`hanoip`).  
**Hardware status:** Exact codec, amplifier, audio bus topology, DSP dependencies, and board revision remain unverified.  
**Implementation status:** Portable PCM utilities exist; no XNU adapter or hardware backend exists.

## 1. Purpose and design rules

This document defines the intended architecture for a genuine audio subsystem in a Darwin/XNU-based research system. It is not a claim that Apple's audio stack can be transplanted wholesale, that iOS audio services are available, or that the Moto G60 audio hardware is supported.

The design follows these rules:

1. Keep client-facing audio policy separate from the kernel data path.
2. Keep PCM stream and buffer management independent of any particular codec or serial audio bus.
3. Isolate XNU-specific synchronization, interrupt, DMA, and device lifecycle code in an adapter layer.
4. Isolate SoC audio controllers, DSP transport, codec, amplifier, and board routing in hardware backends.
5. Discover and advertise only capabilities that the verified backend can actually provide.
6. Never infer physical wiring or hardware capabilities from Android mixer labels alone.
7. Treat playback and capture as separate capabilities with separate route and failure states.
8. Do not expose proprietary firmware or calibration data unless its availability and redistribution rights are established.

## 2. System view

```text
Applications / system clients
          |
Darwin user-space audio service (future)
  - device discovery and policy
  - stream sessions and routing
  - volume / mute policy where supported
          |
Stable audio client protocol (to be designed)
          |
XNU audio device adapter (future)
  - device lifecycle and synchronization
  - request validation and buffer ownership
  - interrupt / completion handoff
          |
Generic PCM engine
  - format and capability negotiation
  - playback and capture stream state
  - period scheduling and queue accounting
  - timestamps, xruns and error reporting
          |
Audio backend contract
  +---------------------+-----------------------+
  |                     |                       |
SoC audio controller   DSP transport        Codec / amp control
  |                     |                       |
DMA / serial link       firmware / services    analog routes / gains
  +---------------------+-----------------------+
                        |
Verified board resources
(clocks, regulators, reset, pin control, IRQs)
                        |
Moto G60 audio hardware (exact topology TBD)
```

This is a target architecture, not a description of existing code. The user-space service and XNU adapter are not implemented.

## 3. Control plane and data plane

### Control plane

The control plane configures devices and streams:

- enumerate devices and independently report playback/capture capabilities;
- negotiate sample format, sample rate, channel count, period size, and route;
- open, prepare, start, pause, resume, stop, and close streams;
- apply volume, mute, and route changes only where backend support is confirmed;
- report device availability, jack state, and asynchronous errors only when sensors/events exist.

A request must fail explicitly when the requested capability is unsupported. No silent format substitution or imaginary endpoint is allowed.

### Data plane

The data plane moves PCM frames:

- accept caller buffers under an explicit ownership contract;
- validate frame and period alignment;
- queue periods with bounded back-pressure;
- submit work to a backend;
- receive completion events;
- maintain monotonic frame counters/timestamps where a trustworthy clock source exists;
- detect and report underrun/overrun (xrun) conditions.

The data plane must not perform slow codec/DSP control transactions in interrupt context. Interrupt handlers should acknowledge the documented source and hand work to a suitable deferred context.

## 4. Layer responsibilities

| Layer | Owns | Must not own |
|---|---|---|
| Client API / user service | Sessions, permissions, policy, routing choices, user-visible controls | DMA descriptors or raw register access |
| XNU device adapter | Kernel lifecycle, synchronization, memory mapping rules, interrupt handoff, cancellation | Guesswork about codec identity or board nets |
| Generic PCM engine | Format validation, period accounting, queues, stream state, xrun accounting | SoC register maps, codec register sequences |
| SoC audio backend | Verified audio controller, serial links, DMA programming and completion | User policy or application sessions |
| DSP backend | Verified DSP transport, service protocol, firmware lifecycle | Invented firmware formats or undocumented commands |
| Codec/amplifier backend | Verified component control, routing, gain, mute, jack/amp state | Assumed component identity |
| Platform resource layer | Verified clocks, regulators, resets, pin states and interrupts | Audio policy |

Some physical components may be combined into one silicon package or controlled by a DSP. The architecture permits that; the implementation must follow the actual schematic and verified driver model.

## 5. Backend contract

A hardware backend should expose a narrow contract to the generic engine. Exact C signatures should be finalized only after the target XNU revision and execution-context rules are pinned.

Required operations:

- `probe()`: identify a supported device using verified IDs/resources; return unsupported if evidence does not match.
- `get_capabilities()`: return independently verified playback and capture capabilities.
- `prepare(direction, format, period)`: configure a validated stream while stopped.
- `start(direction)` / `stop(direction)`: start or quiesce the data path and report completion/failure.
- `pause()` / `resume()`: optional; advertise only if the hardware path can implement them safely.
- `submit(buffer)`: accept a buffer only when ownership and DMA constraints are satisfied.
- `cancel()`: stop accepting queued work and resolve every accepted buffer exactly once.
- `get_position()`: optional; return a monotonic frame position only if a reliable counter exists.
- `set_control()`: optional controls such as gain/mute, each capability-gated.
- `shutdown()`: quiesce interrupts and hardware, release resources, and prevent late callbacks.

Every accepted buffer must receive exactly one terminal outcome: completed, cancelled, or failed. A backend must not call completion twice, complete an unknown ticket, or access a buffer after ownership returns to the caller.

### Backend result model

Use explicit result categories: success, unsupported capability, invalid request, busy, timeout, device absent, hardware fault, cancelled, and resource failure. Preserve diagnostic context in logs/counters without exposing sensitive memory contents.

## 6. Stream and buffer lifecycle

The portable scaffold currently models stream states and buffer tickets. The target architecture adds hardware preparation and asynchronous execution around them:

```text
CLOSED
  | open + backend availability
  v
OPEN
  | configure format / route / periods
  v
PREPARED
  | start backend
  v
RUNNING <----> PAUSED (only if supported)
  |
  +---- stop / cancel ----> DRAINING
                               |
                      all accepted buffers resolved
                               |
                               v
                             OPEN
                               |
                            close
                               v
                            CLOSED

Any active state --fatal backend error--> FAILED
FAILED --quiesce + resolve buffers--> OPEN or CLOSED
```

The existing portable state machine uses a simpler set of states. Do not silently reinterpret its current ABI as this richer lifecycle. A future revision should add PREPARED and DRAINING only alongside explicit tests and migration.

Shutdown ordering is mandatory:

1. Reject new submissions.
2. Stop or quiesce the hardware using documented semantics.
3. Disable or mask the relevant interrupt source.
4. Synchronize with deferred work/callbacks.
5. Resolve every accepted buffer exactly once.
6. Release DMA mappings and memory only after hardware can no longer access them.
7. Release clocks, regulators, and device resources in reverse dependency order.

The exact ordering of interrupt masking, device stop, and synchronization must be validated against the actual controller and XNU execution model.

## 7. DMA, memory, and concurrency requirements

Before any DMA backend is implemented, document:

- address width and DMA mask;
- alignment and boundary restrictions;
- whether the device supports scatter/gather;
- cache coherency and required clean/invalidate operations;
- mapping/unmapping ownership and lifetime;
- descriptor format and ring semantics from authoritative controller documentation;
- whether buffers must be physically contiguous;
- maximum transfer/period size;
- interrupt and completion ordering.

Do not assume that a CPU pointer is a DMA address. Do not assume coherent DMA. Do not free or reuse a buffer until hardware ownership has ended.

Concurrency rules:

- Define which methods may run at thread context, interrupt context, or deferred-work context.
- Use XNU-supported synchronization primitives only after pinning the XNU source/API revision.
- Never call user-space clients from interrupt context.
- Protect stream state and queue state consistently.
- Make cancellation and teardown idempotent where practical.
- Reject stale completions using generation-tagged identities.

The current portable C utilities require caller serialization and are not a substitute for kernel locking or DMA-safe memory management.

## 8. Playback and capture paths

### Playback

Client PCM -> format validation -> period queue -> backend DMA/transport -> SoC serial audio path -> codec DAC/output route -> amplifier (if present) -> speaker, earpiece, or headset.

Only the external endpoints are confirmed at product level. The codec, bus, amplifier, and exact routing are not yet confirmed for the target board revision.

### Capture

Microphone endpoint -> verified analog/digital front end -> codec ADC or digital microphone path -> SoC serial audio path / DSP -> backend capture buffers -> PCM engine -> client.

The existence of microphone endpoints does not establish that the controller, codec, DSP path, or capture route is understood. Capture support must be enabled independently after its path is verified.

Bluetooth audio is a separate system integration: controller transport, Bluetooth host stack, profile support, and audio routing all need their own evidence. Android A2DP/SCO mixer entries do not establish a working HanoipXNU Bluetooth audio path.

## 9. Hanoip evidence map

| Item | Current evidence | Architecture consequence |
|---|---|---|
| Speaker, earpiece, headset jack, microphone endpoints | Motorola product guide | Logical endpoints can be listed as product features; pin routing remains unknown |
| Android routing identifiers such as `SEC_MI2S_RX`, `RX_CDC_DMA_RX_0`, `TX_CDC_DMA_TX_3` | Android audio platform configuration | Leads for source audit only; not physical bus proof |
| `SLIMBUS_7_RX/TX` | Android Bluetooth audio routing config | Do not assume a physical Slimbus link until corroborated |
| WCD934x/WCD937x/WCD938x and WSA881x module references | Android build/init configuration | Candidate driver-family leads only; no part selection |
| Exact codec and amplifier | Not verified against exact XT2135-2 board revision | Hardware backend blocked |
| Audio controller node, bus topology, DMA and IRQ | Not fully correlated to exact board schematic | Controller backend blocked |
| DSP firmware/calibration | Android vendor references; availability/licensing unresolved | DSP integration blocked |
| XNU source revision and audio framework target | Not pinned | XNU adapter/framework mapping blocked |

See [the audio hardware audit](../devices/motorola/hanoip/audio-hardware-audit.md) for source details and confidence.

## 10. Implementation gates and sequence

### Gate 1 — platform evidence

Exit only when the exact board revision and schematic are identified and the following are cross-referenced: codec, amplifier, controller, buses, clocks, reset, regulators, interrupts, DMA, microphones, speakers, and headset routing.

### Gate 2 — XNU target selection

Pin the XNU source revision and determine the actual driver integration model available in that revision. Document the chosen kernel interfaces and execution contexts. Do not assume a current Apple driver framework is available on an arbitrary XNU port.

### Gate 3 — generic engine contract

Review capability negotiation, PREPARED/DRAINING lifecycle, playback/capture independence, timestamps, xrun reporting, cancellation, and buffer ownership. Add mocked-backend tests for all transitions and failure paths.

### Gate 4 — controller and codec backends

Implement one verified component at a time against authoritative documentation. Begin with probe/resource validation and a safe stopped state; add DMA and audio routing only after resource requirements are established.

### Gate 5 — user-space service

Define the client protocol, device enumeration, stream sessions, routing policy, and control permissions. Keep proprietary iOS services and frameworks out of scope unless independently available and legally usable.

### Gate 6 — validation

Run host tests, mocked backend tests, kernel build/static checks, then controlled target tests. Record exact board revision, software revision, route, PCM format, logs, and observed outcome. Claim support only for paths that pass repeatable tests.

## 11. Immediate next tasks

1. Obtain or inspect the exact XT2135-2 board schematic pages and establish the board revision.
2. Reconcile Android audio module aliases, init module list, and actual kernel module build outputs.
3. Correlate candidate audio controller nodes with the exact board DTS and schematic nets.
4. Pin the XNU source revision and investigate its available audio driver integration points.
5. Expand host tests with mocked backend, capability negotiation, cancellation, xrun accounting, and teardown races before any hardware code.

**Current conclusion:** HanoipXNU now has a portable PCM foundation and a proposed end-to-end audio architecture. It does not yet have an XNU audio adapter, SoC audio controller driver, DSP backend, codec driver, or working audio on the Moto G60.
