# Test Plans and Results

Record tests with:
- Research question and expected result
- Exact source revision and tool versions
- Host or emulator configuration
- Reproducible steps
- Logs and observed result
- Limitations and follow-up work

## Current host tests

The initial audio byte-ring utility has host-side tests:

```sh
make -C tests/audio test
```

These validate basic initialization, bounded writes, read/write ordering, wraparound, and reset. They do not validate XNU integration, audio formats, DMA, codec control, or Moto G60 hardware.

A test on an emulator or a different device must not be presented as proof of Moto G60 support.
