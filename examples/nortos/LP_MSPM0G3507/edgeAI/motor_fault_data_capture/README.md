## Example Summary

This example implements a data-capture for motor fault demo for the MSPM0G3507.  
The device acquires vibration samples from an ADXL355 accelerometer and streams raw time-domain samples to a host over the Device Agent Protocol (DAP) via UART/USB. The host tool records and visualizes the acquired samples for offline model training, verification, or analysis.

### Device Migration Recommendations

This project was developed for a superset device included in the MSPM0 LaunchPad. Please visit the [CCS User's Guide](https://software-dl.ti.com/msp430/esd/MSPM0-SDK/latest/docs/english/tools/ccs_ide_guide/doc_guide/doc_guide-srcs/ccs_ide_guide.html#sysconfig-project-migration) for information about migrating to other MSPM0 devices.

### Low-Power Recommendations

TI recommends to terminate unused pins by setting the corresponding functions to GPIO and configure the pins to output low or input with internal pullup/pulldown resistor.

SysConfig allows developers to easily configure unused pins by selecting **Board**→**Configure Unused Pins**.

For more information about jumper configuration to achieve low-power using the MSPM0 LaunchPad, please visit the [LP-MSPM0G3507 User's Guide](https://www.ti.com/lit/slau873).

## Hardware Requirements

1. LP-MSPM0G3507 LaunchPad
2. ADXL355 accelerometer connected over SPI
3. Host PC with a USB/serial connection to the board (or USB DFU supporting the live DAP bridge)

## Example Usage

1. **Hardware Setup**
   - Connect ADXL355 SPI lines to SPI0 (PB18 SCLK, PB17 MOSI, PB19 MISO, PB0 CS).
   - Connect ADXL355 DRDY (data-ready) to PA12 (GPIO interrupt).
   - Power the sensor from the board's regulated 3.3V and ensure common ground.
   - Secure the accelerometer to the device under test (motor) for accurate vibration coupling.

2. **Operation**
   - On startup, firmware initializes SPI, GPIO, interrupts and UART/DAP components and configures the ADXL355 (range, power control, interrupt map).
   - The device waits for host commands over DAP. The host can request a capture by issuing the start-streaming command.
   - When streaming is active, each DRDY interrupt triggers SPI reads to collect XYZ samples. Samples are queued and transmitted to the host using the DAP framing over UART.
   - The DAP framing supports variable payload lengths; the firmware encodes frame start, channel, length and payload, and ends with a frame-end marker.
   - The host receives raw time-domain samples (interleaved X,Y,Z) and saves it in the csv format.

3. **Running the Example (Host)**
   - Build and flash the firmware to the MSPM0G3507.
   - On the host PC run the provided capture/listen tool or CCStudio™ Edge AI Studio Live Preview set to "Data Capture" mode.
   - From CCStudio™ Edge AI Studio connect to the device and request a data capture session via the GUI.
   - Make sure that the correct baudrate (115200) and COM ports are selected before connecting to the device.

## Software Details

- Data acquisition reads ADXL355 XYZ samples on each DRDY event using SPI.
- Samples are packed as 16-bit signed integers (high byte first) and streamed in DAP frames.
- UART settings: 115200 baud.
- Key configurable property: "time" — the host sets how many samples to stream/transfer based on sampling rate (1024 default)*time.

## Notes

- Ensure the ADXL355 sensor is properly powered and connected.
- For more information on the ADXL355, refer to the datasheets and user guides.
   - [Datasheet](https://www.analog.com/media/en/technical-documentation/data-sheets/adxl354_adxl355.pdf)
   - [User guide](https://www.digikey.in/en/htmldatasheets/production/2012480/0/0/1/eval-adxl354-355-user-guide)
- ADXL355 manufacturer page [link](https://www.analog.com/en/index.html)

## References

- [ModelZoo Example](https://github.com/TexasInstruments/tinyml-tensorlab/tree/main/tinyml-modelzoo/examples)
- MSPM0G3507 Technical Reference Manual [Link](https://www.ti.com/product/MSPM0G3507)
- EdgeAI Software Guide: SDK_INSTALL_DIR/docs/english/middleware/edgeAI/MSPM0_EdgeAI_User_Guide.html