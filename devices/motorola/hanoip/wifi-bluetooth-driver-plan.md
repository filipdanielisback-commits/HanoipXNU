# Wi-Fi and Bluetooth Driver Feasibility Plan

**Status:** Preliminary chip-identification evidence found; no driver implementation  
**Target:** Motorola Moto G60, XT2135-2 (`hanoip`)  
**Review date:** 2026-11-09

## Summary

Wi-Fi and Bluetooth are worthwhile research milestones, but neither is currently supported by HanoipXNU.

A third-party-hosted transcription of a Huaqin "Project Hanoi Rev V1" motherboard schematic identifies a combined wireless device as **WCN3980** on sheets 45–46 (dated 2020-11-26). This is the strongest device-specific lead found so far, but the copy is hosted on Scribd rather than an authoritative Motorola or Qualcomm document. Treat WCN3980 as **provisional**, not confirmed silicon identity.

Qualcomm's official Snapdragon 732G materials describe the platform as using the **FastConnect 6200 subsystem** and advertise Wi-Fi 6-ready features and Bluetooth 5.1. Qualcomm's current FastConnect 6200 product page lists WCN3991 and WCN3998 as part numbers. This does not cleanly match the WCN3980 label in the Hanoi schematic. The relationship between the board schematic label and the marketed FastConnect 6200 subsystem needs authoritative clarification.

**Current status: no XNU Wi-Fi driver, no XNU Bluetooth driver, and no verified XNU hardware support.**

## 1. Evidence found

### 1.1 Huaqin Project Hanoi schematic — provisional device-specific lead

A third-party-hosted schematic transcription titled "Moto G60 e G40 Funsiona Esquemático" includes:

- Company field: Huaqin.
- Project: Hanoi Rev V1.
- Date: 2020-11-26.
- Sheets 45–46 label the wireless device block **WCN3980**.
- The schematic labels WLAN RF/baseband/control signals and Bluetooth-related signals.
- It shows signals named `WCN_UART_TX_N`, `WCN_UART_RX_N`, `WCN_UART_CTS_N`, and `WCN_UART_RFR_N`, which are consistent with a UART connection for Bluetooth host communication.

This is useful evidence for the hanoip board design, but the hosted transcription's provenance and exact applicability to every XT2135-2 production revision have not been independently verified. Do not treat it as conclusive proof until matched against an official service schematic or a board-level identification source.

Source: https://www.scribd.com/document/1050732968/Moto-G60-e-G40-Funsiona-Esquematico

### 1.2 Qualcomm Snapdragon 732G platform information

Qualcomm's official Snapdragon 732G page says the platform includes a FastConnect 6200 subsystem, with Wi-Fi 6-ready capabilities and Bluetooth 5.1. Its product brief also identifies FastConnect 6200.

This establishes the wireless subsystem Qualcomm associates with the Snapdragon 732G platform. It does not by itself prove the exact discrete package marking fitted to this specific phone.

Sources:
- https://www.qualcomm.com/smartphones/products/7-series/snapdragon-732g-mobile-platform
- https://www.qualcomm.com/content/dam/qcomm-martech/dm-assets/documents/qualcomm-snapdragon-732g-mobile-platform-product-brief.pdf

### 1.3 Qualcomm FastConnect 6200 part-number information

Qualcomm's FastConnect 6200 product page describes a 2x2 Wi-Fi 5 / Bluetooth 5.2 connectivity SoC and lists WCN3991 and WCN3998 as part numbers. This differs from the WCN3980 label in the Project Hanoi schematic. The difference must remain an explicit unresolved discrepancy; do not silently substitute one model for another.

Source: https://www.qualcomm.com/wi-fi/products/fastconnect/fastconnect-6200

## 2. Android software evidence

Inspected source repositories:

- `motorola-sm6150-devs/device_motorola_hanoip`, branch `lineage-20`
- `bhawesh2002/android_device_motorola_hanoip`, branch `default`

### Wi-Fi evidence

The Lineage device tree's `BoardConfig.mk` maps the Android kernel module name `wlan.ko` to `qca_cld3_wlan.ko`. Its `modules.load` file lists `qca_cld3_wlan.ko`.

This establishes that this Android build expects a Qualcomm WLAN Linux kernel module. It does **not** identify the exact chip by itself or prove that the module supports XNU.

The alternate Hanoip device tree contains a Wi-Fi supplicant configuration file. That file contains Android Wi-Fi framework/supplicant settings; it is not a hardware driver.

### Bluetooth evidence

The alternate device tree contains:

- `bluetooth/bdroid_buildcfg.h`, with Android Bluetooth stack build settings, including BLE vendor support.
- A `BoardConfig.mk` declaration enabling the Qualcomm Bluetooth stack (`TARGET_USE_QTI_BT_STACK := true`).
- Android product declarations for Bluetooth and Bluetooth LE feature permissions.

The Project Hanoi schematic's UART signal labels are a useful lead for the Bluetooth transport, but they should be confirmed against a reliable schematic and exact board revision before implementation.

## 3. Current identification assessment

| Item | Current finding | Confidence |
|---|---|---|
| Wireless combo device label | WCN3980 appears in the Project Hanoi Rev V1 schematic transcription | **Provisional** — third-party-hosted copy |
| Platform wireless subsystem | Snapdragon 732G platform is described by Qualcomm as including FastConnect 6200 | **High** for the platform family; not a package marking |
| WCN3980 vs FastConnect 6200 part number | Qualcomm's FastConnect 6200 page lists WCN3991/WCN3998, not WCN3980 | **Unresolved discrepancy** |
| Bluetooth host transport | Schematic shows WCN UART TX/RX/CTS/RTS-style signals | **Provisional** |
| Wi-Fi host interface to application processor | Not established by inspected material | **Unknown** |
| Firmware and calibration filenames | Not established for this exact board revision | **Unknown** |
| Power/reset/clock sequencing | Some signal and rail labels appear in schematic transcription; complete validated sequence not extracted | **Unknown** |

