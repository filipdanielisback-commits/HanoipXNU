# Hanoip Android Device-Tree Audit

**Status:** Static source inspection; no device tests performed  
**Target:** Motorola Moto G60, XT2135-2, codename `hanoip`  
**Source inspected:** `motorola-sm6150-devs/device_motorola_hanoip`, branch `lineage-20`  
**Review date:** 2026-11-09

## Scope and warning

This document records what the inspected Android device configuration declares. It is not an XNU configuration, does not prove that every value applies to every retail variant, and does not demonstrate that XNU can boot on the phone. No commands were run on the handset and no partitions or firmware were changed.

## 1. Architecture and platform declarations

From `BoardConfig.mk`:

| Setting | Declared value | Interpretation |
|---|---|---|
| Primary architecture | `arm64` | Android build targets 64-bit ARM. |
| Primary architecture variant | `armv8-2a-dotprod` | Build configuration requests this ARM architecture feature level. |
| Primary CPU variant | `cortex-a76` | Android build-time CPU tuning declaration; not independent proof of the exact physical core topology. |
| Secondary architecture | `arm` | A 32-bit secondary Android ABI is configured. |
| Secondary CPU variant | `cortex-a55` | Secondary ABI tuning declaration. |
| Board platform | `sm6150` | Android device configuration's platform identifier. |
| Bootloader board name | `hanoip` | Board-name string used by this build configuration. |
| OTA device assertion | `hanoip` | Device codename expected by this Android build. |

**Important:** Another Hanoip device-tree source uses the label `SM7150 Snapdragon 732G`. The inspected `BoardConfig.mk` and related Android configuration use `sm6150`. These labels must remain recorded as source declarations, not silently normalized into one supposedly verified hardware identity. Confirm against authoritative Motorola/Qualcomm documentation for the exact XT2135-2 variant before using them in low-level work.

## 2. Android kernel build declarations

The inspected configuration declares:

- Kernel source path: `kernel/motorola/sm6150`.
- Kernel image name: `Image`.
- Kernel base: `0x00000000`.
- Kernel page size: `4096`.
- Separate DTBO image: enabled.
- Boot header version: `3`.
- Device tree included in boot image: enabled.
- Ramdisk compression: LZ4.
- Kernel configuration fragments:
  - `vendor/sdmsteppe-perf_defconfig`
  - `vendor/ext_config/moto-sdmsteppe.config`
  - `vendor/ext_config/moto-sdmmagpie-hanoip.config`
  - `vendor/debugfs.config`
  - `vendor/ext_config/lineage-moto-sdmsteppe.config`

These are Android/Linux build conventions and kernel configuration inputs. They do not define an XNU boot protocol or prove compatibility with Apple's kernel.

## 3. Boot command-line clues

The Android command line includes strings such as:

- `console=ttyMSM0,115200n8`
- `androidboot.hardware=qcom`
- `androidboot.console=ttyMSM0`
- `androidboot.usbcontroller=a600000.dwc3`
- `earlycon=msm_geni_serial,0x880000`
- `firmware_class.path=/vendor/firmware_mnt/image`
- `androidboot.hab.product=hanoip`

These strings are useful leads for later documentation research into the Qualcomm platform, serial console, USB controller, firmware loading, and boot metadata. They are **not** instructions for an XNU loader, and they do not establish the memory map, security state, entry point, or handoff contract needed by XNU.

## 4. Display and storage layout declarations

From `device.mk` and `BoardConfig.mk`:

| Setting | Declared value |
|---|---|
| Android build screen width | 1080 |
| Android build screen height | 2460 |
| Android logical LCD density | 440 |
| Dynamic partitions | Enabled |
| A/B OTA partition list | Includes boot, dtbo, product, system, system_ext, vbmeta, vbmeta_system, vendor, vendor_boot |
| Filesystem declarations | ext4 for configured system/vendor partitions; ext4 and F2FS user-image support enabled |

These are Android product/build settings, not a complete physical partition map. They do not authorize repartitioning or flashing.

## 5. Dependencies and proprietary components

The inspected `lineage.dependencies` requests the separate `android_hardware_motorola` repository at `hardware/motorola`.

The `device.mk` inherits vendor product configuration from `vendor/motorola/hanoip`. `BoardConfig.mk` also includes vendor configuration from Hanoip vendor and firmware trees.

The `proprietary-files.txt` manifest lists proprietary Android components, including Qualcomm-related libraries and services, Motorola camera and settings components, radio/IMS components, and other vendor libraries. The manifest is evidence that the Android build expects additional vendor material; it is not the binaries themselves and does not grant redistribution rights.

For this project:
- Do not copy proprietary blobs into this repository.
- Do not assume Android HALs or Linux kernel modules can be used by XNU.
- Track the existence of required vendor components separately from whether lawful redistribution or an XNU-compatible implementation is available.

## 6. What this audit does not tell us

The inspected files do not establish:

- The exact XT2135-2 board revision or whether all G60 variants share identical hardware.
- A complete physical memory map or reserved-memory layout.
- The boot ROM / bootloader chain and a documented third-party kernel handoff.
- The precise interrupt-controller, timer, IOMMU, power-management, or device-tree requirements for XNU.
- Working XNU drivers for display, touch, storage, USB, audio, camera, Wi-Fi, Bluetooth, cellular modem, or sensors.
- Availability of an iOS userland or proprietary Apple frameworks.

## 7. Current assessment

**Verified from this source revision:** the Android device tree configures an ARM64 Android/Linux build for the `hanoip` codename, declares `sm6150` as its board platform, references a separate Motorola kernel source tree, and depends on external vendor configuration and proprietary Android components.

**Not verified:** a working XNU build for this platform, an XNU bootloader, any successful XNU boot on XT2135-2, or an iOS installation.

## 8. Next research tasks

1. Cross-check the platform and CPU declarations against authoritative documentation for the exact XT2135-2 variant.
2. Inspect the referenced Android kernel tree's public documentation and configuration metadata for architecture and platform clues; do not treat Linux drivers as XNU drivers.
3. Locate authoritative documentation for the handset's boot stages and kernel handoff. If no public specification exists, record that as an unresolved blocker.
4. Build a hardware capability matrix with evidence and confidence labels. Keep all untested XNU support marked **unknown**.

## Sources

- Hanoip Android device tree, `lineage-20`: https://github.com/motorola-sm6150-devs/device_motorola_hanoip
- Hanoip Android device-tree alternate repository and family specification: https://github.com/bhawesh2002/android_device_motorola_hanoip
- TeamWin Hanoip recovery device tree and family specification: https://github.com/Motorola-SM6150/android_device_motorola_hanoip-TWRP

The source repository is third-party Android development material. Its contents are treated as technical references, not as an endorsement or proof of XNU compatibility.
