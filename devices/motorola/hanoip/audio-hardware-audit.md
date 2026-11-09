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


## 6. Physical audio endpoints and board-documentation leads (2026-11-09)

A second source pass found useful endpoint-level evidence, but it still does not settle the codec IC.

### 6.1 Motorola user guide: externally visible audio hardware

Motorola's Moto G60 user guide identifies these audio-related physical endpoints:

- Headset jack.
- Earpiece.
- A microphone on the upper/front-side hardware diagram.
- A second microphone on the rear/bottom-side diagram.
- Loudspeaker.

This independently supports the existence of headset, earpiece, speaker, and two microphone endpoints at the product level. It does not identify their codec pins, analog/digital interface, amplifier part number, or board revision-specific routing.

### 6.2 XT2135-X schematic/service-manual listing

A public repair-document listing describes a motherboard schematic and service manual for the XT2135-X G60/G40 family, and labels the platform as Hanoi / SM-7150. This is a potentially valuable board-level source lead, but the listing itself is not the schematic contents. The exact XT2135-2 board revision, schematic revision, and audio pages have not been independently inspected or cross-checked against a physical board.

Therefore, do not treat the listing's chipset label as resolving the project's existing `sm6150` versus `SM7150` discrepancy.

### 6.3 Charging-board microphone spare-part listing

A third-party spare-parts catalogue lists a charging connector/microphone board as compatible with Moto G60 XT2135-2. This supports the existence of a microphone associated with the lower charging-board assembly, but it is not a board schematic and does not identify the microphone component, signal interface, or codec connection. It should be treated as corroborating commercial parts evidence only.

### 6.4 Codec-family caution

The Android source tree's WCD934x/WCD937x/WCD938x and WSA881x references remain software configuration clues. Linux kernel Kconfig documentation describes these as distinct codec families with different integration requirements (for example, SoundWire dependencies for WCD937x/WCD938x); generic upstream driver documentation cannot establish which one is populated on this phone.

No exact codec or external speaker amplifier should be selected for an XNU implementation until the board-specific schematic or a trustworthy component marking is tied to the exact XT2135-2 board revision.

## 7. Updated evidence table

| Hardware fact | Current evidence | Status |
|---|---|---|
| Headset jack | Motorola user guide hardware diagram | Confirmed at product level |
| Earpiece | Motorola user guide hardware diagram | Confirmed at product level |
| Loudspeaker | Motorola user guide hardware diagram | Confirmed at product level |
| Two microphone endpoints | Motorola user guide diagrams; Android platform-info lists two built-in microphones | Confirmed at product/configuration level; exact routing unknown |
| Lower microphone associated with charging board | Third-party XT2135-2 spare-parts listing | Corroborating, not schematic-grade |
| Qualcomm audio routing backends | Android audio platform XML | Confirmed in inspected Android configuration |
| Physical MI2S / CDC-DMA / SoundWire wiring | Hanoip Linux DT overlay configures secondary MI2S pins and RX/TX SoundWire macro paths; physical schematic correlation remains pending | Configured in Linux DT; electrical implementation not independently verified |
| Codec IC and revision | Hanoip Linux DT explicitly enables a `qcom,wcd937x-codec` node and references WCD937x RX/TX SoundWire slaves | Strong device-tree configuration evidence; exact fitted part/revision still needs board-level corroboration |
| External speaker amplifier | Hanoip Linux DT configures an `awinic,aw882xx_smartpa` I2C node; WSA speaker path is disabled in that overlay | Strong device-tree configuration evidence; exact fitted part/revision still needs board-level corroboration |
| Audio power/reset/clock sequencing | Not yet established from exact board schematic and device tree | Unknown |
| XNU codec/controller driver | No implementation | Not implemented |

## 8. Immediate research gate before XNU hardware code

The next meaningful step is not writing register accesses. It is obtaining and validating board-specific evidence for the exact XT2135-2 revision:

1. Identify the schematic revision and board revision that correspond to XT2135-2.
2. Inspect only the audio-related schematic pages and extract component reference designators, IC markings/part numbers, bus nets, clocks, reset lines, regulators, interrupts, and speaker/microphone connections.
3. Cross-reference those nets against the Hanoip device-tree audio nodes and kernel driver bindings.
4. Reconcile those findings with the Android mixer/backend names and module aliases.
5. Record any missing proprietary DSP firmware/calibration dependency and its redistribution status.

Until those steps are complete, keep the hardware-specific XNU audio backend unimplemented. Do not guess codec register maps, invent device-tree nodes, or label a generic Qualcomm audio component as the handset's confirmed hardware.

## Additional references

