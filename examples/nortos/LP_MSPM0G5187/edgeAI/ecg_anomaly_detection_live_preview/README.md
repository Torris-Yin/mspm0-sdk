## Example Summary

This example demonstrates an ECG (Electrocardiogram) classification application using an AI model on MSPM0G5187 microcontroller. The system collects ECG data from an AFE1594 analog front-end, processes the signal, and performs inference using a pre-trained neural network model. The detected ECG class is indicated by toggling onboard LEDs.

This application also supports live preview using EdgeAI Studio Model Composer GUI. The device transfers the inferred class and the class outputs to a host(the connected PC) over the Device Agent Protocol (DAP) via UART/USB. The live results are then previewed on the EdgeAI Studio GUI.

In this example the inference happens using the hardware accelerator(NPU) available on the device.

The class to LED color mapping is shown below:
   - **Green**: Class 1 (Normal)
   - **Yellow**: Class 2 (Mild)
   - **Red**: Class 3 (Other)

### Low-Power Recommendations

Terminate unused pins by configuring them as GPIO outputs low or inputs with
internal pull-up/pull-down resistors. Use SysConfig to easily configure unused pins.

SysConfig allows developers to easily configure unused pins by selecting **Board**→**Configure Unused Pins**.

For more information about jumper configuration to achieve low-power using the
MSPM0 LaunchPad, please visit the [LP-MSPM0G5187 User's Guide](https://www.ti.com/lit/slau967).

## Hardware requirements

1. MSPM0G5187 microcontroller
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
     - Power Down (PA25) -> AFE PDN
   - Connect the AFE interrupt pin (Data Ready) to the designated GPIO interrupt pin (PA8).
   - Ensure LEDs are connected to the appropriate GPIO pins for class indication.
   - Connect ECG electrodes to the AFE1594 inputs according to the AFE1594 documentation.

2. **Operation**
   - On startup, the application initializes the AFE and peripherals.
   - The AFE1594 collects ECG signal data.
   - When new data is ready, an interrupt triggers data acquisition.
   - The raw ECG data is processed and fed to the AI model for inference.
   - The output class is determined and stored in a circular buffer.
   - The LED corresponding to the most frequently detected class in the buffer is toggled:
     - **LED1 (Green)**: Class 0 (Normal)
     - **LED2 (Yellow)**: Class 1 (Mild)
     - **LED3 (Red)**: Class 2 (Other)
   - Live Preview:
      - Open the Model Composer Edge AI studiO.
      - Open the ECG Anomaly detection example.
      - Click on the Live Preview tab.
      - Make sure the apllication is running on the device and click on connect to device.
      - Click on start preview to preview the predicted class and the output values of each class on the GUI. 

3. **Running the Example**
   - Compile, load, and run the application on your MSPM0 device.
   - The system will continuously monitor ECG signals and indicate detected classes via LEDs.
   - The Edge AI model composer studio will display the predicted class and the output values of each of the class.
   - Make sure that the correct baudrate (115200) and COM ports are selected before connecting to the device.

## Software Details

- **Data Acquisition**: Efficient SPI data collection from AFE1594.
- **Signal Processing**: Raw ECG signal processing for feature extraction.
- **AI Inference**: Runs a pre-trained model using TVM runtime on the NPU.
- **Interrupt Handling**: Efficient data collection via GPIO interrupts.
- **LED Indication**: Classification result is indicated by toggling onboard LEDs.
- **AI model used**: Model details are available on [Model Zoo](https://github.com/TexasInstruments/tinyml-tensorlab/tree/r1.3/tinyml-modelzoo)
- Samples are packed as 16-bit signed integers (high byte first) and streamed in DAP frames.
- UART settings: 115200 baud.

## Notes

- Ensure the AFE1594 is properly powered and connected.
- Proper electrode placement is critical for accurate ECG measurements.
- Adjust LED and GPIO pin assignments as needed for your hardware.

## References

- [ECG ModelZoo Example](https://github.com/TexasInstruments/tinyml-tensorlab/tree/main/tinyml-modelzoo/examples/ecg_classification)
- MSPM0G5187 Technical Reference Manual [Link](https://www.ti.com/product/MSPM0G5187)
- [TI Neural Network Compiler Guide](https://software-dl.ti.com/mctools/nnc/mcu/users_guide/)
- TI Model Training Guide: [tinyml-tensorlab](https://github.com/TexasInstruments/tinyml-tensorlab/tree/main)
- EdgeAI Software Guide: SDK_INSTALL_DIR/docs/english/middleware/edgeAI/MSPM0_EdgeAI_User_Guide.html
- For more information on the AFE1594, refer to the TI datasheet and user guides.