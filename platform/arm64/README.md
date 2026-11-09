# ARM64 platform layer

This directory is for HanoipXNU-owned ARM64 adaptation notes and code. It must remain separate from XNU's upstream ARM64 implementation.

Before writing platform code, document the expected exception level, MMU state, cache coherency, interrupt controller, timer, SMP startup, memory map, and boot handoff. The Moto G60's actual boot handoff and firmware state are not yet verified.

No ARM64 XNU platform implementation is present here yet.
