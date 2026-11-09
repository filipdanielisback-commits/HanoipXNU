# Audio Driver Interface Proposal

**Status:** Design only. No XNU audio driver exists and no target-device audio test has been performed.

## Evidence boundary

The Android device configuration contains routing and mixer information referencing endpoints such as `SEC_MI2S_RX`, `RX_CDC_DMA_RX_0`, `TX_CDC_DMA_TX_3`, `SLIMBUS_7_RX`, and `SLIMBUS_7_TX`. It also references Qualcomm audio DSP, codec, and amplifier-related modules.

These are Android software configuration clues. They do **not** establish the exact physical codec/amplifier, board wiring, DSP protocol, or availability of an equivalent XNU interface. See the [audio hardware audit](../../devices/motorola/hanoip/audio-hardware-audit.md).

## Proposed layers

1. **Audio client/API layer:** stream open/close, supported-format query, start/stop, volume/mute controls where supported.
2. **PCM engine:** format conversion only where explicitly designed, bounded ring buffers, timestamps, underrun/overrun reporting.
3. **XNU device adapter:** lifecycle, synchronization, interrupt-to-work handoff, DMA buffer ownership.
4. **Codec/DSP backend:** device-specific routing and control, implemented only after exact components and protocols are verified.
5. **Platform resource provider:** clocks, power rails, reset and interrupt resources supplied through verified platform descriptions.

## Minimum interface contract

- Enumerate capabilities before opening a stream.
- Negotiate sample format, rate, channel count, and buffer size; reject unsupported combinations.
- Define whether buffers are driver-owned or client-owned and exactly when ownership changes.
- Report stream state and asynchronous errors explicitly.
- Handle start, stop, suspend, resume, and teardown without use-after-free or stale callbacks.
- Keep playback and capture capabilities independent; do not infer microphone support from speaker routing.
- Treat Bluetooth audio routing as a separate integration dependency, not proof of a working Bluetooth controller.

## Initial logical endpoints to investigate

| Endpoint | Android configuration clue | XNU status |
|---|---|---|
| Speaker playback | `SEC_MI2S_RX` | Unknown |
| Handset/headphone playback | `RX_CDC_DMA_RX_0` | Unknown |
| Microphone capture | `TX_CDC_DMA_TX_3` | Unknown |
| Bluetooth audio | `SLIMBUS_7_RX/TX` | Unknown |

## Required evidence before implementation

- Exact codec and amplifier part numbers for XT2135-2 board revision.
- Verified audio bus topology and controller assignment.
- DSP firmware dependency and legally usable firmware availability.
- Clock, regulator, reset, interrupt, and DMA requirements.
- Verified XNU audio framework/API target and supported kernel version.

## Planned tests

Before hardware tests: unit-test stream state transitions, capability negotiation, ring-buffer bounds, underrun/overrun reporting, cancellation, and teardown using mocked backends.

Later device tests must record exact model/revision, software build, test procedure, logs, and observed result. No audio support should be claimed from Android configuration alone.
