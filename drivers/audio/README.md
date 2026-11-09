# Audio Core Scaffold

This directory contains two portable, hardware-independent C components:

- `pcm_ring.h/.c`: bounded byte-ring operations over caller-owned storage.
- `stream.h/.c`: a stream lifecycle state machine for open, start, pause, resume, stop, close, and error transitions.

They perform no hardware I/O, memory allocation, DMA, codec control, DSP communication, interrupt handling, or XNU API calls.

**These are not XNU audio drivers.** They are host-testable building blocks whose eventual integration into an XNU target remains to be designed and reviewed.

## Stream lifecycle

```text
CLOSED --open--> READY --start--> RUNNING
                   ^                 |
                   |                 | pause
                   |                 v
                   +--- stop ---- PAUSED
                                      |
                                      | resume
                                      +----> RUNNING

READY / RUNNING / PAUSED --fail(error)--> FAILED
FAILED --stop--> READY
READY / FAILED --close--> CLOSED
```

Invalid transitions return an explicit error and leave the state unchanged. A failed stream retains its nonzero error code until stopped or closed.

The lifecycle object is deliberately not internally synchronized. Callers must serialize operations on each instance. The current state machine is bookkeeping only: a future hardware adapter must perform actual device operations and update lifecycle state only when those operations have succeeded.

## Host tests

From the repository root:

```sh
make -C tests/audio test
```

The tests cover ring-buffer behavior and stream state transitions. Passing them does not validate XNU integration or Moto G60 hardware.
