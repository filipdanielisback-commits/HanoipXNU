# Audio Core Scaffold

This directory contains portable, hardware-independent C components:

- `pcm_ring.h/.c`: bounded byte-ring operations over caller-owned storage.
- `stream.h/.c`: stream lifecycle state machine for open, start, pause, resume, stop, close, and error transitions.
- `buffer_queue.h/.c`: bounded descriptor queue with explicit queued, in-flight, completed, and error states.
- `engine.h/.c`: coordinator joining stream lifecycle rules to buffer submission, acquisition, completion, and release.

They perform no hardware I/O, memory allocation, DMA, codec control, DSP communication, interrupt handling, locking, or XNU API calls.

**These are not XNU audio drivers.** They are host-testable building blocks whose eventual integration into an XNU target remains to be designed and reviewed.

## Buffer ownership and completion

The queue metadata array and audio data buffers are caller-owned. A successful submit transfers the buffer to the queue contract: callers must not modify, free, or reuse that buffer until its completion has been released.

Each submission receives a 64-bit ticket. The ticket identifies one queue slot generation, so a late completion for an already released submission is rejected as stale. The queue supports bounded submission, FIFO acquisition, explicit completion with transferred-byte count and status, querying completion details, and release for slot reuse.

The caller must serialize all queue operations. This first version permits one or more buffers to be marked in flight, but it does not schedule hardware, start DMA, perform cache maintenance, or guarantee playback ordering at a device. A future backend must define those details against verified hardware and XNU APIs.

A completion status of zero marks a buffer COMPLETE; a nonzero status marks it ERROR. Completion byte counts larger than the submitted length are rejected. Completed/error descriptors remain occupied until explicitly released. Callers should query completion details before releasing a descriptor. On engine failure, buffers that were queued but not yet acquired are converted to ERROR using the failure code, so they can be inspected and released. Buffers already in flight still require an explicit completion report before shutdown.

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

The lifecycle object is bookkeeping only: a future hardware adapter must perform actual device operations and update lifecycle state only when those operations have succeeded.

## Host tests

From the repository root:

```sh
make -C tests/audio test
```

The tests cover ring-buffer behavior, stream state transitions, queue capacity, FIFO acquisition, completion result reporting, completion validation, and stale-ticket rejection. Passing them does not validate XNU integration or Moto G60 hardware.
