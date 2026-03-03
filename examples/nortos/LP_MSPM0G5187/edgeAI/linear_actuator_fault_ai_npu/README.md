## Example Summary

This example demonstrates linear actuator fault detection and classification using an AI model on the MSPM0G5187 microcontroller with hardware-accelerated inference. The system classifies sensor data from a linear actuator into one of four categories: Normal, Spalling, Lack of Lubrication, or Backlash. The classification result is indicated by toggling onboard LEDs.

The dataset contains raw sensor data from a linear actuator rig operated under different loading conditions and motion profiles, sampled at 25 Hz with three input features: position set point, position error, and motor current. The raw data has been pre-processed into 13 CSV files representing 1 normal condition and 12 fault conditions at various degradation stages (2 backlash stages, 2 lack of lubrication stages, and 8 spalling stages). Each file contains 60 test combinations covering 2 motion profiles, 3 loading conditions, and 10 repetitions.

In this example the inference happens using the TinyEngine™ NPU hardware accelerator, thus making it faster in comparison to using M0+ CPU to do the same.

The class to LED color mapping is shown below:
   - **Red**: Class 0 (Normal)
   - **Green**: Class 1 (Spalling - Surface defects on screw/balls)
   - **Blue**: Class 2 (Lack of Lubrication - Removed lubricant, tightened seals)
   - **Yellow** (Red+Green): Class 3 (Backlash - Undersized balls causing excess play)

## AI Model Information

| Property             | Value                                                           |
| -------------------- | --------------------------------------------------------------- |
| Model Architecture   | 5-layer CNN + 1 FC layer                                        |
| Number of Parameters | ~126,615                                                        |
| Input Shape          | Time-series (position set point, position error, motor current) |
| Output Classes       | 4                                                               |
| Quantization         | INT8                                                            |

A 5-layer quantized convolutional network with 1 pooling layer and progressive channel expansion (3→28→64→96→132→148), followed by 1 fully connected layer.

In the PyTorch training framework, neural networks are trained with optimizations (for example, aggressive quantization) that target TI MCUs. After training, the neural networks are compiled by the [TI Neural Network Compiler](https://software-dl.ti.com/mctools/nnc/mcu/users_guide/index.html). Options passed to the compiler determine which of the following actions the generated inference library performs:
1. Hardware accelerated inference using TinyEngine™ NPU.
2. Software-only inference using the CPU on the MCU.

This example is built for the hardware-accelerated inference mode using the TinyEngine™ NPU on MSPM0G5187.

The output from the TI Neural Network Compiler is an artifacts directory that will contain: A header file (for example, tvmgen_default.h), and a library file (for example, model.a). This makes the output from the compiler easier to integrate with the project.

## AI Performance

| Metric                   | Value     |
| ------------------------ | --------- |
| Accuracy (R square)      | ~99.9     |
| Flash Usage(KB)          | 66.6      |
| RAM Usage(KB)            | 28.7      |

*Note: Performance metrics measured on MSPM0G5187 at 80 MHz with TinyEngine™ NPU acceleration.*

### Device Migration Recommendations

This project was developed for a superset device included in the MSPM0 LaunchPad. Please visit the [CCS User's Guide](https://software-dl.ti.com/msp430/esd/MSPM0-SDK/latest/docs/english/tools/ccs_ide_guide/doc_guide/doc_guide-srcs/ccs_ide_guide.html#sysconfig-project-migration) for information about migrating to other MSPM0 devices.

**Note**: This example requires an NPU-enabled device (MSPM0G5187). Migration to non-NPU devices will require using the CPU-only version of this example.

### Low-Power Recommendations

TI recommends to terminate unused pins by setting the corresponding functions to GPIO and configure the pins to output low or input with internal pullup/pulldown resistor.

SysConfig allows developers to easily configure unused pins by selecting **Board**→**Configure Unused Pins**.

For more information about jumper configuration to achieve low-power using the MSPM0 LaunchPad, please visit the [LP-MSPM0G5187 User's Guide](https://www.ti.com/lit/slau967).

## Hardware Requirements

1. LP-MSPM0G5187 LaunchPad

## Example Usage

1. **Hardware Setup**
   - Connect the LP-MSPM0G5187 LaunchPad to your PC via USB.
   - Ensure the LED jumpers (J11, J12, J13) are populated to enable LED indication.

2. **Operation**
   - On startup, the application initializes the peripherals and powers up the NPU.
   - The NPU interrupt is enabled and the system waits for inference completion.
   - The pre-loaded test vector containing actuator sensor data is fed to the AI model.
   - The model performs hardware-accelerated inference using the TinyEngine™ NPU.
   - Once inference is complete, the NPU is powered down to save energy.
   - The LED corresponding to the detected class is turned on:
     - **Red**: Normal operation
     - **Green**: Spalling fault
     - **Blue**: Lack of Lubrication fault
     - **Yellow**: Backlash fault

3. **Running the Example**
   - Compile, load, and run the application on your MSPM0 device.
   - Observe the LED indication for the classification result.
   - To test different input data, modify the test vector in `linear_actuator_fault_testvector.h`.

## Software Details

- **AI Inference**: Runs a pre-trained 5-layer CNN model using TVM runtime with NPU acceleration.
- **NPU Management**: The application handles NPU power-up, interrupt configuration, and power-down.
- **LED Indication**: Classification result is indicated by toggling onboard LEDs with 4 different color combinations.
- **AI Model**: Model details are available on [Model Zoo](https://github.com/TexasInstruments/tinyml-tensorlab/tree/r1.3/tinyml-modelzoo)

## Notes

- This example uses a pre-loaded test vector for demonstration. For real-world applications, integrate with position and current sensors.
- The model is optimized for NPU deployment with aggressive quantization.
- The NPU is powered down after inference to minimize power consumption.
- The 4-class classification covers: Normal operation, Spalling (surface defects), Lack of Lubrication, and Backlash (excess play) conditions.

## References

- [ModelZoo Examples](https://github.com/TexasInstruments/tinyml-tensorlab/tree/main/tinyml-modelzoo/examples)
- MSPM0G5187 Technical Reference Manual [Link](https://www.ti.com/product/MSPM0G5187)
- [TI Neural Network Compiler Guide](https://software-dl.ti.com/mctools/nnc/mcu/users_guide/)
- TI Model Training Guide: [tinyml-tensorlab](https://github.com/TexasInstruments/tinyml-tensorlab/tree/main)
- EdgeAI Software Guide: SDK_INSTALL_DIR/docs/english/middleware/edgeAI/MSPM0_EdgeAI_User_Guide.html
- Linear Actuator Fault Dataset: [Cranfield University Repository](https://cran-test-dspace.koha-ptfs.co.uk/items/eff8a9f5-861e-49f1-9e0b-67d5c7393616/full)
