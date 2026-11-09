# Test Plans and Results

Record tests with:
- Research question and expected result
- Exact source revision and tool versions
- Host or emulator configuration
- Reproducible steps
- Logs and observed result
- Limitations and follow-up work

## Current host tests

The audio utilities have host-side tests:

```sh
make -C tests/audio test
```

These validate basic initialization, bounded writes, read/write ordering, wraparound, and reset. They do not validate XNU integration, audio formats, DMA, codec control, or Moto G60 hardware.

The hardware-independent Wi-Fi lifecycle core has a separate test suite:

```sh
make -C tests/wifi test
```

These tests exercise lifecycle transitions, request matching, generation-token rejection of stale asynchronous completions, suspend/resume outcomes, failure handling, and a deterministic fake backend. They do not emulate a chipset, firmware, transport, radio, XNU networking, or Moto G60 hardware. The suite now also tests bounded TX/RX packet descriptor queues: FIFO acquisition, capacity, ownership states, generation/ticket matching, stale and duplicate completions, cancellation, and release. These tests validate queue bookkeeping only; they do not transmit or receive network packets. Record actual compiler output and environment when running the full suite locally.

A test on an emulator or a different device must not be presented as proof of Moto G60 support.
