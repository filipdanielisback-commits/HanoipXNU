# Boot Process Research

## Status
The target device boot chain has not yet been independently documented in this repository. No custom loader or boot image is provided.

## Research goals
- Identify the documented boot stages for the exact XT2135-2 variant.
- Understand the supported boot-image and kernel handoff formats.
- Determine which interfaces are available to a third-party operating system.
- Establish whether an emulator can validate any part of the proposed design.

## Loader terminology
The name **xnuloader** is a project concept only. It is not an implemented EFI application, verified bootloader, or supported boot menu.

A conventional PC UEFI application or disk image cannot be assumed to boot on an Android phone. Mobile devices may use a vendor-specific chain with platform-specific image formats and verification rules.

## Safety boundary
Do not flash experimental boot images or modify boot partitions based on this document. A future device-side plan requires authoritative device-specific research, a validated stock recovery method, and independent review.
