## Example Summary

This example demonstrates multi-sensor data capture for TIDA-010997 reference design using CCStudio™ Edge AI Studio. The firmware acquires data from various sensors including PIR motion sensor (via ADC), temperature & humidity (HDC3020), pressure (BMP384), ambient light (OPT4001), digital microphone (ICS43434), IMU (BMI270), and hall effect sensor (TMAG5170). Data is streamed to a host PC over the Device Agent Protocol (DAP) via UART for data acquisition, model training, verification, or analysis.

The example supports multiple sensor types:
- **PIR Motion Sensor**: Analog signal captured via ADC at 31.25 Hz sampling rate
- **Temperature & Humidity**: I2C-based HDC3020 sensor
- **Pressure**: I2C-based BMP384 barometric pressure sensor
- **Ambient Light**: I2C-based OPT4001 optical sensor
- **Digital Microphone**: I2S-based ICS43434 microphone
- **IMU**: SPI-based BMI270 6-axis accelerometer and gyroscope
- **Hall Effect Sensor**: SPI-based TMAG5170 3D linear Hall effect sensor

### Device Migration Recommendations
This project was developed for a superset device included in the MSPM0 LaunchPad. Please
visit the [CCS User's Guide](https://software-dl.ti.com/msp430/esd/MSPM0-SDK/latest/docs/english/tools/ccs_ide_guide/doc_guide/doc_guide-srcs/ccs_ide_guide.html#sysconfig-project-migration)
for information about migrating to other MSPM0 devices.

### Low-Power Recommendations
TI recommends to terminate unused pins by setting the corresponding functions to
GPIO and configure the pins to output low or input with internal
pullup/pulldown resistor.

SysConfig allows developers to easily configure unused pins by selecting **Board**→**Configure Unused Pins**.

For more information about jumper configuration to achieve low-power using the MSPM0 LaunchPad, please visit the [LP-MSPM0G5187 User's Guide](https://www.ti.com/lit/slau967).

## Hardware Requirements

1. MSPM0G5187 LaunchPad (LP-MSPM0G5187)
2. TIDA-010997 EdgeAI BoosterPack

## Example Usage

### Hardware Setup
1. Connect the TIDA-010997 BoosterPack to the LP-MSPM0G5187 LaunchPad
2. Ensure all sensors are properly seated and connections are secure
3. Connect J7 1:2 on boosterpack

### Operation
1. **Build and Flash**
   - Build the firmware using the provided build system
   - Flash the firmware to the MSPM0G5187 LaunchPad
   - The device initializes all sensors (I2C, SPI, I2S, ADC) and UART/DAP communication

2. **CCStudio™ Edge AI Studio Connection**
   - Launch CCStudio™ Edge AI Studio on your host PC
   - Configure the serial port with baud rate **115200 bps**
   - Verify the status bar displays "**Hardware connected**"

3. **Data Capture**
   - Navigate to the **Capture** tab in CCStudio™ Edge AI Studio
   - Select the desired sensor from the available options:
     - PIR Motion (analog ADC data)
     - Temperature (HDC3020)
     - Humidity (HDC3020)
     - Pressure (BMP384)
     - Ambient Light (OPT4001)
     - Digital Microphone (ICS43434)
     - Accelerometer/Gyroscope (BMI270)
     - Hall Effect (TMAG5170)
   - Specify the number of samples to capture
   - Assign a label for the dataset
   - Click **Start Capture** to begin data acquisition
   - Data is streamed over DAP protocol and saved in CSV format

4. **Data Flow**
   - The firmware waits for host commands over DAP
   - When streaming is active, sensor data is read using the appropriate protocol (I2C/SPI/I2S/ADC)
   - Data is framed with DAP headers (start byte, channel, payload length) and transmitted via UART
   - The host receives and logs the data for training, analysis, or visualization

## Software Details

- **Communication Protocols**:
  - UART: 115200 baud for DAP communication
  - I2C: For HDC3020, BMP384, and OPT4001 sensors
  - SPI: For BMI270 and TMAG5170 sensors
  - I2S: For ICS43434 sensor


## References

- [ModelZoo Examples](https://github.com/TexasInstruments/tinyml-tensorlab/tree/main/tinyml-modelzoo/examples)
- MSPM0G5187 Technical Reference Manual: [Link](https://www.ti.com/product/MSPM0G5187)
- TIDA-010997 Reference Design: [Link](https://www.ti.com/tool/TIDA-010997)
- EdgeAI Software Guide: SDK_INSTALL_DIR/docs/english/middleware/edgeAI/MSPM0_EdgeAI_User_Guide.html
- [TI Neural Network Compiler Guide](https://software-dl.ti.com/mctools/nnc/mcu/users_guide/)
- TI Model Training Guide: [tinyml-tensorlab](https://github.com/TexasInstruments/tinyml-tensorlab/tree/main)

### Sensor Datasheets
- HDC3020 Temperature & Humidity Sensor: [Datasheet](https://www.ti.com/product/HDC3020)
- BMP384 Pressure Sensor: [Datasheet](https://www.bosch-sensortec.com/products/environmental-sensors/pressure-sensors/bmp384/)
- OPT4001 Ambient Light Sensor: [Datasheet](https://www.ti.com/product/OPT4001)
- ICS43434 Digital Microphone: [Datasheet](https://www.invensense.com/products/ics-43434/)
- BMI270 IMU: [Datasheet](https://www.bosch-sensortec.com/products/motion-sensors/imus/bmi270/)
- TMAG5170 Hall Effect Sensor: [Datasheet](https://www.ti.com/product/TMAG5170)