Do not infer the exact chip from the SoC family, Android module name, or generic product specifications alone.

## 4. What a real XNU implementation would require

### Wi-Fi

A complete Wi-Fi path would require more than a kernel module:

1. Confirm the exact wireless silicon and its application-processor host interface.
2. Obtain reliable documentation for initialization, firmware, calibration, power, clocks, and interrupts.
3. Implement a compatible low-level XNU/IOKit driver for the actual hardware and bus.
4. Integrate with XNU networking and the chosen wireless control framework.
5. Provide user-space control/configuration for scanning, authentication, association, and network management.
6. Test power management, reconnect behavior, regulatory constraints, and error recovery.

Apple's published XNU source describes IOKit as its C++ driver framework. Apple's archived IOKit documentation describes a Network family for network controllers and interfaces. These references explain the general architecture, but do not supply a ready-made driver for this Qualcomm device.

### Bluetooth

A complete Bluetooth path would require:

1. Confirm the controller and UART wiring/flow-control details from authoritative board documentation.
2. Document reset, enable, clock, power, and firmware initialization requirements.
3. Implement XNU support for the transport and device control.
4. Integrate a compatible Bluetooth host-controller interface and host stack.
5. Provide user-space services for discovery, pairing, connections, and audio/input profiles.
6. Test Bluetooth Classic and BLE separately, plus sleep/wake and reconnection.

A kernel driver alone would not deliver a complete Bluetooth experience; the host stack and user-space services are also necessary.

## 5. Why Android drivers cannot simply be reused

The listed `qca_cld3_wlan.ko` is a Linux kernel module built for an Android/Linux kernel environment. Android Bluetooth configuration files configure Android's Bluetooth stack. Neither is a drop-in XNU/IOKit driver.

Potential reuse is limited to carefully reviewed, legally usable protocol knowledge or hardware documentation. Any code reuse would require license review and substantial adaptation; no compatibility is assumed.

## 6. Proposed development order

| Phase | Work | Exit condition |
|---|---|---|
| A | Verify WCN3980 label against an authoritative service schematic or board revision | Chip identity and exact board applicability confirmed |
| B | Resolve relationship to Qualcomm FastConnect 6200 / part numbers | No unresolved model-name substitution |
| C | Confirm Wi-Fi host interface and Bluetooth UART wiring | Bus and signal map tied to XT2135-2 |
| D | Document firmware, power, reset, clock, and interrupt dependencies | Hardware initialization requirements understood |
| E | Map XNU driver interfaces and user-space requirements | Written design reviewed; no unsupported code claims |
| F | Build isolated driver prototypes in a safe development environment | Reproducible build and tests on a documented test target |
| G | Only after a validated XNU boot path exists, evaluate device-level integration | Logs demonstrate actual enumeration and operation |

No device-level test should be attempted until the boot and recovery prerequisites are independently established. Do not flash the phone as part of this documentation milestone.

## 7. Current capability matrix

| Feature | Evidence | HanoipXNU status |
|---|---|---|
| Wi-Fi/BT combo chip | WCN3980 label in third-party-hosted Hanoi schematic transcription | **Provisional identification** |
| Platform wireless subsystem | Qualcomm FastConnect 6200 for Snapdragon 732G | **Platform-level reference** |
| Wi-Fi Linux module | `qca_cld3_wlan.ko` listed | **Android/Linux only** |
| Wi-Fi host interface | Not established | **Unknown** |
| Bluetooth UART | UART signal names in schematic transcription | **Provisional** |
| Firmware/calibration requirements | Not established | **Unknown** |
| XNU Wi-Fi driver | No Hanoip implementation found | **Not implemented** |
| XNU Bluetooth driver and host stack | No Hanoip implementation found | **Not implemented** |

## References

- Hanoip Android device tree: https://github.com/motorola-sm6150-devs/device_motorola_hanoip
- Alternate Hanoip Android device tree: https://github.com/bhawesh2002/android_device_motorola_hanoip
- Project Hanoi schematic transcription (third-party host): https://www.scribd.com/document/1050732968/Moto-G60-e-G40-Funsiona-Esquematico
- Qualcomm Snapdragon 732G: https://www.qualcomm.com/smartphones/products/7-series/snapdragon-732g-mobile-platform
- Qualcomm Snapdragon 732G product brief: https://www.qualcomm.com/content/dam/qcomm-martech/dm-assets/documents/qualcomm-snapdragon-732g-mobile-platform-product-brief.pdf
- Qualcomm FastConnect 6200: https://www.qualcomm.com/wi-fi/products/fastconnect/fastconnect-6200
- Apple XNU source and IOKit overview: https://github.com/apple-oss-distributions/xnu
- Apple IOKit architecture overview: https://developer.apple.com/library/archive/documentation/DeviceDrivers/Conceptual/IOKitFundamentals/ArchitectOverview/ArchitectOverview.html
- Apple IOKit Network family reference: https://developer.apple.com/library/archive/documentation/DeviceDrivers/Conceptual/IOKitFundamentals/Families_Ref/Families_Ref.html

These references document source material and general architecture. They do not demonstrate HanoipXNU support.
