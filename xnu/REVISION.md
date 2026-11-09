# Upstream XNU revision record

- Upstream: https://github.com/apple-oss-distributions/xnu
- Selected tag: **not selected**
- Immutable commit: **not selected**
- Candidate Darwin/iOS family: Darwin 24 / iOS 18 (research target only)
- Exact iOS 18 XNU source match: **not established**
- Build environment: **not established**
- Last reviewed: 2026-11-09

## Selection decision

Do not substitute a newer XNU `main` checkout or a macOS XNU tag and call it the exact iOS 18 kernel. First compare the kernel build identifiers associated with the intended iOS 18 release against Apple-published XNU tags and source commits. Record the precise match or document the delta.

The public XNU repository is useful primary source for the shared kernel architecture and ARM64 code, but its public source availability does not establish that every iOS 18 platform change, driver, private header, SDK, firmware component, or build dependency is present.

## Required before pinning

1. Identify the exact iOS 18 release/build being used as a compatibility reference.
2. Record its kernel version/build from a trustworthy, independently verifiable source.
3. Enumerate Apple-published XNU tags/commits in the matching Darwin generation.
4. Compare version metadata and relevant source changes; do not rely on tag names alone.
5. Record the selected tag and immutable commit SHA.
6. Save upstream license and notice references and audit any patch-derived files.
7. Attempt a clean reference build and record the exact host, compiler, SDK, and result.

Until these steps are complete, all HanoipXNU work must describe its kernel base as **unselected public XNU source under evaluation**.
