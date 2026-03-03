## Example Summary

This example demonstrates gearbox fault detection using an AI model on the MSPM0G5187 microcontroller with hardware-accelerated inference. The system classifies vibration data from a gearbox into one of two categories: Healthy or Broken Tooth condition. The classification result is indicated by toggling onboard LEDs.

The gearbox fault detection dataset includes vibration data recorded by SpectraQuest's Gearbox Fault Diagnostics Simulator. The dataset was recorded using 4 vibration sensors placed in four different directions, under varying load conditions from 0% to 90%. Two scenarios are included: Healthy condition and Broken Tooth condition.

In this example the inference happens using the TinyEngine™ hardware accelerator, thus making it faster in comparison to using M0+ CPU to do the same.

The class to LED color mapping is shown below:
   - **Green**: Class 0 (Healthy)
   - **Red**: Class 1 (Broken Tooth)

## AI Model Information

| Property              | Value                      |
| --------------------- | -------------------------- |
| Model Architecture    | 4-layer CNN                |
| Number of Parameters  | ~2,672                     |
| Input Shape           | Time-series vibration data |
| Output Classes        | 2 (Healthy, Broken Tooth)  |
| Quantization          | INT8                       |

A 4-layer convolutional neural network that uses aggressive early pooling followed by progressive channel expansion (4→12→16→24→32) with batch normalization.

In the PyTorch training framework, neural networks are trained with optimizations (for example, aggressive quantization) that target TI MCUs. After training, the neural networks are compiled by the [TI Neural Network Compiler](https://software-dl.ti.com/mctools/nnc/mcu/users_guide/index.html). Options passed to the compiler determine which of the following actions the generated inference library performs:
1. Hardware accelerated inference using TinyEngine™ NPU.
2. Software-only inference using the CPU on the MCU.

This example is built for the hardware-accelerated inference mode using the TinyEngine™ NPU on MSPM0G5187.

The output from the TI Neural Network Compiler is an artifacts directory that will contain: A header file (for example, tvmgen_default.h), and a library file (for example, model.a). This makes the output from the compiler easier to integrate with the project.

## AI Performance

| Metric                   | Value     |
| ------------------------ | --------- |
| Accuracy                 | ~99.9     |
| Flash Usage(KB)          | 8.9       |
| RAM Usage(KB)            | 2.2       |

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
   - The pre-loaded test vector containing vibration data is fed to the AI model.
   - The model performs hardware-accelerated inference using the TinyEngine™ NPU.
   - Once inference is complete, the NPU is powered down to save energy.
   - The LED corresponding to the detected class is turned on:
     - **Green**: Healthy gearbox
     - **Red**: Broken Tooth condition

3. **Running the Example**
   - Compile, load, and run the application on your MSPM0 device.
   - Observe the LED indication for the classification result.
   - To test different input data, modify the test vector in `gearbox_fault_diagnosis_testvector.h`.

## Software Details

- **AI Inference**: Runs a pre-trained 4-layer CNN model using TVM runtime with NPU acceleration.
- **NPU Management**: The application handles NPU power-up, interrupt configuration, and power-down.
- **LED Indication**: Classification result is indicated by toggling onboard LEDs.
- **AI Model**: Model details are available on [Model Zoo](https://github.com/TexasInstruments/tinyml-tensorlab/tree/r1.3/tinyml-modelzoo)

## Notes

- This example uses a pre-loaded test vector for demonstration. For real-world applications, integrate with vibration sensors.
- The model is optimized for NPU deployment with aggressive quantization.
- The NPU is powered down after inference to minimize power consumption.

## References

- [ModelZoo Examples](https://github.com/TexasInstruments/tinyml-tensorlab/tree/main/tinyml-modelzoo/examples)
- MSPM0G5187 Technical Reference Manual [Link](https://www.ti.com/product/MSPM0G5187)
- [TI Neural Network Compiler Guide](https://software-dl.ti.com/mctools/nnc/mcu/users_guide/)
- TI Model Training Guide: [tinyml-tensorlab](https://github.com/TexasInstruments/tinyml-tensorlab/tree/main)
- EdgeAI Software Guide: SDK_INSTALL_DIR/docs/english/middleware/edgeAI/MSPM0_EdgeAI_User_Guide.html
- Gearbox Fault Diagnosis Dataset: [Kaggle](https://www.kaggle.com/datasets/brjapon/gearbox-fault-diagnosis)
