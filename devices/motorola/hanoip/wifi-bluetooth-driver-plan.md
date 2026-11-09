# Wi-Fi and Bluetooth Driver Feasibility Plan

**Status:** Initial source audit; no driver implementation  
**Target:** Motorola Moto G60, XT2135-2 (`hanoip`)  
**Review date:** 2026-11-09

## Summary

Wi-Fi and Bluetooth are worthwhile research milestones, but neither is currently supported by HanoipXNU. The Android sources show that the Android/Linux build expects a Qualcomm WLAN kernel module and a Qualcomm Bluetooth stack configuration. Those are clues about the existing Android software path—not reusable XNU drivers.

**Current status: no XNU Wi-Fi driver, no XNU Bluetooth driver, and no verified XNU hardware support.**

## 1. What the Android source tree tells us

Inspected source repositories:

- `motorola-sm6150-devs/device_motorola_hanoip`, branch `lineage-20`
- `bhawesh2002/android_device_motorola_hanoip`, branch `default`

### Wi-Fi evidence

The Lineage device tree's `BoardConfig.mk` maps the Android kernel module name `wlan.ko` to `qca_cld3_wlan.ko`. Its `modules.load` file lists `qca_cld3_wlan.ko`.

This establishes that this Android build expects a Qualcomm WLAN Linux kernel module. It does **not** identify the exact Wi-Fi chip or prove that the module supports XNU.

The alternate Hanoip device tree contains a Wi-Fi supplicant configuration file. That file contains Android Wi-Fi framework/supplicant settings; it is not a hardware driver.

### Bluetooth evidence

The alternate device tree contains:

- `bluetooth/bdroid_buildcfg.h`, with Android Bluetooth stack build settings, including BLE vendor support.
- A `BoardConfig.mk` declaration enabling the Qualcomm Bluetooth stack (`TARGET_USE_QTI_BT_STACK := true`).
- Android product declarations for Bluetooth and Bluetooth LE feature permissions.

These are Android stack/build declarations. They do not identify the exact Bluetooth controller, its bus/transport, firmware requirements, or an XNU driver interface.

## 2. Hardware facts still unknown

The exact wireless combo-chip model for the specific XT2135-2 variant has not been verified from an authoritative hardware source.

Record the following as **unknown** until evidence is found:

- Wi-Fi chipset and silicon revision.
- Bluetooth controller model and whether it shares a combo module with Wi-Fi.
- Wi-Fi host interface (for example, PCIe or SDIO).
- Bluetooth host transport (for example, UART or USB).
- Required firmware and calibration data, and their licensing/redistribution status.
- Power, reset, clock, regulator, GPIO, and interrupt requirements.
- Supported Wi-Fi bands, standards, and Bluetooth feature set for this exact handset variant.

Do not infer the chipset from the Qualcomm SoC family or from the name of a Linux driver alone.

## 3. What a real XNU implementation would require

### Wi-Fi

A complete Wi-Fi path would require more than a kernel module:

1. Verified identification of the physical WLAN device and host interface.
2. A compatible low-level XNU/IOKit driver for that hardware and bus.
3. Firmware loading and device initialization, where required.
4. Integration with the relevant XNU networking and wireless framework interfaces.
5. A user-space control and configuration path for scanning, authentication, association, and network management.
6. Testing of power management, reconnect behavior, regulatory constraints, and error recovery.

Apple's published XNU source describes IOKit as its C++ driver framework. Apple's archived IOKit documentation describes a Network family for network controllers and interfaces. These references explain the general architecture, but do not supply a ready-made driver for this Qualcomm device.

### Bluetooth

A complete Bluetooth path would require:

1. Verified controller model and transport.
2. XNU support for the controller's bus/transport and power/reset sequencing.
3. Firmware and initialization support, if needed.
4. A host-controller interface implementation compatible with the chosen Bluetooth host stack.
5. User-space services and APIs for discovery, pairing, connections, and audio/input profiles.
6. Tests for Bluetooth Classic and BLE separately, plus sleep/wake and reconnection.

A kernel driver alone would not deliver a complete Bluetooth experience; the host stack and user-space services are also necessary.

## 4. Why Android drivers cannot simply be reused

The listed `qca_cld3_wlan.ko` is a Linux kernel module built for an Android/Linux kernel environment. Android Bluetooth configuration files configure Android's Bluetooth stack. Neither is a drop-in XNU/IOKit driver.

Potential reuse is limited to carefully reviewed, legally usable protocol knowledge or hardware documentation. Any code reuse would require license review and substantial adaptation; no compatibility is assumed.

## 5. Proposed development order

| Phase | Work | Exit condition |
|---|---|---|
| A | Identify exact Wi-Fi and Bluetooth silicon and buses | Evidence tied to XT2135-2, not just the hanoip family |
| B | Document firmware, power, reset, and interrupt dependencies | Hardware initialization requirements understood |
| C | Map XNU driver interfaces and user-space requirements | Written design reviewed; no unsupported code claims |
| D | Build isolated driver prototypes in a safe development environment | Reproducible build and tests on a documented test target |
| E | Only after a validated XNU boot path exists, evaluate device-level integration | Logs demonstrate actual enumeration and operation |

No device-level test should be attempted until the boot and recovery prerequisites are independently established. Do not flash the phone as part of this documentation milestone.

## 6. Current capability matrix

| Feature | Android source evidence | HanoipXNU status |
|---|---|---|
| Wi-Fi Linux module | `qca_cld3_wlan.ko` listed | **Not implemented** |
| Wi-Fi chip identity | Not established by inspected files | **Unknown** |
| Bluetooth Android stack | Qualcomm stack configuration present | **Not implemented** |
| Bluetooth controller identity | Not established by inspected files | **Unknown** |
| Bluetooth transport | Not established by inspected files | **Unknown** |
| Firmware/calibration requirements | Not established | **Unknown** |
| XNU driver and user-space stack | No Hanoip implementation found | **Not implemented** |

## References

- Hanoip Android device tree: https://github.com/motorola-sm6150-devs/device_motorola_hanoip
- Alternate Hanoip Android device tree: https://github.com/bhawesh2002/android_device_motorola_hanoip
- Apple XNU source and IOKit overview: https://github.com/apple-oss-distributions/xnu
- Apple IOKit architecture overview: https://developer.apple.com/library/archive/documentation/DeviceDrivers/Conceptual/IOKitFundamentals/ArchitectOverview/ArchitectOverview.html
- Apple IOKit Network family reference: https://developer.apple.com/library/archive/documentation/DeviceDrivers/Conceptual/IOKitFundamentals/Families_Ref/Families_Ref.html

These references document the source material and general architecture. They do not demonstrate HanoipXNU support.
