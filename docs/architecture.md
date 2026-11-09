# Architecture Overview

## Intended layers

The project is investigating whether a Darwin/XNU-based system can be adapted to the Moto G60. The conceptual layers are:

1. **Device boot firmware and boot chain** — vendor- and device-specific startup stages.
2. **Boot integration** — a compatible handoff from the device's boot environment to a kernel. The feasibility and exact format are unknown.
3. **Kernel** — XNU and the platform-specific code required to initialize the hardware.
4. **Drivers and platform services** — display, input, storage, networking, audio, power, and other device functions.
5. **Userland** — system services, runtime libraries, and applications compatible with the selected Darwin environment.

## Current status
This is a conceptual model only. No layer is currently claimed to work on the target device.

## Critical distinction
XNU is a kernel, not the complete iOS operating system. A functioning iOS environment also relies on proprietary components and platform integration that are not provided by XNU's public source code.

## Open questions
- What exact processor architecture and boot protocol are exposed by the target firmware?
- Is there a viable kernel entry path compatible with XNU?
- Which required kernel subsystems depend on Apple-specific hardware or interfaces?
- What portions of the device hardware can be documented from public sources?
- Is a useful emulator-based proof of concept possible before any phone testing?
