# Hardware Support Matrix

No target-device hardware feature has been validated by this project. The entries marked **Investigating** mean that source research exists; they do not mean that a driver works.

| Component | Status | Evidence / next requirement |
|---|---|---|
| Kernel boot | Not tested | Reproducible boot proof and logs |
| Display | Not tested | Panel identification and driver feasibility |
| Touchscreen | Not tested | Controller identification and input tests |
| Storage | Not tested | Controller and filesystem integration research |
| Wi-Fi | Investigating | [Wireless interface audit](../devices/motorola/hanoip/wireless-interface-audit.md); exact board host interface and XNU driver remain unknown |
| Bluetooth | Investigating | [Wireless interface audit](../devices/motorola/hanoip/wireless-interface-audit.md); controller/transport evidence is provisional and no XNU stack exists |
| Audio | Investigating | [Audio hardware audit](../devices/motorola/hanoip/audio-hardware-audit.md); Android routing/module evidence found, exact physical codec remains unconfirmed |
| Battery / charging | Not tested | Power-management interface research |
| Sensors | Not tested | Sensor inventory and interface research |
| GPU acceleration | Not tested | GPU architecture and driver feasibility |
| Userland / apps | Not tested | Runtime and ABI compatibility research |

## Preliminary family-level reference specifications

A public TeamWin device configuration for the Moto G40/G60 family lists Snapdragon 732G, Kryo 470 CPU cores, Adreno 618 graphics, 4/6 GB RAM options, and a 1080 × 2400 display. Treat these as provisional family-level reference data, not proof of the exact components in every XT2135-2 hardware revision.

There is conflicting community labeling of the Qualcomm platform identifier (`sm6150` vs. `SM7150`). This project leaves the identifier unresolved pending stronger evidence.

References:
- [TeamWin hanoip device tree](https://github.com/TeamWin/android_device_motorola_hanoip)
- [motorola-sm6150-devs hanoip device tree](https://github.com/motorola-sm6150-devs/device_motorola_hanoip)
- [Archived Android/Linux kernel tree for hanoip](https://github.com/Dark-Matter7232/CosmicFresh-Hanoip)

The kernel tree is not an XNU port and is not imported by this project.

## Status definitions
- **Not tested:** no project test has been performed.
- **Investigating:** research is underway; no working support is implied.
- **Experimental:** a limited test exists and is documented with evidence.
- **Working:** repeatable tests pass on the explicitly identified hardware and software revision.

Do not promote a feature to Working based only on a successful build or a test on a different device.
