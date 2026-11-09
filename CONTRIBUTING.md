# Contributing to HanoipXNU

Thank you for your interest in the project.

## Current contribution priorities
- Device research backed by reliable sources
- Architecture and boot-process documentation
- Build reproducibility and emulator tests
- Clear descriptions of limitations and failed experiments
- License and provenance review for proposed code

## Before opening a pull request
1. Explain the problem or research question.
2. Link sources for device-specific claims.
3. Mark statements as **verified**, **inferred**, or **unknown**.
4. Include exact tool versions and test environment for reproducible results.
5. Do not describe untested code as working.

## Code and third-party material
Do not submit proprietary Apple software, leaked SDKs, firmware images, signing keys, credentials, or code whose license does not permit redistribution. Preserve upstream copyright and license notices. Do not assume that a third-party component can be relicensed under the project's eventual license.

## Device testing
Prefer emulators and non-destructive analysis. Do not ask contributors to flash unverified images or disable device protections. Any future device-side test proposal must include risks, a verified recovery plan, and independent review.

## Pull request checklist
- [ ] Scope and motivation are clear.
- [ ] Sources and provenance are included.
- [ ] Tests or reasons tests could not be run are documented.
- [ ] No secrets, personal data, or restricted proprietary files are included.
- [ ] Documentation accurately describes the implementation status.
