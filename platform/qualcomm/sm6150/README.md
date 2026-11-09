# Qualcomm platform research (provisional SM6150 label)

Some public Android device-tree material labels the hanoip platform as `sm6150`; other sources use `SM7150`. This directory is therefore a research placeholder, not a confirmed SoC implementation.

Before adding code, establish the exact SoC/platform identity for XT2135-2 and distinguish:
- SoC-family common facts;
- board-specific wiring and device-tree overlays;
- vendor firmware and proprietary components;
- Linux/Android reference behavior;
- XNU-compatible implementation work.

Do not add guessed addresses, interrupts, clocks, power domains, or device buses.
