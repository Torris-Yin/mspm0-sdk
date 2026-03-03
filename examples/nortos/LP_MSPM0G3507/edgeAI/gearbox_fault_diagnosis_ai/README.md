## Example Summary

This example demonstrates gearbox fault detection using an AI model on the MSPM0G3507 microcontroller. The system classifies vibration data from a gearbox into one of two categories: Healthy or Broken Tooth condition. The classification result is indicated by toggling onboard LEDs.

The gearbox fault detection dataset includes vibration data recorded by SpectraQuest's Gearbox Fault Diagnostics Simulator. The dataset was recorded using 4 vibration sensors placed in four different directions, under varying load conditions from 0% to 90%. Two scenarios are included: Healthy condition and Broken Tooth condition.

In this example the inference happens using the M0+ CPU itself and no hardware accelerator is used.

The class to LED color mapping is shown below:
   - **Green**: Class 0 (Healthy)
   - **Red**: Class 1 (Broken Tooth)

## AI Model Information

| Property             | Value                       |
| -------------------- | --------------------------- |
| Model Architecture   | 3-layer CNN                 |
| Number of Parameters | ~1,410                      |
| Input Shape          | Time-series vibration data  |
| Output Classes       | 2 (Healthy, Broken Tooth)   |
| Quantization         | INT8                        |

A 3-layer convolutional neural network that uses early 9x pooling for spatial reduction, followed by three convolutional blocks with fixed 16-channel width and batch normalization.

In the PyTorch training framework, neural networks are trained with optimizations (for example, aggressive quantization) that target TI MCUs. After training, the neural networks are compiled by the [TI Neural Network Compiler](https://software-dl.ti.com/mctools/nnc/mcu/users_guide/index.html). Options passed to the compiler determine which of the following actions the generated inference library performs:
1. Hardware accelerated inference using TinyEngine™ NPU.
2. Software-only inference using the CPU on the MCU.

This example is built for the software-only inference mode using the CPU on MSPM0.

The output from the TI Neural Network Compiler is an artifacts directory that will contain: A header file (for example, tvmgen_default.h), and a library file (for example, model.a). This makes the output from the compiler easier to integrate with the project.

## AI Performance

| Metric                   | Value     |
| ------------------------ | --------- |
| Accuracy (R square)      | ~99.9     |
| Flash Usage(KB)          | 10.1      |
| RAM Usage(KB)            | 1.9       |

*Note: Performance metrics measured on MSPM0G3507 at 80 MHz.*

### Device Migration Recommendations

This project was developed for a superset device included in the MSPM0 LaunchPad. Please visit the [CCS User's Guide](https://software-dl.ti.com/msp430/esd/MSPM0-SDK/latest/docs/english/tools/ccs_ide_guide/doc_guide/doc_guide-srcs/ccs_ide_guide.html#sysconfig-project-migration) for information about migrating to other MSPM0 devices.

### Low-Power Recommendations

TI recommends to terminate unused pins by setting the corresponding functions to GPIO and configure the pins to output low or input with internal pullup/pulldown resistor.

SysConfig allows developers to easily configure unused pins by selecting **Board**→**Configure Unused Pins**.

For more information about jumper configuration to achieve low-power using the MSPM0 LaunchPad, please visit the [LP-MSPM0G3507 User's Guide](https://www.ti.com/lit/slau873).

## Hardware Requirements

1. LP-MSPM0G3507 LaunchPad

## Example Usage

1. **Hardware Setup**
   - Connect the LP-MSPM0G3507 LaunchPad to your PC via USB.
   - Ensure the LED connections are properly configured.

2. **Operation**
   - On startup, the application initializes the peripherals.
   - The pre-loaded test vector containing vibration data is fed to the AI model.
   - The model performs inference and classifies the input as Healthy or Broken Tooth.
   - The LED corresponding to the detected class is turned on:
     - **Green**: Healthy gearbox
     - **Red**: Broken Tooth condition

3. **Running the Example**
   - Compile, load, and run the application on your MSPM0 device.
   - Observe the LED indication for the classification result.
   - To test different input data, modify the test vector in `gearbox_fault_diagnosis_testvector.h`.

## Software Details

- **AI Inference**: Runs a pre-trained 3-layer CNN model using TVM runtime.
- **LED Indication**: Classification result is indicated by toggling onboard LEDs.
- **AI Model**: Model details are available on [Model Zoo](https://github.com/TexasInstruments/tinyml-tensorlab/tree/r1.3/tinyml-modelzoo)

## Notes

- This example uses a pre-loaded test vector for demonstration. For real-world applications, integrate with vibration sensors.
- The model is optimized for low-power MCU deployment with aggressive quantization.

## References

- [ModelZoo Examples](https://github.com/TexasInstruments/tinyml-tensorlab/tree/main/tinyml-modelzoo/examples)
- MSPM0G3507 Technical Reference Manual [Link](https://www.ti.com/product/MSPM0G3507)
- [TI Neural Network Compiler Guide](https://software-dl.ti.com/mctools/nnc/mcu/users_guide/)
- TI Model Training Guide: [tinyml-tensorlab](https://github.com/TexasInstruments/tinyml-tensorlab/tree/main)
- EdgeAI Software Guide: SDK_INSTALL_DIR/docs/english/middleware/edgeAI/MSPM0_EdgeAI_User_Guide.html
- Gearbox Fault Diagnosis Dataset: [Kaggle](https://www.kaggle.com/datasets/brjapon/gearbox-fault-diagnosis)
