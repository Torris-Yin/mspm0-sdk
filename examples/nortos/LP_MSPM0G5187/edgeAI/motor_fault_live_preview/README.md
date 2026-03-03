## Example Summary

This example demonstrates a motor fault detection application using an AI model on MSPM0G5187 microcontroller. The system collects vibration data from an ADXL355 accelerometer, extracts frequency-domain features using FFT, and performs inference using a pre-trained neural network model. The detected fault class is indicated by toggling onboard LEDs.

This application also supports live preview using CCStudio™ Edge AI Studio. The device transfers the inferred class and the class outputs to a host(the connected PC) over the Device Agent Protocol (DAP) via UART/USB. The live results are then previewed on the CCStudio™ Edge AI Studio.

In this example the inference happens using the hardware accelerator(NPU) available on the device.

The class to LED color mapping is shown below:
   - **Blue**: Class 0 (Blade Damage)
   - **Red**: Class 1 (Blade Imbalance)
   - **Green**: Class 2 (Blade Obstruction)
   - **White**: Class 3 (Normal)

### Device Migration Recommendations

This project was developed for a superset device included in the MSPM0 LaunchPad. Please visit the [CCS User's Guide](https://software-dl.ti.com/msp430/esd/MSPM0-SDK/latest/docs/english/tools/ccs_ide_guide/doc_guide/doc_guide-srcs/ccs_ide_guide.html#sysconfig-project-migration) for information about migrating to other MSPM0 devices.

### Low-Power Recommendations

TI recommends to terminate unused pins by setting the corresponding functions to GPIO and configure the pins to output low or input with internal pullup/pulldown resistor.

SysConfig allows developers to easily configure unused pins by selecting **Board**→**Configure Unused Pins**.

For more information about jumper configuration to achieve low-power using the MSPM0 LaunchPad, please visit the [LP-MSPM0G5187 User's Guide](https://www.ti.com/lit/slau967).

## Hardware Requirements

1. LP-MSPM0G5187 LaunchPad
2. ADXL355 accelerometer (sensor)
3. Host PC with a USB/serial connection to the board (or USB DFU supporting the live DAP bridge)
4. EF80251S1-1000U-A99 (DC Brushless Fans - BLDC)

## Example Usage

1. **Hardware Setup**
   - Connect the ADXL355 accelerometer (SPI Peripheral) to the MSPM0 SPI interface (SPI Controller).
     - Controller SCLK -> Peripheral SCLK
     - Controller PICO -> Peripheral PICO
     - Controller POCI <- Peripheral POCI
     - Controller CS   -> Peripheral CS
   - Connect the sensor ready pin (Data Ready - DRDY) to the designated GPIO interrupt pin (PA12).
     - DRDY -> Standard Input
   - Ensure LEDs are connected to the appropriate GPIO pins for class indication.
   - Ensure that the digital ground and power from the microcontroller is connected to respective pins on the sensor.
   - Stick the sensor to the top of the motor.

2. **Operation**
   - On startup, the application initializes the sensor and peripherals.
   - The ADXL355 accelerometer collects vibration data from the motor.
   - When new data is ready, an interrupt triggers data acquisition.
   - The application performs feature extraction using FFT and bins the frequency magnitudes.
   - Extracted features are fed to the AI model for inference.
   - The output class (fault type) is determined and stored in a circular buffer.
   - The LED corresponding to the most frequently detected class in the buffer is toggled:
     - **Blue**: Class 0 (Blade Damage)
     - **Red**: Class 1 (Blade Imbalance)
     - **Green**: Class 2 (Blade Obstruction)
     - **White**: Class 3 (Normal)
   - Live Preview:
      - Open CCStudio™ Edge AI Studio.
      - Open the motor fault example.
      - Click on the Live Preview tab.
      - Make sure the application is running on the device and click on connect to device.
      - Click on start preview to preview the predicted class and the output values of each class on the GUI. 

3. **Running the Example**
   - Compile, load, and run the application on your MSPM0 device.
   - The system will continuously monitor motor vibration and indicate detected fault classes via LEDs.
   - CCStudio™ Edge AI Studio will display the predicted class and the output values of each class.
   - Make sure that the correct baudrate (115200) and COM ports are selected before connecting to the device.

## Software Details

- **Feature Extraction**: Uses ARM CMSIS-DSP library for real FFT and magnitude calculation.
- **AI Inference**: Runs a pre-trained model using TVM runtime.
- **Interrupt Handling**: Efficient data collection via GPIO interrupts.
- **LED Indication**: Fault class is indicated by toggling onboard LEDs.
- **AI model used**: Model details are available on [Model Zoo](https://github.com/TexasInstruments/tinyml-tensorlab/tree/r1.3/tinyml-modelzoo)
- Samples are packed as 16-bit signed integers (high byte first) and streamed in DAP frames.
- UART settings: 115200 baud.

## Notes

- Ensure the ADXL355 sensor is properly powered and connected.
- Adjust LED and GPIO pin assignments as needed for your hardware.

## References

- [ModelZoo Example](https://github.com/TexasInstruments/tinyml-tensorlab/tree/main/tinyml-modelzoo/examples)
- MSPM0G5187 Technical Reference Manual [Link](https://www.ti.com/product/MSPM0G5187)
- [TI Neural Network Compiler Guide](https://software-dl.ti.com/mctools/nnc/mcu/users_guide/)
- TI Model Training Guide: [tinyml-tensorlab](https://github.com/TexasInstruments/tinyml-tensorlab/tree/main)
- EdgeAI Software Guide: SDK_INSTALL_DIR/docs/english/middleware/edgeAI/MSPM0_EdgeAI_User_Guide.html
- For more information on the ADXL355, refer to the datasheets and user guides.
    - [Datasheet](https://www.analog.com/media/en/technical-documentation/data-sheets/adxl354_adxl355.pdf)
    - [User guide](https://www.digikey.in/en/htmldatasheets/production/2012480/0/0/1/eval-adxl354-355-user-guide)
- ADXL355 manufacturer page [link](https://www.analog.com/en/index.html)