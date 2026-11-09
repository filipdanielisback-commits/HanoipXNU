# HanoipXNU

**Experimental XNU/Darwin research for the Motorola Moto G60 (XT2135-2, device codename `hanoip`).**

HanoipXNU is an independent research project exploring what would be required to bring an XNU/Darwin-based environment to the Moto G60. The immediate goal is to document the device, investigate its boot architecture, and build reproducible research tooling.

> [!WARNING]
> This is an early research project. There is currently no claim of a booting XNU kernel, working iOS, functioning device drivers, or a safe-to-flash image. Do not flash experimental images or alter device partitions based on this repository.

## Project status

<kbd>Pre-alpha · Documentation and feasibility</kbd>

| Area | Status |
|---|---|
| Project documentation | In progress |
| Device research | Not yet validated |
| Build environment | Planned |
| XNU integration | Not started |
| Boot-chain research | Not started |
| Display and touchscreen | Not tested |
| Wi-Fi, Bluetooth, audio | Investigating; no XNU drivers implemented |
| Battery and sensors | Not tested |
| Userland and applications | Not started |

See [ROADMAP.md](ROADMAP.md), [docs/hardware-support.md](docs/hardware-support.md), and the [Phase 2 driver architecture](docs/driver-architecture.md).

## Device target

- **Device:** Motorola Moto G60
- **Target model:** XT2135-2
- **Device codename:** `hanoip`
- **Original platform:** Android
- **Memory target:** 6 GB RAM configuration

Exact hardware revisions, firmware versions, boot configuration, and partition layout still need to be documented from reliable device-specific sources.

## What this project is — and is not

HanoipXNU is an operating-system porting and platform research effort. XNU source availability does not provide the complete iOS operating system. iOS also depends on proprietary frameworks, services, signing infrastructure, and hardware-specific components that are not supplied by the open-source XNU repository.

Accordingly, this project does **not** claim to be an iOS 18 port. Any future Darwin-compatible experiment must be described accurately and must respect the licenses and distribution restrictions of every component. The public Apple XNU source is now being evaluated as the real kernel baseline; no exact iOS 18 source match or reproducible XNU build has been established yet. See [XNU revision record](xnu/REVISION.md).

## Repository layout

```text
HanoipXNU/
├── README.md
├── ROADMAP.md
├── CONTRIBUTING.md
├── SECURITY.md
├── xnu/                   # Upstream XNU revision record and HanoipXNU patches
├── kernel/                # Kernel integration notes
├── platform/              # ARM64 and Qualcomm platform research
├── configs/               # Project-owned build configuration plans
├── drivers/               # Portable core and proposed XNU-facing drivers
├── bootloader/            # Boot-chain research and experiments
├── devices/motorola/hanoip/ # Device-specific research
├── build/                 # Reproducible build tooling
├── tools/                 # Source and build research tools
├── qemu/                  # Future emulator experiments
├── sysimage/              # Future local image-layout research
├── tests/                 # Test plans and results
└── userland/              # Userspace research
```

Directories are added as research becomes concrete; their presence does not imply implemented functionality.

## Getting started

Start with these documents:

1. [Project requirements](docs/requirements.md)
2. [Architecture overview](docs/architecture.md)
3. [Boot-process research](docs/boot-process.md)
4. [Hardware support matrix](docs/hardware-support.md)
5. [Phase 2 driver architecture](docs/driver-architecture.md)
6. [Driver interface proposals](drivers/README.md)
7. [Legal and licensing notes](docs/legal.md)
8. [Roadmap](ROADMAP.md)

At this stage, work should be limited to source review, documentation, and safe development-environment setup. Use an emulator for early experiments wherever practical. Keep the phone on its known-working stock software until a tested recovery plan and a validated device-specific procedure exist.

## Contributing

Research notes, source references, build-system improvements, and carefully documented test results are welcome. Read [CONTRIBUTING.md](CONTRIBUTING.md) before opening a pull request. Please distinguish verified facts from hypotheses.

## Security

Please see [SECURITY.md](SECURITY.md) for responsible reporting. Never publish personal data, device identifiers, account credentials, signing keys, or proprietary firmware.

## License

The project license has not yet been selected in this repository. Third-party components retain their own licenses. See [docs/legal.md](docs/legal.md) before adding or redistributing external code.
