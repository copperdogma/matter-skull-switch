# ESP32 Matter Occupancy Sensor - Development Environment Setup

## 0. Cleanup Previous Installations (Optional)

If you have attempted this setup before or have existing ESP-IDF/ESP-Matter installations in the `~/esp` directory that you want to remove, run the following command **before** proceeding. **Warning:** This will permanently delete the entire `~/esp` directory and its contents.

```bash
rm -rf ~/esp
```

## Primary Reference Documents:
- **ESP-Matter Programming Guide (Developing with the SDK):** [https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html](https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html)
- **ESP-IDF Get Started (General, check prerequisites for your OS):** [https://docs.espressif.com/projects/esp-idf/en/v5.4.1/esp32s3/get-started/index.html](https://docs.espressif.com/projects/esp-idf/en/v5.4.1/esp32s3/get-started/index.html)

## 1. Prerequisites

Before you begin, ensure your development host (Linux, macOS, or Windows with WSL) meets the prerequisites for ESP-IDF. This typically includes:
- Python 3.8 or newer
- Git
- CMake & Ninja
- Relevant compilers for your OS

Refer to the "System Requirements" and "Step 1. Install Prerequisites" sections of the [ESP-IDF Get Started Guide](https://docs.espressif.com/projects/esp-idf/en/v5.4.1/esp32s3/get-started/index.html) for detailed, OS-specific instructions.

## 2. ESP-IDF Setup

It's recommended to create a dedicated directory for ESP-IDF and related SDKs (e.g., `~/esp`).

1.  **Clone ESP-IDF Repository:**
    ```bash
    mkdir -p ~/esp
    cd ~/esp
    git clone -b v5.4.1 --recursive https://github.com/espressif/esp-idf.git
    ```
    *Note: This takes a long time due to recursive clone of all submodules.*

2.  **Install ESP-IDF Tools:**
    ```bash
    cd ~/esp/esp-idf
    ./install.sh esp32s3
    ```
    (Or run `./install.sh all` to install for all supported chips).

3.  **Source ESP-IDF Environment Script (for current terminal):**
    ```bash
    source ~/esp/esp-idf/export.sh
    ```
    *Note: You'll need to run this command in every new terminal session where you intend to use ESP-IDF.*

## 3. ESP-Matter Setup

**Version Compatibility Note:** The official ESP-Matter documentation for ESP-IDF `v5.4.1` ([link](https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html#esp-idf-setup)) does not specify checking out a particular tag of `esp-matter`. This implies the default (`main`) branch of `esp-matter` is intended to be compatible. This setup proceeds using the `main` branch.

1.  **Clone ESP-Matter Repository:**
    Make sure you are in your chosen SDKs directory (e.g., `~/esp`).
    ```bash
    cd ~/esp
    git clone --recursive https://github.com/espressif/esp-matter.git
    ```
    *Note: This is a large repository with many submodules. The clone will take significant time.*

    **If submodule cloning fails** (e.g., with 502/503 errors), run the following command to force update all submodules:
    ```bash
    cd ~/esp/esp-matter
    git submodule update --init --recursive --force
    ```

2.  **Install ESP-Matter SDK and Host Tools:**
    Ensure ESP-IDF environment is sourced (`source ~/esp/esp-idf/export.sh`).
    ```bash
    cd ~/esp/esp-matter
    ./install.sh
    ```
    *Note: This script installs dependencies for ESP-Matter and its submodules, including Pigweed.*

    **If you encounter build errors** related to missing files (e.g., `json_reader.cpp`), run:
    ```bash
    git submodule update --init --recursive --force
    ```
    And then re-run `./install.sh`.

3.  **Source ESP-Matter Environment Script (for current terminal):**
    This should be done *after* ESP-IDF's `export.sh`.
    ```bash
    source ~/esp/esp-matter/export.sh
    ```
    *Note: This script should be sourced after ESP-IDF's `export.sh`. You'll need to do this in every new terminal session.*

## 4. Activate Build Environment

For each new terminal session, you need to set up the environment. The order is critical:

1.  **Source Core Environment Scripts:**
    ```bash
    source ~/esp/esp-idf/export.sh
    source ~/esp/esp-matter/export.sh
    export IDF_CCACHE_ENABLE=1 # Optional: Speeds up rebuilds. Install ccache if not present.
    ```

2.  **Verify Environment:**
    Verify that the following commands work:
    ```bash
    idf.py --version
    chip-cert --version
    which python3  # Should point to ESP-IDF Python env (~/.espressif/python_env/...)
    ```

    **If you see Python package errors** (like `esp-idf-nvs-partition-gen` version mismatch), re-run the ESP-IDF installation:
    ```bash
    cd ~/esp/esp-idf
    ./install.sh esp32s3
    ```

## 5. Project Setup and Configuration

We'll first build a simple example to validate the environment setup, then proceed to the occupancy sensor.

### 5.1 First: Validate Environment with Simple Example

1.  **Copy the Light Example Project:**
    ```bash
    cd /path/to/your/projects_directory # e.g., cd ~/Documents/Projects
    # Create a test directory to validate setup
    mkdir -p matter-occupancy-sensor/firmware-test
    cp -r ~/esp/esp-matter/examples/light/* matter-occupancy-sensor/firmware-test/
    ```

2.  **Build the Example:**
    ```bash
    cd matter-occupancy-sensor/firmware-test
    idf.py set-target esp32s3
    idf.py build
    ```

3.  **Verify Success:**
    If the build completes without errors, your environment is set up correctly. You can now proceed to the occupancy sensor setup.

### 5.2 Create the Occupancy Sensor Project

1.  **Copy the Sensors Example Project:**
    ```bash
    cd /path/to/your/projects_directory # e.g., cd ~/Documents/Projects
    # Create the main firmware directory 
    mkdir -p matter-occupancy-sensor/firmware
    cp -r ~/esp/esp-matter/examples/sensors/* matter-occupancy-sensor/firmware/
    ```

2.  **Navigate to Firmware Directory:**
    ```bash
    cd matter-occupancy-sensor/firmware
    ```

3.  **Set Target Device:**
    ```bash
    idf.py set-target esp32s3
    ```

4.  **Run Menuconfig to Customize:**
    ```bash
    idf.py menuconfig
    ```
    Apply the following settings:
    -   Under `Component config → CHIP Device Layer → Device Identification Options`:
        -   `Device Vendor Id`: `0xFFF1` (Test Vendor ID)
        -   `Device Product Id`: `0x8000` (Test Product ID)
        -   `Default Device Hardware Version`: `1`
        -   `Device Software Version Number`: `1`
        -   `Default Device type`: `22` (This is 0x0016, the Matter ID for a "Root Node". Endpoint 0 must be a Root Node to describe the device and its functionalities; specific device types like Occupancy Sensor are assigned to other endpoints, e.g., Endpoint 1 for the actual sensor.)
    -   Under `Component config → ESP Matter`:
        -   `Device Info Provider options`: `Device Info - Custom` (Allows setting/overriding device info via code or factory NVS)
        -   **Keep "Use ESP-Matter data model" CHECKED** (Essential for initial build)
        -   `Enable Matter Server`: Checked (Essential for Matter functionality)
        -   `Initialize Thread stack...`: Unchecked (This project uses Wi-Fi, not Thread)

    *Save your changes and exit menuconfig.*

5.  **Build the Project:**
    ```bash
    idf.py build
    ```

6.  **Customize the Data Model (Optional, Advanced):**
    Once the initial build is successful, you can customize the data model by:
    1. Create a custom data model in your application code
    2. Run `menuconfig` again and uncheck "Use ESP-Matter data model"
    3. Modify the application code to initialize your custom data model
    4. Rebuild with `idf.py build`

## 6. Generate Matter Development Certificates

**IMPORTANT ENVIRONMENT NOTE:**
These commands rely on tools (`chip-cert`, `esp-matter-mfg-tool`) and environment variables (`ESP_MATTER_PATH`) that are set up by sourcing the ESP-IDF and ESP-Matter `export.sh` scripts (see Section 4: Activate Build Environment).

**Before running these commands, ensure you have sourced both scripts in your *current active terminal session*:**
```bash
source ~/esp/esp-idf/export.sh
source ~/esp/esp-matter/export.sh
```

**Certificate Generation Steps:**

Create a directory to store these certificates, e.g., `matter-occupancy-sensor/credentials/dev-certs`. Navigate into it before running the commands.

```bash
mkdir -p matter-occupancy-sensor/credentials/dev-certs
cd matter-occupancy-sensor/credentials/dev-certs
```

1.  **Generate Product Attestation Authority (PAA) Certificate & Key:**
    ```bash
    chip-cert gen-att-cert --type a -O PAA_key.pem --out PAA_cert.pem \
      --subject-cn "Matter Test PAA" \
      --valid-from "2023-01-01 00:00:00" --lifetime 7305
    ```

2.  **Generate Product Attestation Intermediate (PAI) Certificate & Key:**
    *(Uses PAA to sign)*
    ```bash
    chip-cert gen-att-cert --type i -O PAI_key.pem --out PAI_cert.pem \
      --subject-cn "Matter Test PAI FFF1" --subject-vid 0xFFF1 \
      --valid-from "2023-01-01 00:00:00" --lifetime 7305 \
      --ca-key PAA_key.pem --ca-cert PAA_cert.pem
    ```

3.  **Generate Device Attestation Certificate (DAC) & Key:**
    *(Uses PAI to sign)*
    ```bash
    chip-cert gen-att-cert --type d -O DAC_key.pem --out DAC_cert.pem \
      --subject-cn "Matter Test DAC FFF18000" --subject-vid 0xFFF1 --subject-pid 0x8000 \
      --valid-from "2023-01-01 00:00:00" --lifetime 7305 \
      --ca-key PAI_key.pem --ca-cert PAI_cert.pem
    ```

4.  **Generate Certification Declaration (CD):**
    *(Requires Test CD Signing Cert/Key from SDK - paths might need adjustment)*
    ```bash
    # chip-cert gen-cd --key $ESP_MATTER_PATH/connectedhomeip/connectedhomeip/credentials/test/certification-declaration/Chip-Test-CD-Signing-Key.pem \
    #   --cert $ESP_MATTER_PATH/connectedhomeip/connectedhomeip/credentials/test/certification-declaration/Chip-Test-CD-Signing-Cert.pem \
    #   --out CD.der \
    #   --format-version 1 --vendor-id 0xFFF1 --product-id 0x8000 \
    #   --device-type-id 263 --certificate-id "ESP0000000000000001" \
    #   --security-level 0 --security-info 0 --version-number 1 --certification-type 0
    ```
    *Note: The `device-type-id` (263 for Occupancy Sensor) and VID/PID should match your device.*

5.  **Generate Factory NVS Partition Binary using `esp-matter-mfg-tool`:**
    This process involves two main commands executed in the `matter-occupancy-sensor/credentials/dev-certs` directory.
    Ensure all `.pem` and `.der` files (PAA_cert.pem, PAI_cert.pem, DAC_key.pem, DAC_cert.pem, CD.der) are present in this directory.

    **5.1. Ensure `esp-matter-mfg-tool` is installed in the ESP-IDF Python environment:**
    It's crucial to use the Python interpreter from your ESP-IDF environment. The path below is an example; verify it for your system if needed (e.g., by running `source ~/esp/esp-idf/export.sh && source ~/esp/esp-matter/export.sh && which python3` in a new terminal).
    ```bash
    /Users/cam/.espressif/python_env/idf5.4_py3.13_env/bin/python3 -m pip install esp-matter-mfg-tool
    ```

    **5.2. Run `esp-matter-mfg-tool` to generate the partition binary:**
    This command relies on the `esp-matter-mfg-tool` script being in the PATH, which should be handled by sourcing the ESP-IDF and ESP-Matter `export.sh` scripts.
    ```bash
    esp-matter-mfg-tool \
      -v 0xFFF1 -p 0x8000 \
      --vendor-name "MyTestVendor" --product-name "OccupancySensor" \
      --hw-ver 1 --hw-ver-str "1.0.0" \
      --mfg-date "2024-05-21" --serial-num "OCCSENSOR001" \
      --dac-key DAC_key.pem --dac-cert DAC_cert.pem \
      --pai -c PAI_cert.pem \
      -cd CD.der \
      --discriminator 3840 --passcode 20202021 \
      --out factory_nvs_output
    ```
    *This creates a directory named `factory_nvs_output` containing the results.*

6.  **Locate and Copy the NVS Binary:**
    The actual NVS binary is nested within the `factory_nvs_output` directory. The exact path will include a VID_PID folder and a UUID-like folder name generated by the tool.
    For the command above with VID `0xFFF1` and PID `0x8000`, the path to the binary would be inside a directory like `factory_nvs_output/fff1_8000/xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx/` and the file would be named `xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx-partition.bin` where `xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx` is the generated UUID.

    Copy this binary to `mfg_nvs.bin` in the `credentials/dev-certs` directory:
    ```bash
    # Adjust the source path with the actual VID_PID and UUID generated by the tool.
    # Example, if UUID was d6e078de-1a28-4b57-a713-be172effac1f:
    cp factory_nvs_output/fff1_8000/d6e078de-1a28-4b57-a713-be172effac1f/d6e078de-1a28-4b57-a713-be172effac1f-partition.bin mfg_nvs.bin
    ```

## 7. Flashing Firmware and NVS Partition

1.  **Flash the Factory NVS Partition:**
    Navigate to your project's `firmware` directory.
    ```bash
    cd ../../firmware # Assuming you were in credentials/dev-certs
    # Or cd /path/to/your/project/firmware
    ```
    Flash the `mfg_nvs.bin` to the NVS partition offset. For this project, the offset is `0x10000` (defined in `partitions.csv`).
    ```bash
    python -m esptool --chip esp32s3 -p /dev/tty.usbmodemXXX write_flash 0x10000 ../credentials/dev-certs/mfg_nvs.bin
    ```
    *(Replace `/dev/tty.usbmodemXXX` with your actual serial port.)*

2.  **Build and Flash Main Application:**
    After flashing the NVS partition, re-flash your main application firmware.
    ```bash
    idf.py build
    idf.py -p /dev/tty.usbmodemXXX flash monitor
    ```

3.  **Troubleshooting Commissioning after Flashing NVS:**
    If, after flashing the factory NVS and your application, the device doesn't advertise for commissioning or fails to commission:
    *   Perform a **factory reset** on the ESP32: Power cycle the device while holding the BOOT/GPIO0 button for about 10 seconds. This clears the main NVS partition (where fabric info is stored) but leaves the factory NVS partition intact.
    *   **Remove any existing instance** of this accessory from your Matter controller app (e.g., Apple Home).
    *   Attempt commissioning again. The device should now use the credentials from the factory NVS partition.

## Troubleshooting Tips

1.  **"gn command not found" during build:**
    - Ensure the ESP-Matter environment is properly sourced.
    - Check `echo $ESP_MATTER_PATH` points to your `esp-matter` directory.
    - Verify `ls $ESP_MATTER_PATH/connectedhomeip/connectedhomeip/.environment/cipd/packages/pigweed/gn` shows the `gn` executable.
    - Never re-run ESP-IDF's `install.sh` after sourcing ESP-Matter environment.

2.  **Python Environment Issues:**
    - Always use the Python interpreter provided by ESP-IDF environment.
    - Check `which python3` to ensure it points to `~/.espressif/python_env/...`.
    - If you see Python package version mismatches, try re-running ESP-IDF's `install.sh`.
    - Never mix `python` and `python3` when running multiple commands.

3.  **Submodule Cloning Issues:**
    - If Git submodule operations fail with network errors (e.g., 502/503), try using the `--force` flag:
      ```bash
      git submodule update --init --recursive --force
      ```
    - If specific files are missing, check if the corresponding submodule was properly initialized.

4.  **Build Errors in the C++ Code:**
    - If you see errors about missing symbols or undefined references, they often indicate environment or configuration issues.
    - Try a full clean: `idf.py fullclean` before rebuilding.
    - Verify that all required ESP-Matter components are correctly included in your project's `CMakeLists.txt`.
