# Device registry

Device-specific work belongs under `devices/<vendor>/<codename>/`. The current target is `devices/motorola/hanoip/`.

The intended layering is:
- common project kernel and drivers;
- SoC-family research under `platform/qualcomm/`;
- exact device/board configuration under `devices/motorola/hanoip/`.

This follows a common OS-porting separation of concerns, but the directory names, metadata, scripts, and implementation must be HanoipXNU's own. No HakiOS source or patches are copied.

A multi-device manifest is deferred until there is more than one validated target.
