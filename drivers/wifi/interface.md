# Wi-Fi Driver Interface Proposal

**Status:** Design only. No XNU Wi-Fi driver exists; chipset and host interface remain unresolved.

## Evidence boundary

The shared Qualcomm Linux device-tree source contains an `qcom,icnss` WLAN platform node with memory, interrupt, IOMMU, shared-memory, and power-resource references. Hanoip Android configuration lists `qca_cld3_wlan.ko`.

This is evidence of a Qualcomm WLAN software path in the Android/Linux ecosystem, not proof of the exact XT2135-2 board wiring, physical chipset identity, host transport, firmware compatibility, or XNU support. See the [wireless interface audit](../../devices/motorola/hanoip/wireless-interface-audit.md).

## Proposed layers

1. **Network interface adapter:** link state, MTU/capabilities, packet enqueue/dequeue, statistics.
2. **802.11 control plane:** scan, associate, authenticate, disconnect, regulatory state, power-save policy.
3. **Device protocol layer:** command/event transport and firmware interaction, defined only after chipset and bus are verified.
4. **Platform integration:** interrupts, DMA/IOMMU mappings, memory regions, power, clocks, reset and wake events.
5. **Firmware boundary:** explicit firmware identity, provenance, format, loading mechanism, and licensing review.

## Minimum interface contract

- Advertise only capabilities confirmed by the backend.
- Keep control-plane commands separate from packet data queues.
- Define packet ownership, queue limits, back-pressure, and completion semantics.
- Validate all lengths and firmware/device-originated event fields.
- Use bounded timeouts and explicit error states; never report association or link-up without a confirmed device event.
- Define suspend/resume and radio-state behavior only after hardware requirements are known.
- Do not expose a fake network interface that reports connectivity without a functioning backend.

## Unknowns that block implementation

- Exact WLAN chip/package on the target board revision.
- Physical host interface (PCIe, SDIO, or another transport).
- Exact board-specific enabled device-tree nodes and resource assignments.
- Firmware image identity, format, loading sequence, and redistribution rights.
- Required DMA/IOMMU constraints and power sequencing.
- Feasible XNU networking integration point and API version.

## Planned tests

Mocked tests should cover interface lifecycle, queue bounds, malformed events, timeout/error propagation, cancellation, and suspend/resume state transitions. Hardware tests come only after the transport and resource map are verified.

No Linux module or Android vendor blob should be treated as an XNU driver. Do not infer a host bus from the generic `icnss` node alone.
