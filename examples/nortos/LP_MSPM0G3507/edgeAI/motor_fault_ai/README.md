## Example Summary

This example demonstrates a motor fault detection application using an AI model on MSPM0G3507 microcontroller. The system collects vibration data from an ADXL355 accelerometer, extracts frequency-domain features using FFT, and performs inference using a pre-trained neural network model. The detected fault class is indicated by toggling onboard LEDs.

In this example the inference happens using the M0+ CPU itself and no hardware accelerator is used.

The class to LED color mapping is shown below:
   - **Blue**: Class 0 (Blade Damage)
   - **Red**: Class 1 (Blade Imbalance)
   - **Green**: Class 2 (Blade Obstruction)
   - **White**: Class 3 (Normal)

## AI Model Information

| Property | Value |
| --- | --- |
| Model Architecture | CNN |
| Number of Parameters | 5,228 |
| Input Shape | (1, 256) |
| Output Classes | 4 |
| Quantization | INT8 |

In the PyTorch training framework, neural networks are trained with optimizations (for example, aggressive quantization) that target TI MCUs. After training, the neural networks are compiled by the [TI Neural Network Compiler](https://software-dl.ti.com/mctools/nnc/mcu/users_guide/index.html). Options passed to the compiler determine which of the following actions the generated inference library performs:
1. Hardware accelerated inference using TinyEngine™ NPU.
2. Software-only inference using the CPU on the MCU.

The output from the TI Neural Network Compiler is an artifacts directory that will contain: A header file (for example, tvmgen_default.h), and a library file (for example, model.a). This makes the output from the compiler easier to integrate with the project.

## AI Performance

| Metric | Value |
| --- | --- |
| Accuracy | ~99% |
| Flash Usage | 1.4 kB |
| RAM Usage | 1 kB |
| Inference Latency (CPU) | 2.09 ms |

*Note: Performance metrics measured on LP-MSPM0G5187.*

### Device Migration Recommendations

This project was developed for a superset device included in the MSPM0 LaunchPad. Please visit the [CCS User's Guide](https://software-dl.ti.com/msp430/esd/MSPM0-SDK/latest/docs/english/tools/ccs_ide_guide/doc_guide/doc_guide-srcs/ccs_ide_guide.html#sysconfig-project-migration) for information about migrating to other MSPM0 devices.

### Low-Power Recommendations

TI recommends to terminate unused pins by setting the corresponding functions to GPIO and configure the pins to output low or input with internal pullup/pulldown resistor.

SysConfig allows developers to easily configure unused pins by selecting **Board**→**Configure Unused Pins**.

For more information about jumper configuration to achieve low-power using the MSPM0 LaunchPad, please visit the [LP-MSPM0G3507 User's Guide](https://www.ti.com/lit/slau873).

## Hardware Requirements

1. LP-MSPM0G3507 LaunchPad
2. ADXL355 accelerometer [Link](https://www.digikey.in/en/products/detail/analog-devices-inc/EVAL-ADXL355-PMDZ/7324256)
3. DC Brushless Fan (BLDC) for motor fault testing [Link](https://www.digikey.in/en/products/detail/sunon-fans/EF80251S1-1000U-A99/6198727)

## Example Usage

1. **Hardware Setup**
   - Connect the ADXL355 accelerometer (SPI Peripheral) to the MSPM0 SPI interface (SPI Controller).
     - Controller SCLK -> Peripheral SCLK
     - Controller PICO -> Peripheral PICO
     - Controller POCI <- Peripheral POCI
     - Controller CS   -> Peripheral CS
   - Connect the sensor ready pin (Data Ready - DRDY) to the designated GPIO interrupt pin (PA12).
   - Ensure LEDs are connected to the appropriate GPIO pins for class indication.
   - Ensure that the digital ground and power from the microcontroller is connected to respective pins on the sensor.
   - Mount the sensor on top of the motor.

2. **Operation**
   - On startup, the application initializes the sensor and peripherals.
   - The ADXL355 accelerometer collects vibration data from the motor.
   - When new data is ready, an interrupt triggers data acquisition.
   - The application performs feature extraction using FFT and bins the frequency magnitudes.
   - Extracted features are fed to the AI model for inference.
   - The output class (fault type) is determined and stored in a circular buffer.
   - The LED corresponding to the most frequently detected class in the buffer is toggled.

3. **Running the Example**
   - Compile, load, and run the application on your MSPM0 device.
   - The system will continuously monitor motor vibration and indicate detected fault classes via LEDs.

## Software Details

- **Data Acquisition**: Vibration data collected from ADXL355 accelerometer via SPI.
- **Feature Extraction**: Uses ARM CMSIS-DSP library for real FFT and magnitude calculation.
- **AI Inference**: Runs a pre-trained MLP model using TVM runtime.
- **Interrupt Handling**: Efficient data collection via GPIO interrupts.
- **LED Indication**: Fault class is indicated by toggling onboard LEDs.
- **AI Model**: Model details are available on [Model Zoo](https://github.com/TexasInstruments/tinyml-tensorlab/tree/r1.3/tinyml-modelzoo)

## Notes

- Ensure the ADXL355 sensor is properly powered and connected.
- Adjust LED and GPIO pin assignments as needed for your hardware.
- The model uses frequency-domain features extracted from accelerometer data.

## References

- [ModelZoo Example](https://github.com/TexasInstruments/tinyml-tensorlab/tree/main/tinyml-modelzoo/examples)
- MSPM0G3507 Technical Reference Manual [Link](https://www.ti.com/product/MSPM0G3507)
- [TI Neural Network Compiler Guide](https://software-dl.ti.com/mctools/nnc/mcu/users_guide/)
- TI Model Training Guide: [tinyml-tensorlab](https://github.com/TexasInstruments/tinyml-tensorlab/tree/main)
- EdgeAI Software Guide:[Link](https://dev.ti.com/tirex/explore/node?node=A__AKCnvqDed-Plz2JO5Umb3Q__MSPM0-SDK__a3PaaoK__LATEST)
- ADXL355 Datasheet: [Link](https://www.analog.com/media/en/technical-documentation/data-sheets/adxl354_adxl355.pdf)
- ADXL355 User Guide: [Link](https://www.digikey.in/en/htmldatasheets/production/2012480/0/0/1/eval-adxl354-355-user-guide)
- Tensorlab User Guide [Link](https://software-dl.ti.com/C2000/esd/mcu_ai/01_03_00/user_guide/index.html)
