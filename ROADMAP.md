# Roadmap

This roadmap describes research milestones, not delivery promises. Milestones may change as feasibility is established.

## Pre-alpha 0.1 — Project foundation
- [x] Create repository overview and research structure
- [x] Document scope, risks, and current unknowns
- [x] Collect initial public device-tree and kernel research leads
- [ ] Record exact XT2135-2 hardware and firmware revisions
- [ ] Resolve the Qualcomm platform identifier discrepancy
- [ ] Decide project license after reviewing all included material

## Pre-alpha 0.2 — Feasibility research
- [ ] Document the device boot chain and supported boot modes
- [x] Identify preliminary family-level SoC, memory, display, and GPU references
- [ ] Verify exact SoC/platform identity for XT2135-2
- [ ] Review relevant upstream XNU source and its supported architectures
- [ ] Identify the gap between upstream XNU and this device's platform
- [ ] Define an emulator-based proof-of-concept plan

## Research prototype — Emulator first
- [ ] Establish a reproducible host build environment
- [ ] Build and test only components that are supported by the selected emulator
- [ ] Record logs, exact source revisions, and reproducible test steps

## Device feasibility — No flashing yet
- [ ] Complete a documented boot-chain compatibility assessment
- [ ] Establish a verified stock recovery procedure
- [ ] Review licensing and redistribution requirements
- [ ] Require independent review before any device-side experiment

## Later research areas
- [ ] Display and input feasibility
- [ ] Storage and filesystem integration
- [ ] Audio and sensor feasibility
- [ ] Wi-Fi and Bluetooth feasibility
- [ ] Power management and battery reporting
- [ ] Userland and application compatibility

A checkbox is completed only when there is reproducible evidence in the repository. A roadmap item is not a promise that the feature is technically possible.
