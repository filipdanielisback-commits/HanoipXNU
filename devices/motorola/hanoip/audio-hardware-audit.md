# Audio Hardware Audit

**Status:** Android-source audit; no XNU implementation or device test  
**Target:** Motorola Moto G60 XT2135-2 (`hanoip`)  
**Review date:** 2026-11-09

## Summary

The inspected Hanoip Android device tree contains a substantial Qualcomm audio configuration: mixer paths, platform routing information, audio policy, speaker attenuation configuration, microphone characteristics, and a set of Qualcomm audio kernel modules and proprietary Android audio libraries.

This is useful evidence for reconstructing the Android audio software path. It does **not** yet identify the exact physical codec/amplifier combination on every XT2135-2 board revision, and it does not provide an XNU driver.

**Current HanoipXNU status: audio is not implemented or tested.**

## 1. Source inspected

Primary reference:

- Hanoip Android device tree: https://github.com/bhawesh2002/android_device_motorola_hanoip

Files inspected in the repository include:

- `audio/audio_platform_info.xml`
- `audio/mixer_paths.xml`
- `audio/audio_policy_configuration.xml`
- `audio/audio_ext_spkr.conf`
- `rootdir/etc/init.target.rc`
- `BoardConfig.mk`
- `device.mk`
- `proprietary-files.txt`

These files belong to an Android/Linux build configuration. They are reference material only, not code that can be loaded by XNU.

## 2. Findings

### 2.1 Audio routing and interfaces

The Android `audio_platform_info.xml` maps logical audio devices to Qualcomm backend/interface names. Examples include:

- Speaker output mapped to `SEC_MI2S_RX`.
- Handset and headphone paths mapped to `RX_CDC_DMA_RX_0`.
- Microphone capture paths mapped to `TX_CDC_DMA_TX_3`.
- Bluetooth A2DP/SCO paths referencing `SLIMBUS_7_RX` and `SLIMBUS_7_TX`.

This indicates that the Android configuration expects multiple Qualcomm audio interfaces and distinct routing paths for speaker, handset, headset, microphone, and Bluetooth audio.

**Limit:** these names are software routing identifiers. They do not, by themselves, prove the exact physical wiring or which interface is enabled on the final XT2135-2 board.

### 2.2 Qualcomm audio module family

The inspected Android init configuration attempts to load modules with names including:

- `audio_q6`, `audio_apr`, `audio_adsp_loader`
- `audio_swr`, `audio_platform`
- `audio_wcd_core`, `audio_bolero_cdc`
- `audio_wsa881x`, `audio_wsa_macro`
- `audio_va_macro`, `audio_rx_macro`, `audio_tx_macro`
- `audio_wcd938x` and `audio_wcd938x_slave`

The BoardConfig module-alias list also contains WCD934x/WCD937x-related names and a machine-driver alias that does not exactly match the machine-driver name in the inspected init script.

This inconsistency matters: the repository may contain configuration carried across related Qualcomm targets or revisions. The module names are clues about the expected driver family, **not sufficient proof that the handset physically contains a particular WCD codec or WSA amplifier**.

### 2.3 Audio policy and calibration

The device build references Android audio policy, mixer-path and platform-info files. Its proprietary file list includes Qualcomm audio libraries and calibration-related components, including ACDB loader/runtime libraries and audio configuration libraries.

The vendor property file references calibration data for speaker, headset, general audio, codec, Bluetooth, and HDMI paths. Those references demonstrate that the Android vendor stack expects calibration assets. They do not establish that those proprietary blobs are available for lawful reuse in an XNU project.

### 2.4 Microphone configuration

The inspected platform-info file describes two built-in microphone entries and their characteristics. Treat this as Android configuration evidence only. It is not independent confirmation of the physical microphone count, placement, or exact board revision.

### 2.5 Bluetooth audio relationship

The Android audio routing configuration contains A2DP and SCO device paths. This shows how Android's audio subsystem is configured to route Bluetooth audio. It does not prove that HanoipXNU Bluetooth transport, controller firmware, audio profiles, or host stack work.

Wi-Fi and Bluetooth hardware findings remain in [wireless-interface-audit.md](wireless-interface-audit.md) and [wifi-bluetooth-driver-plan.md](wifi-bluetooth-driver-plan.md).

## 3. Findings and confidence

| Question | Finding | Confidence |
|---|---|---|
| Does the inspected Android tree include device-specific audio configuration? | Yes: mixer paths, platform routing, policy, speaker configuration | High for that source tree |
| Are Qualcomm audio DSP/codec driver modules referenced? | Yes: Q6/ADSP, APR, WCD/Bolero/WSA-related names | High for the configuration; not proof of fitted components |
| Are MI2S, CDC DMA and Slimbus identifiers used in routing? | Yes | High for the XML configuration |
| Is the exact physical codec confirmed for XT2135-2? | No; WCD family references conflict across files | Unknown |
| Is the exact external speaker amplifier confirmed? | No; WSA881x is referenced but not independently tied to board revision | Provisional |
| Are calibration assets and initialization requirements fully understood? | No | Unknown |
| Does HanoipXNU have an audio driver? | No | Not implemented |
| Has audio been tested under XNU on this phone? | No | Not tested |

## 4. What a genuine XNU audio implementation would require

A working audio path needs more than identifying a codec name. Research would need to establish:

1. The exact board revision and authoritative codec/amplifier identification.
2. The audio DSP, codec, amplifier, and physical bus topology.
3. Clock, reset, regulator, interrupt, pin-control, and power sequencing.
4. Firmware and calibration dependencies, including licensing and redistribution rights.
5. Supported PCM formats, sample rates, channel layouts, and low-power modes.
6. XNU/IOKit driver design for the relevant controller and codec interfaces.
7. An audio service and routing layer for playback, capture, volume, jack/speaker selection, and Bluetooth audio.
8. Repeatable tests on an explicitly identified target and software revision.

The Android mixer XML and Qualcomm Linux modules cannot simply be copied into XNU. The kernel interfaces, driver frameworks, DSP communication, calibration handling, and user-space audio services differ.

## 5. Next source-only tasks

| Priority | Task | Exit condition |
|---|---|---|
| 1 | Compare the Hanoip audio module aliases, init module list, and exact kernel module build outputs | Explain the WCD934x/WCD937x/WCD938x and machine-driver naming differences |
| 2 | Find the exact XT2135-2 board-specific sound/codec device-tree nodes | Codec, amplifier and bus claims tied to board-specific evidence |
| 3 | Correlate audio routing backends with the soundwire/MI2S/CDC-DMA controller nodes | Interface topology documented without assuming XML names equal physical wiring |
| 4 | Inventory firmware and calibration dependencies by filename and license | Required assets and legal status recorded |
| 5 | Draft a platform-neutral XNU audio architecture note | Driver boundaries and user-space responsibilities reviewed |

No phone modification, flashing, or device-level experimentation is part of this audit.

## References

- Hanoip Android device tree: https://github.com/bhawesh2002/android_device_motorola_hanoip
- Device build configuration: https://github.com/bhawesh2002/android_device_motorola_hanoip/blob/default/device.mk
- Android audio platform configuration: https://github.com/bhawesh2002/android_device_motorola_hanoip/tree/default/audio
- Shared Motorola SM6150 Android device configuration: https://github.com/LineageOS/android_device_motorola_sm6150-common
- Shared Motorola SM6150 Linux kernel source: https://github.com/LineageOS/android_kernel_motorola_sm6150

These sources support an Android-source audit only. They do not demonstrate an XNU audio driver, a successful HanoipXNU build, or working audio on the target phone.
