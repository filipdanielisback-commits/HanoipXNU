# Bluetooth Driver Interface Proposal

**Status:** Design only. No controller transport or XNU Bluetooth integration has been verified on the target.

## Evidence boundary

A third-party transcription of a Huaqin schematic contains WCN-related UART signal labels, which may indicate a UART path for Bluetooth. The schematic transcription and component label are provisional and have not been confirmed against the exact XT2135-2 board revision. Qualcomm family-level references and the provisional WCN3980 label are not sufficient to identify the target controller.

See the [wireless interface audit](../../devices/motorola/hanoip/wireless-interface-audit.md).

## Proposed layers

1. **Host protocol integration:** HCI command/event and data-channel abstraction.
2. **Controller transport:** pluggable transport backend (UART, USB, or another verified bus). Do not select one until evidence confirms it.
3. **Power/wake integration:** controller enable, host wake, device wake, clocks and reset, only as verified.
4. **Bluetooth services:** higher-level profiles and audio routing are separate integration layers and are not part of a basic controller transport driver.

## Minimum interface contract

- Expose transport capabilities and maximum frame sizes.
- Define framing, flow control, buffer ownership, and completion behavior.
- Separate command, event, and data paths.
- Validate packet type and length before dispatch.
- Handle partial reads/writes, transport errors, timeouts, and controller reset according to verified protocol documentation.
- Define wake/suspend/resume behavior only after board signals and timing requirements are confirmed.
- Do not report a controller as available until initialization and a valid response are observed.

## Unknowns that block implementation

- Exact Bluetooth controller part number and board revision.
- Confirmed transport and pin assignment.
- UART/USB controller instance, baud/flow-control requirements, or other bus parameters.
- Power, reset, host-wake/device-wake, and clock dependencies.
- Firmware/NVM requirements and redistribution constraints.
- XNU Bluetooth host-stack integration point.

## Planned tests

Mocked tests should cover frame parsing, partial I/O, malformed frames, queue bounds, timeout handling, cancellation, and lifecycle transitions. Later controller tests require verified hardware identity and a documented transport configuration.

Do not assume that Bluetooth audio routing references in Android configuration prove that the controller or transport is operational under XNU.
