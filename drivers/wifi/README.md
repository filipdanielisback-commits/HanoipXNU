# Wi-Fi portable core

**Status:** Hardware-independent lifecycle state machine and bounded packet-queue primitives. This is not a Wi-Fi driver.

## Files

- `state.h` — state and transition API.
- `state.c` — allocation-free, I/O-free lifecycle implementation.

The caller must serialize access. This module does not perform device discovery, firmware loading, scanning, authentication, packet I/O, DMA, interrupts, power control, or XNU integration. Its transition functions should be called only when the backend has supplied the corresponding confirmation.

The state machine tracks device discovery and initialization, ready/scanning/connecting/link-up/disconnecting, suspend/resume, failure, stop, and detach. Scan and connection requests use nonzero caller-supplied request identifiers, and asynchronous completions must carry the device generation token to reject stale events from an earlier attachment. Initialization, failure, scan/connect/disconnect, and suspend/resume completions are generation-checked. Request IDs must not be reused within a device generation. A successful connect completion represents a confirmed backend result; the state machine does not create connectivity itself.

## Host tests

Run:

```sh
make -C tests/wifi test
```

The tests use a deterministic fake backend and validate state transitions and error paths. They do not emulate a Qualcomm chipset, firmware, a bus, the radio environment, an XNU network stack, or Moto G60 hardware.

Compile defaults to strict C11 warnings-as-errors. No external libraries are required.


## Bounded packet queues

- `packet_queue.h` / `packet_queue.c` provide separate caller-initialized TX or RX queues with fixed descriptor capacity.
- Submit records a caller-owned buffer, nonzero device-generation token, length/capacity, and a unique queue-local ticket. TX length is the packet byte count; RX length is writable buffer capacity.
- Acquire hands the oldest queued descriptor to the backend and marks it in flight. Completion must match both ticket and device generation, may occur exactly once, and cannot report more bytes than the submitted length.
- Completion details remain queryable until release. Only completed or failed descriptors can be released. Queued descriptors can be marked failed during cancellation; in-flight work is deliberately not declared stopped by this operation.
- The queue never allocates, copies, frees, maps, or otherwise owns the buffer memory. The caller must keep the buffer valid and unmodified as required by the backend contract until release. Access to each queue must be serialized by its caller.
- Queue metadata and descriptor storage are caller-owned. Tickets are monotonic within one queue initialization; initialize a queue only when prior callbacks are quiesced, and use a new device generation after detach/reattach.

Run the packet-queue test together with the lifecycle and fake-backend tests:

```sh
make -C tests/wifi test
```

These are portable bookkeeping primitives only. They do not transmit or receive packets, define a network packet format, implement DMA or a transport, or establish XNU/Moto G60 compatibility.
