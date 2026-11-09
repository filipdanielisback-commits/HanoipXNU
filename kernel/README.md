# Kernel Research

This directory is reserved for XNU source references, integration notes, and project-authored patches.

No target-device kernel port, XNU driver adapter, or patch set is currently implemented. The repository has not yet pinned an upstream XNU revision or reproducible kernel build environment.

Before importing or adapting upstream code:
- Select and record the exact upstream XNU repository and commit.
- Verify the applicable source license and preserve copyright notices.
- Document the target architecture, compiler/toolchain, SDK, and build environment.
- Identify the intended driver integration model and required kernel interfaces.
- Keep project-specific changes separate, reviewable, and tested.
- Do not treat macOS driver APIs as automatically available in a standalone or embedded XNU environment.

Portable host-side utilities may be developed and tested independently, but they are not XNU kernel drivers until explicitly integrated and built against the selected XNU target.


The upstream integration layout now lives in [`../xnu/`](../xnu/README.md), with a revision-selection record in [`../xnu/REVISION.md`](../xnu/REVISION.md). Keep generic kernel research here and keep upstream provenance plus project-authored XNU patches separate.
