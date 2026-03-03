## Example Summary

This example demonstrates multi-sensor data capture for TIDA-010997 reference design using EdgeAI Studio. The firmware acquires data from various sensors including PIR motion sensor (via ADC), temperature & humidity (HDC3020), pressure (BMP384), ambient light (OPT4001), digital microphone (ICS43434), and IMU (BMI270). Data is streamed to a host PC over the Device Agent Protocol (DAP) via UART for data acquisition, model training, verification, or analysis.

The example supports multiple sensor types:
- **PIR Motion Sensor**: Analog signal captured via ADC at 31.25 Hz sampling rate
- **Temperature & Humidity**: I2C-based HDC3020 sensor
- **Pressure**: I2C-based BMP384 barometric pressure sensor
- **Ambient Light**: I2C-based OPT4001 optical sensor
- **IMU**: SPI-based BMI270 6-axis accelerometer and gyroscope

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

2. **EdgeAI Studio Connection**
   - Launch EdgeAI Studio on your host PC
   - Configure the serial port with baud rate **115200 bps**
   - Verify the status bar displays "**Hardware connected**"

3. **Data Capture**
   - Navigate to the **Capture** tab in EdgeAI Studio
   - Select the desired sensor from the available options:
     - PIR Motion (analog ADC data)
     - Temperature (HDC3020)
     - Humidity (HDC3020)
     - Pressure (BMP384)
     - Ambient Light (OPT4001)
     - Digital Microphone (ICS43434)
     - Accelerometer/Gyroscope (BMI270)
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
  - SPI: For BMI270 IMU
  - I2S: For ICS43434 digital microphone


## References

- [PIR Detection ModelZoo Example](https://github.com/TexasInstruments/tinyml-tensorlab/tree/main/tinyml-modelzoo/examples/pir_detection)
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