- Motorola Moto G60 User Guide, hardware diagrams (headset jack, earpiece, microphones, speaker): https://help.motorola.com/hc/3260/11/pdf/help-moto-g60-11-global-en-us.pdf
- XT2135-X G60/G40 schematic/service-manual listing (source lead only; schematic pages not independently verified): https://eletronicabr.com/en/topic/309099-xt2135-x-g60-and-g40-motherboard-service-manual-schematic/
- XT2135-2 charging connector/microphone spare-part listing (third-party parts evidence): https://www.gsmnet.ro/piese-componente-conectori-incarcare/placa-cu-conector-incarcare-microfon-motorola-moto-g60-service-pack-5p68c18349
- Linux ASoC codec Kconfig reference (generic driver-family information, not handset identification): https://android.googlesource.com/kernel/common/+/1bd3a76880b2bce017987cf53780b372cf59528e/sound/soc/codecs/Kconfig


## 9. Hanoip-specific Linux device-tree audio audit (2026-11-09)

A source pass through the `LineageOS/android_kernel_motorola_sm6150` repository's `lineage-20` branch found Hanoip-specific overlays. This materially improves the evidence compared with generic SoC files and Android mixer XML.

### 9.1 Overlay inclusion chain

The kernel DT build lists three Hanoip overlays:

- `sdmmagpie-hanoip-evt1-overlay.dts`
- `sdmmagpie-hanoip-dvt1-overlay.dts`
- `sdmmagpie-hanoip-dvt2-overlay.dts`

Each includes `sdmmagpie-hanoip-common-overlay.dtsi`. That common overlay includes both `sdmmagpie-audio-overlay.dtsi` and `sdmmagpie-hanoi-audio-overlay.dtsi`. The first includes the shared `sm6150-audio-overlay.dtsi`; the second applies Hanoi/Hanoip-specific overrides.

This means the following findings are present in the inspected Hanoip Linux device-tree configuration across those listed overlay variants. It does not prove which overlay or exact board revision corresponds to a particular retail XT2135-2 handset.

### 9.2 Codec configuration: WCD937x

The Hanoip-specific audio overlay:

- sets the `&bolero` codec block to `okay`;
- enables `&wcd937x_codec`;
- enables `&wcd937x_rst_gpio`;
- enables the RX and TX SoundWire GPIO/pinctrl groups;
- disables the WSA SoundWire GPIO group and WSA speaker-enable groups;
- disables the WCD934x reset path and legacy Slimbus audio nodes;
- configures codec mic-bias values of 2800 mV in the Hanoip overlay.

The shared SM6150 audio overlay defines the WCD937x codec node as `compatible = "qcom,wcd937x-codec"`, with separate RX and TX SoundWire slave nodes. It also describes codec supply references and external audio clock references. These are Linux device-tree resource declarations, not a safe XNU initialization sequence.

**Evidence interpretation:** WCD937x is now the leading codec-family candidate in the Hanoip Linux device-tree configuration. This is stronger than module-name inference. Exact silicon marking, board revision, and physical net mapping still need confirmation before implementing a register-level driver.

### 9.3 Secondary MI2S configuration

The Hanoip-specific overlay defines secondary MI2S pin states on the LPI TLMM:

| Signal role in DTS | Pin | Function |
|---|---:|---|
| Secondary MI2S clock | GPIO8 | func3 |
| Secondary MI2S word select | GPIO9 | func3 |
| Secondary MI2S data 0 | GPIO10 | func4 |
| Secondary MI2S data 1 | GPIO11 | func2 |

The overlay groups these into `cdc_sec_mi2s_gpios` and attaches that group to `qcom,sec-mi2s-gpios`. It enables the secondary Q6 MI2S DAI node and declares one RX line and two TX lines.

These are concrete pin/function assignments in the Linux DT source. They do not independently prove which external components are connected to each line or whether every listed signal is used by the retail board revision under investigation.

### 9.4 External smart-amplifier configuration

The Hanoip-specific overlay adds an I2C child under `&qupv3_se2_i2c`:

- compatible: `awinic,aw882xx_smartpa`;
- I2C address: `0x34`;
- reset GPIO reference: TLMM GPIO43;
- IRQ GPIO reference: TLMM GPIO91;
- AFE RX port ID: `0x1002`;
- AFE TX port ID: `0x1003`.

This is strong evidence that the Hanoip Linux configuration expects an AW882xx-family smart-amplifier device on that I2C controller. The exact AW882xx chip variant, board population, interrupt polarity/trigger interpretation, reset timing, and required firmware/configuration still need independent verification. Do not write an I2C register driver from these properties alone.

### 9.5 SoundWire, DSP, and DMA implications

