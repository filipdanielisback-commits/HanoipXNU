# Platform adaptation

Platform-specific code is separated from generic XNU integration and from device-specific facts.

Planned layers:
- `arm64/`: architecture-level assumptions and glue;
- `qualcomm/`: SoC-family research and reusable platform components;
- `devices/motorola/hanoip/`: exact board/device configuration and evidence.

Do not infer register addresses, interrupt routing, memory maps, clocks, power rails, or bus topology from a marketing SoC name. Every hardware-specific implementation needs a source citation and a confidence label.
