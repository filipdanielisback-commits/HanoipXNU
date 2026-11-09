# Wi-Fi portable core

**Status:** Hardware-independent lifecycle state machine only. This is not a Wi-Fi driver.

## Files

- `state.h` — state and transition API.
- `state.c` — allocation-free, I/O-free lifecycle implementation.

The caller must serialize access. This module does not perform device discovery, firmware loading, scanning, authentication, packet I/O, DMA, interrupts, power control, or XNU integration. Its transition functions should be called only when the backend has supplied the corresponding confirmation.

The state machine tracks device discovery and initialization, ready/scanning/connecting/link-up/disconnecting, suspend/resume, failure, stop, and detach. Scan and connection requests use nonzero caller-supplied request identifiers, and asynchronous completions must carry the device generation token to reject stale events from an earlier attachment. A successful connect completion represents a confirmed backend result; the state machine does not create connectivity itself.

## Host tests

Run:

```sh
make -C tests/wifi test
```

The tests use a deterministic fake backend and validate state transitions and error paths. They do not emulate a Qualcomm chipset, firmware, a bus, the radio environment, an XNU network stack, or Moto G60 hardware.

Compile defaults to strict C11 warnings-as-errors. No external libraries are required.
