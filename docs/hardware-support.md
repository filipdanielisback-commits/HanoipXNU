# Hardware Support Matrix

No target-device hardware feature has been validated by this project yet.

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

## Status definitions
- **Not tested:** no project test has been performed.
- **Investigating:** research is underway; no working support is implied.
- **Experimental:** a limited test exists and is documented with evidence.
- **Working:** repeatable tests pass on the explicitly identified hardware and software revision.

Do not promote a feature to Working based only on a successful build or a test on a different device.
