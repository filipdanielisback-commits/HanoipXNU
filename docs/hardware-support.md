# Hardware Support Matrix

No target-device hardware feature has been validated by this project.

| Component | Status | Evidence needed |
|---|---|---|
| Kernel boot | Not tested | Reproducible boot proof and logs |
| Display | Not tested | Panel identification and driver feasibility |
| Touchscreen | Not tested | Controller identification and input tests |
| Storage | Not tested | Controller and filesystem integration research |
| Wi-Fi | Not tested | Chipset identification, firmware and driver licensing review |
| Bluetooth | Not tested | Controller and stack compatibility research |
| Audio | Not tested | Codec, routing, and driver research |
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
