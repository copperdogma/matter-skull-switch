# Project Setup Phase Scratchpad

## Current Phase
- Initial Project Setup & Onboarding

## Next Steps
1.  **Device Commissioning:** Add the ESP32 to the HomeKit network.
    *   User to attempt commissioning with codes found in `docs/CHANGELOG.md`:
        *   QR Code Text: `MT:Y.K90MBW17GU0Y7RD00`
        *   Manual Code: `3321-531-0437`
    *   If unsuccessful, investigate if these codes are current or if regeneration is needed.

## Completed Tasks
-   Project initialized.
-   Firmware flashed to ESP32.
-   Device visible in Apple Home app (but not added).

## Issues or Blockers
-   None currently.

## Decisions Made
-   Proceeding with HomeKit commissioning.

## Action Items
-   User to attempt HomeKit pairing with found codes.

## Recently Completed Actions
-   Successfully flashed base Matter firmware to ESP32.
-   Confirmed device is discoverable by Apple Home.

## Notes on Integrating ESP-Matter Example Utilities (e.g., app_reset)

When incorporating utility code (like `app_reset.h` and `app_reset.cpp`) from the `esp-matter` SDK's example directories into the project's `main` component, the following steps were found to be crucial for successful compilation and linking:

1.  **Locate SDK Files:**
    *   Identify the necessary header (`.h`) and source (`.cpp` or `.c`) files within the `esp-matter` SDK (e.g., `~/esp/esp-matter/examples/common/app_reset/app_reset.h` and `app_reset.cpp`).
2.  **Copy to Project:**
    *   Copy these files directly into the project's `main` component directory (e.g., `firmware/main/`).
3.  **Update `main/CMakeLists.txt`:**
    *   Ensure `idf_component_register` includes `"."` in `SRC_DIRS` so that source files directly within the `main` directory are compiled.
    *   Ensure `"."` (or the relevant local subdirectory if you organize them differently) is in `INCLUDE_DIRS` and/or `PRIV_INCLUDE_DIRS` so that headers can be found (e.g., `#include "app_reset.h"` from `app_main.cpp`).
4.  **Source File Includes:**
    *   In your project code (e.g., `app_main.cpp`), include the header using a path relative to the include directories (e.g., `#include "app_reset.h"` if both `app_main.cpp` and `app_reset.h` are in `firmware/main/`).
5.  **Clean Build:**
    *   If linker errors like "undefined reference" occur despite the files being present and `CMakeLists.txt` appearing correct, perform a full clean of the build:
        *   `idf.py fullclean`
    *   **Managed Component Hash Issue:** If `fullclean` fails due to modified managed components (e.g., `espressif__esp_matter`), remove the `.component_hash` file from the problematic managed component's directory (e.g., `rm firmware/managed_components/espressif__esp_matter/.component_hash`).
        *   Then, re-run `idf.py fullclean` followed by `idf.py build`.

This process ensures that both the declarations (from the header) and the definitions (from the source file) of the utility functions are properly compiled and linked into the final application. 