# Audio Core Scaffold

This directory currently contains a small portable PCM byte-ring utility:

- `pcm_ring.h`
- `pcm_ring.c`

It provides bounded read/write operations over caller-owned storage and does not allocate memory.

**This is not an XNU audio driver.** It has no IOKit/AudioFamily integration, DMA support, interrupt handling, codec control, DSP communication, hardware discovery, or Moto G60-specific code. It is a host-testable building block whose eventual use in an XNU driver remains to be designed and reviewed.

## Concurrency and ownership

The ring is not internally synchronized. Callers must serialize every operation with an appropriate lock or otherwise guarantee single-threaded access. The source/destination buffers passed to read/write must not overlap the ring's backing storage. The caller owns the backing storage and must keep it valid for the ring's lifetime.

The utility copies bytes only; it does not interpret PCM formats, sample rates, channels, timestamps, or audio frames.

## Host tests

From the repository root:

```sh
make -C tests/audio test
```

The host test validates initialization, wraparound, bounded capacity, read/write ordering, and reset. Passing this test does not validate an XNU build or target-device audio.
