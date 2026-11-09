# Wireless Hardware Interface Audit

**Status:** Source-level audit; no hardware validation  
**Target:** Motorola Moto G60 XT2135-2 (`hanoip`)  
**Review date:** 2026-11-09

## Summary

This audit traces the wireless-related evidence in the available Hanoip Android device trees and the shared Qualcomm SM6150 Linux device-tree source.

The strongest new finding is that the shared SM6150 device-tree source describes a Qualcomm WLAN platform integration node (`qcom,icnss`) with memory-mapped resources, copy-engine interrupts, IOMMU/SMMU configuration, WLAN shared memory, and power-supply dependencies. This is useful architectural evidence for the SoC family.

**Important limitation:** the inspected `sm6150.dtsi` is a shared SoC-level include, not a verified XT2135-2 board-specific device tree. Its nodes describe capabilities/configuration available to a platform; they do not prove that every node is enabled or wired in the exact Hanoip board DTB. They also do not identify the exact combo-chip package.

No XNU driver has been implemented or tested.

## 1. Evidence chain

### 1.1 Hanoip Android device tree

The `motorola-sm6150-devs/device_motorola_hanoip` tree:

- Lists `qca_cld3_wlan.ko` in `modules.load`.
- Maps the Android module alias `wlan.ko` to `qca_cld3_wlan.ko` in `BoardConfig.mk`.

This confirms that the referenced Android build expects a Qualcomm CLD3 WLAN Linux kernel module. It does not establish the exact wireless silicon, the physical host bus, or XNU compatibility.

Source: https://github.com/motorola-sm6150-devs/device_motorola_hanoip

### 1.2 Shared Qualcomm SM6150 device-tree source

The `LineageOS/android_kernel_motorola_sm6150` repository's `lineage-20` branch contains the shared file:

`arch/arm64/boot/dts/qcom/sm6150.dtsi`

The inspected source includes a WLAN node compatible with `qcom,icnss`. Its properties include:

- A memory-mapped WLAN/ICNSS resource region.
- Copy-engine interrupt entries labelled CE0 through CE11.
- An IOMMU reference.
- WLAN MSA shared-memory configuration.
- Power-supply references for CX/MX, 1.8 V, 1.3 V RF, and 3.3 V rails.
- SMP2P interrupt/state references for WLAN crash/error signaling.

This points to a Qualcomm WLAN integration model involving platform resources, DMA/IOMMU, shared memory, interrupts, and power management. These are dependencies a future low-level port would need to understand.

**It does not prove the exact transport used by the discrete Hanoip wireless device.** In particular, the generic node must not be treated as proof that the XT2135-2 uses a particular PCIe, SDIO, or other host connection. The board-specific device tree and actual boot DTB must be examined before making that claim.

Source: https://github.com/LineageOS/android_kernel_motorola_sm6150/blob/lineage-20/arch/arm64/boot/dts/qcom/sm6150.dtsi

### 1.3 Bluetooth configuration

The alternate Hanoip Android device tree sets:

`TARGET_USE_QTI_BT_STACK := true`

and includes Android Bluetooth configuration files. This identifies the Android Bluetooth software stack choice, not the exact controller transport or a reusable XNU implementation.

Source: https://github.com/bhawesh2002/android_device_motorola_hanoip

The third-party-hosted Project Hanoi schematic transcription previously reviewed labels UART signals associated with the wireless block. That remains provisional board-level evidence until matched to an authoritative schematic and the exact board revision.

## 2. Findings and confidence

| Question | Finding | Confidence |
|---|---|---|
| Does the Android build load a Qualcomm WLAN module? | `qca_cld3_wlan.ko` is listed and mapped as `wlan.ko` | High for the inspected build tree |
| Does the shared SM6150 source describe Qualcomm WLAN platform resources? | Yes: ICNSS, CE interrupts, IOMMU, MSA memory, supplies, SMP2P | High for the inspected shared SoC source |
| Is that shared DTSI the exact XT2135-2 board configuration? | Not established | Unknown |
| Is the exact Wi-Fi host bus confirmed for XT2135-2? | No | Unknown |
| Is the exact combo-chip package confirmed? | No; WCN3980 remains provisional from the schematic transcription | Provisional |
| Does the Android device tree use QTI Bluetooth stack configuration? | Yes, in the inspected alternate tree | High for that tree |
| Is Bluetooth UART wiring fully confirmed for this board revision? | No; schematic signal labels are provisional | Provisional |
| Are power/reset/clock/firmware sequences fully documented? | No | Unknown |
| Does HanoipXNU contain working Wi-Fi or Bluetooth support? | No | Not implemented |

## 3. What this means for a future XNU design

### Wi-Fi research requirements

Before writing a driver, obtain and correlate:

1. The exact XT2135-2 board-specific device tree and the matching boot DTB.
2. The WLAN node's final status and all board-level overrides.
3. The actual host interface and resource map.
4. Interrupt, DMA, IOMMU, shared-memory, and firmware-loading requirements.
5. Power, reset, clock, calibration, and regulatory dependencies.
6. Licensing and availability of any protocol documentation or reference code.

The generic ICNSS node is not a driver specification. Linux's `qca_cld3_wlan.ko` is not a drop-in XNU driver.

### Bluetooth research requirements

Before writing a transport driver, obtain:

1. The authoritative board schematic and exact board revision.
2. UART instance, pin mux, baud-rate behavior, and flow-control mapping.
3. Reset/enable GPIOs, clocks, regulators, and startup ordering.
4. Firmware download and controller initialization details.
5. A compatible host-controller transport and host-stack plan.

The Android QTI stack setting alone cannot answer these hardware questions.

## 4. Next evidence-gathering tasks

| Priority | Task | Exit condition |
|---|---|---|
| 1 | Locate the exact Hanoip board DTS/DTSI and determine how it includes/overrides the shared SM6150 source | Board-specific WLAN and Bluetooth nodes identified |
| 2 | Inspect the matching built DTB or device tree source for final node status and resources | No reliance on generic SoC defaults |
| 3 | Correlate WLAN resources with the actual wireless chip and host interface | Bus and chip identity supported by board-specific evidence |
| 4 | Correlate Bluetooth UART labels with SoC UART aliases and pinctrl | UART/pin mapping supported by board-specific evidence |
| 5 | Record firmware, calibration, power, and reset requirements | Initialization dependencies documented |

No flashing, bootloader modification, or device-level experimentation is part of this milestone. Keep this work source-only until the boot/recovery prerequisites and hardware facts are independently verified.

## References

- Hanoip Android device tree: https://github.com/motorola-sm6150-devs/device_motorola_hanoip
- Alternate Hanoip Android device tree: https://github.com/bhawesh2002/android_device_motorola_hanoip
- Shared SM6150 device-tree source: https://github.com/LineageOS/android_kernel_motorola_sm6150/blob/lineage-20/arch/arm64/boot/dts/qcom/sm6150.dtsi
- Project Hanoi schematic transcription (third-party host; provisional): https://www.scribd.com/document/1050732968/Moto-G60-e-G40-Funsiona-Esquematico

These sources support a source-level research audit only. They do not demonstrate XNU support or a working HanoipXNU build.
