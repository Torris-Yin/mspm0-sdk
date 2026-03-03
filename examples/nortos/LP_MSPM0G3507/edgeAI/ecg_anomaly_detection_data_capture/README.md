## Example Summary

This example implements a data-capture for ECG Anomaly detection for the MSPM0G3507.  
The device acquires vibration samples from an AFE1594 sensor and streams raw time-domain samples to a host over the Device Agent Protocol (DAP) via UART/USB. The host tool records and visualizes the acquired samples for offline model training, verification, or analysis.

### Device Migration Recommendations

This project was developed for a superset device included in the MSPM0 LaunchPad. Please visit the [CCS User's Guide](https://software-dl.ti.com/msp430/esd/MSPM0-SDK/latest/docs/english/tools/ccs_ide_guide/doc_guide/doc_guide-srcs/ccs_ide_guide.html#sysconfig-project-migration) for information about migrating to other MSPM0 devices.

### Low-Power Recommendations

TI recommends to terminate unused pins by setting the corresponding functions to GPIO and configure the pins to output low or input with internal pullup/pulldown resistor.

SysConfig allows developers to easily configure unused pins by selecting **Board**→**Configure Unused Pins**.

For more information about jumper configuration to achieve low-power using the MSPM0 LaunchPad, please visit the [LP-MSPM0G3507 User's Guide](https://www.ti.com/lit/slau873).

## Hardware Requirements

1. LP-MSPM0G3507 LaunchPad
2. AFE1594 Analog Front-End for ECG acquisition
3. Host PC with a USB/serial connection to the board (or USB DFU supporting the live DAP bridge)
4. ECG electrodes and connectors

## Example Usage

1. **Hardware Setup**
   - Connect the AFE1594 analog front-end to the MSPM0 SPI interface:
     - Controller SCLK (PB18) -> AFE SCLK
     - Controller MOSI (PB17) -> AFE MOSI
     - Controller MISO (PB19) <- AFE MISO
     - Controller CS (PA0) -> AFE CS
   - Connect AFE control pins:
     - Reset (PA15) -> AFE Reset
     - Power Down (PA26) -> AFE PDN
   - Connect the AFE interrupt pin (Data Ready) to the designated GPIO interrupt pin (PA8).
   - Connect ECG electrodes to the AFE1594 inputs according to the AFE1594 documentation.

2. **Operation**
   - On startup, the application initializes the AFE and peripherals.
   - The AFE1594 collects ECG signal data.
   - When new data is ready, an interrupt triggers data acquisition.
   - The raw ECG data is processed and fed to the AI model for inference.
   - The output class is determined and stored in a circular buffer.
   - The LED corresponding to the most frequently detected class in the buffer is toggled:
     - **LED1 (Blue)**: Class 0 (Normal)
     - **LED2 (Red)**: Class 1 (Mild)
     - **LED3 (Green)**: Class 2 (Severe)
     - **All LEDs**: Class 3 (Emergency)

   - On startup, firmware initializes SPI, GPIO, interrupts and UART/DAP components and configures the AFE1594 (range, power control, interrupt map).
   - The device waits for host commands over DAP. The host can request a capture by issuing the start-streaming command.
   - When streaming is active, each DRDY interrupt triggers SPI reads to collect 125 samples. Samples are queued and transmitted to the host using the DAP framing over UART.
   - The DAP framing supports variable payload lengths. The firmware encodes frame start, channel, length and payload, and ends with a frame-end marker.
   - The host receives raw time-domain samples and saves it in the csv format.

3. **Running the Example**
   - Compile, load, and run the application on your MSPM0 device.
   - The system will continuously monitor ECG signals and indicate detected classes via LEDs.

## Software Details

- **Data Acquisition**: Efficient SPI data collection from AFE1594.
- **Signal Processing**: Raw ECG signal processing for feature extraction.
- **AI Inference**: Runs a pre-trained model using TVM runtime on the NPU.
- **Interrupt Handling**: Efficient data collection via GPIO interrupts.
- **LED Indication**: Classification result is indicated by toggling onboard LEDs.
- **AI model used**: Model details can be found in TensorLab repository.

## Notes

- Ensure the AFE1594 is properly powered and connected.
- Proper electrode placement is critical for accurate ECG measurements.

## References

- [ECG ModelZoo Example](https://github.com/TexasInstruments/tinyml-tensorlab/tree/main/tinyml-modelzoo/examples/ecg_classification)
- MSPM0G3507 Technical Reference Manual [Link](https://www.ti.com/product/MSPM0G3507)
- [TI Neural Network Compiler Guide](https://software-dl.ti.com/mctools/nnc/mcu/users_guide/)
- TI Model Training Guide: [tinyml-tensorlab](https://github.com/TexasInstruments/tinyml-tensorlab/tree/main)
- EdgeAI Software Guide: SDK_INSTALL_DIR/docs/english/middleware/edgeAI/MSPM0_EdgeAI_User_Guide.html
- For more information on the AFE1594, refer to the TI datasheet and user guides.