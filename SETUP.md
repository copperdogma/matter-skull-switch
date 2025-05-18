# ESP32 Matter Occupancy Sensor - Development Environment Setup

## Quick Reference

```bash
# Initial setup (one-time)
mkdir -p ~/esp
cd ~/esp
git clone -b v5.4.1 --recursive https://github.com/espressif/esp-idf.git
cd ~/esp/esp-idf && ./install.sh esp32s3
git clone --recursive https://github.com/espressif/esp-matter.git
cd ~/esp/esp-matter && ./install.sh

# For every new terminal session
source ~/esp/esp-idf/export.sh
source ~/esp/esp-matter/export.sh

# Project commands
idf.py set-target esp32s3  # Set ESP32-S3 as target
idf.py menuconfig          # Configure project
idf.py build               # Build project
idf.py -p PORT flash       # Flash firmware
idf.py -p PORT monitor     # Monitor serial output
```

## Primary Reference Documents
- [ESP-Matter Programming Guide](https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html)
- [ESP-IDF Get Started](https://docs.espressif.com/projects/esp-idf/en/v5.4.1/esp32s3/get-started/index.html)

## 1. Prerequisites

Before you begin, ensure your development host (Linux, macOS, or Windows with WSL) meets the prerequisites for ESP-IDF:
- Python 3.8 or newer
- Git
- CMake & Ninja
- Relevant compilers for your OS

Refer to the [ESP-IDF Get Started Guide](https://docs.espressif.com/projects/esp-idf/en/v5.4.1/esp32s3/get-started/index.html) for detailed, OS-specific instructions.

## 2. Initial Setup

### 2.1 Optional: Clean Previous Installations

If you have existing ESP-IDF/ESP-Matter installations in the `~/esp` directory that you want to remove:

```bash
rm -rf ~/esp
```

### 2.2 ESP-IDF Setup

```bash
mkdir -p ~/esp
cd ~/esp
git clone -b v5.4.1 --recursive https://github.com/espressif/esp-idf.git
cd ~/esp/esp-idf
./install.sh esp32s3
```

### 2.3 ESP-Matter Setup

```bash
cd ~/esp
git clone --recursive https://github.com/espressif/esp-matter.git
cd ~/esp/esp-matter
./install.sh
```

If submodule cloning fails with 502/503 errors:
```bash
git submodule update --init --recursive --force
```

## 3. Environment Management

For each new terminal session, source both environment scripts in this order:

```bash
source ~/esp/esp-idf/export.sh
source ~/esp/esp-matter/export.sh
```

To verify your environment is correctly set up:
```bash
idf.py --version
chip-cert --version
which python3  # Should point to ESP-IDF Python env (~/.espressif/python_env/...)
```

## 4. Project Setup and Configuration

### 4.1 Validate Environment with Example

```bash
cd /path/to/your/projects_directory
mkdir -p matter-occupancy-sensor/firmware-test
cp -r ~/esp/esp-matter/examples/light/* matter-occupancy-sensor/firmware-test/
cd matter-occupancy-sensor/firmware-test
idf.py set-target esp32s3
idf.py build
```

### 4.2 Create the Occupancy Sensor Project

```bash
cd /path/to/your/projects_directory
mkdir -p matter-occupancy-sensor/firmware
cp -r ~/esp/esp-matter/examples/sensors/* matter-occupancy-sensor/firmware/
cd matter-occupancy-sensor/firmware
idf.py set-target esp32s3
```

### 4.3 Configure the Project

Run menuconfig:
```bash
idf.py menuconfig
```

Apply these key settings:

| Section | Setting | Value |
|---------|---------|-------|
| **Component config → CHIP Device Layer → Device Identification Options** | | |
| | Device Vendor Id | 0xFFF1 |
| | Device Product Id | 0x8000 |
| | Default Device Hardware Version | 1 |
| | Device Software Version Number | 1 |
| | Default Device type | 22 (Root Node) |
| **Component config → ESP Matter** | | |
| | Device Info Provider options | Device Info - Custom |
| | Use ESP-Matter data model | ✓ (Checked) |
| | Enable Matter Server | ✓ (Checked) |
| | Initialize Thread stack | ✗ (Unchecked) |

Build the project:
```bash
idf.py build
```

## 5. Matter Development Certificates

Create a directory for certificates:
```bash
mkdir -p matter-occupancy-sensor/credentials/dev-certs
cd matter-occupancy-sensor/credentials/dev-certs
```

### 5.1 Generate Certificate Chain

```bash
# Product Attestation Authority (PAA)
chip-cert gen-att-cert --type a -O PAA_key.pem --out PAA_cert.pem \
  --subject-cn "Matter Test PAA" \
  --valid-from "2023-01-01 00:00:00" --lifetime 7305

# Product Attestation Intermediate (PAI)
chip-cert gen-att-cert --type i -O PAI_key.pem --out PAI_cert.pem \
  --subject-cn "Matter Test PAI FFF1" --subject-vid 0xFFF1 \
  --valid-from "2023-01-01 00:00:00" --lifetime 7305 \
  --ca-key PAA_key.pem --ca-cert PAA_cert.pem

# Device Attestation Certificate (DAC)
chip-cert gen-att-cert --type d -O DAC_key.pem --out DAC_cert.pem \
  --subject-cn "Matter Test DAC FFF18000" --subject-vid 0xFFF1 --subject-pid 0x8000 \
  --valid-from "2023-01-01 00:00:00" --lifetime 7305 \
  --ca-key PAI_key.pem --ca-cert PAI_cert.pem
```

### 5.2 Generate Factory NVS Partition

Ensure the esp-matter-mfg-tool is installed:
```bash
python3 -m pip install esp-matter-mfg-tool
```

Generate the partition binary:
```bash
esp-matter-mfg-tool \
  -v 0xFFF1 -p 0x8000 \
  --vendor-name "MyTestVendor" --product-name "OccupancySensor" \
  --hw-ver 1 --hw-ver-str "1.0.0" \
  --mfg-date "2024-05-21" --serial-num "OCCSENSOR001" \
  --dac-key DAC_key.pem --dac-cert DAC_cert.pem \
  --pai -c PAI_cert.pem \
  --discriminator 3840 --passcode 20202021 \
  --out factory_nvs_output
```

Copy the NVS binary:
```bash
# Find the generated UUID, e.g., d6e078de-1a28-4b57-a713-be172effac1f
cp factory_nvs_output/fff1_8000/{UUID}/{UUID}-partition.bin mfg_nvs.bin
```

## 6. Flashing Firmware and NVS Partition

1. **Flash the Factory NVS Partition:**
   ```bash
   cd /path/to/your/project/firmware
   python -m esptool --chip esp32s3 -p /dev/tty.usbmodemXXX write_flash 0x10000 ../credentials/dev-certs/mfg_nvs.bin
   ```

2. **Build and Flash Main Application:**
   ```bash
   idf.py build
   idf.py -p /dev/tty.usbmodemXXX flash monitor
   ```

3. **Commissioning Information:**
   After flashing, check the serial monitor for:
   - QR code payload (e.g., `MT:Y.K90GSY00KA0648G00`) 
   - Manual pairing code (e.g., `34970112332`)

## 7. Troubleshooting

| Issue | Solution |
|-------|----------|
| **"gn command not found"** | Verify environment: `echo $ESP_MATTER_PATH` <br>Check for executable: `ls $ESP_MATTER_PATH/connectedhomeip/connectedhomeip/.environment/cipd/packages/pigweed/gn` |
| **Python environment issues** | Ensure using ESP-IDF Python: `which python3` <br>Re-run ESP-IDF's `install.sh` if needed |
| **Submodule cloning failures** | Use force flag: `git submodule update --init --recursive --force` |
| **Build errors in C++ code** | Try full clean: `idf.py fullclean` before rebuilding |
| **Commissioning failures** | Factory reset: Hold BOOT/GPIO0 button for ~10s during power cycle <br>Remove device from Matter controller app <br>Check serial monitor for current QR code and pairing code |