The shared SM6150 audio DT defines Q6/ADSP audio service nodes, RX/TX codec macro blocks, SoundWire masters/slaves, and a large set of Q6 DAI/CDC-DMA endpoints. The Hanoip overlay enables RX/TX SoundWire paths while disabling the WSA path and Slimbus audio nodes.

This supports a likely architecture with:
- Qualcomm Q6/ADSP audio services in the Android implementation;
- WCD937x codec control/data paths over RX/TX SoundWire;
- a separate secondary MI2S interface configured by the Hanoip overlay;
- AW882xx smart-amplifier control over I2C, with audio data/processing coordinated through Qualcomm AFE port identifiers.

The precise playback/capture route for each handset endpoint remains to be mapped from the Hanoip mixer XML, codec routing, DSP port configuration, and board schematic. Do not infer that every generic Q6 DAI or CDC-DMA endpoint is enabled or used on Hanoip.

### 9.6 Updated evidence and implementation status

| Item | Current finding | Confidence / next proof |
|---|---|---|
| Codec family | WCD937x node explicitly enabled in Hanoip Linux DT | High for DT configuration; verify physical chip marking and exact board revision |
| Codec digital link | RX/TX SoundWire masters and WCD937x slaves configured | High for DT configuration; map actual board nets |
| Secondary MI2S | GPIO8–GPIO11 and secondary DAI configured | High for DT configuration; verify schematic connectivity |
| Smart amplifier | AW882xx-family I2C node at address 0x34 | High for DT configuration; identify exact chip and board population |
| Amplifier control resources | GPIO43 reset, GPIO91 IRQ, AFE port IDs 0x1002/0x1003 | High for DT property values; electrical behavior and driver protocol not verified |
| WSA881x speaker path | WSA macro/speaker-enable path disabled in Hanoip overlay | High for this DT overlay; do not treat WSA881x as Hanoip's selected amplifier |
| Codec rails/clocks | Shared DT contains supply and clock declarations | Configuration evidence only; regulator/clock sequencing and board mapping require review |
| DSP firmware/calibration | Android vendor dependencies remain incompletely inventoried | Unknown/legal review required |
| XNU implementation | No XNU controller, DSP, codec, or amplifier backend | Not implemented |

### 9.7 Next source-only tasks

1. Match the exact XT2135-2 board revision to one of the Hanoip EVT/DVT device-tree overlays.
2. Inspect the actual schematic pages for WCD937x, AW882xx, SoundWire, secondary MI2S, I2C, reset, IRQ, clocks, and supplies.
3. Trace the Android mixer routes to the now-identified candidate backends and DSP/AFE port IDs.
4. Inspect the Linux driver sources for the WCD937x and AW882xx compatible strings to document protocols and firmware/calibration dependencies; use them as behavioral references, not code to copy into XNU.
5. Pin the XNU revision and decide whether a platform-neutral mocked backend is the next implementation step while board-level evidence is being verified.

### 9.8 Kernel source references

All references below point to the inspected `lineage-20` branch of the public LineageOS Motorola SM6150 kernel repository:

- Hanoip EVT1 overlay: https://github.com/LineageOS/android_kernel_motorola_sm6150/blob/lineage-20/arch/arm64/boot/dts/qcom/sdmmagpie-hanoip-evt1-overlay.dts
- Hanoip DVT1 overlay: https://github.com/LineageOS/android_kernel_motorola_sm6150/blob/lineage-20/arch/arm64/boot/dts/qcom/sdmmagpie-hanoip-dvt1-overlay.dts
- Hanoip DVT2 overlay: https://github.com/LineageOS/android_kernel_motorola_sm6150/blob/lineage-20/arch/arm64/boot/dts/qcom/sdmmagpie-hanoip-dvt2-overlay.dts
- Hanoip common overlay include chain: https://github.com/LineageOS/android_kernel_motorola_sm6150/blob/lineage-20/arch/arm64/boot/dts/qcom/sdmmagpie-hanoip-common-overlay.dtsi
- Hanoip audio overlay: https://github.com/LineageOS/android_kernel_motorola_sm6150/blob/lineage-20/arch/arm64/boot/dts/qcom/sdmmagpie-hanoi-audio-overlay.dtsi
- Shared SM6150 audio overlay (WCD937x codec, SoundWire nodes and resources): https://github.com/LineageOS/android_kernel_motorola_sm6150/blob/lineage-20/arch/arm64/boot/dts/qcom/sm6150-audio-overlay.dtsi
- Shared SM6150 audio controller/DAI definitions: https://github.com/LineageOS/android_kernel_motorola_sm6150/blob/lineage-20/arch/arm64/boot/dts/qcom/sm6150-audio.dtsi

These are Linux DT source references. They do not demonstrate an XNU driver, a successful XNU build, or working audio on the target handset.
