## Example Summary

  This example demonstrates glass break detection using an AI model on the MSPM0G5187 microcontroller with hardware-accelerated inference.

  A glass break detector is a sensor used in building automation and security systems. It generates an alarm when a pane of glass is shattered or broken. These sensors are commonly installed near glass doors or storefront windows and typically use a microphone to monitor noise or vibrations from the glass.

  In this example, both feature extraction (using filterbanks) and classification are performed using AI models. The inference runs on the TinyEngine hardware accelerator, enabling faster execution compared to using the M0+ CPU alone.
  The class to LED color mapping is shown below:
   - **Red**: Class 0 (Glass Break Event)
   - **Green**: Class 1 (Normal)
   - **Blue** : Class 2 (Sound Similar to Glass Break)

## AI Model Information

| Property                                        | Value                                                           |
| ----------------------------------------------- | --------------------------------------------------------------- |
| Main Model Architecture                         | TCDSResNet( 1 initial Conv+ 3 TCDS Basic Block+ 1 linear layer) |
| FE Model Architecture                           | 1-D CNN                                                         |
| Number of Parameters( Main Model)               | ~55k                                                            |
| Number of Parameters( Feature Extraction Model) | ~20k                                                            |
| Input Shape                                     | Time-series vibration data                                      |
| Output Classes                                  | 3 (Glass Break Event, Normal, Event similar to glass break)     |
| Quantization                                    | INT8                                                            |


In the PyTorch training framework, neural networks are trained with optimizations (for example, aggressive quantization) that target TI MCUs. After training, the neural networks are compiled by the [TI Neural Network Compiler](https://software-dl.ti.com/mctools/nnc/mcu/users_guide/index.html). Options passed to the compiler determine which of the following actions the generated inference library performs:
1. Hardware accelerated inference using TinyEngine™ NPU.
2. Software-only inference using the CPU on the MCU.

This example is built for the hardware-accelerated inference mode using the TinyEngine™ NPU on MSPM0G5187.

The output from the TI Neural Network Compiler is an artifacts directory that will contain: A header file (for example, tvmgen_default.h), and a library file (for example, model.a). This makes the output from the compiler easier to integrate with the project.

## AI Performance

| Metric                  | Value   |
| ----------------------- | ------- |
| Accuracy(Main Model)    | 97.08%  |
| Flash Usage(Main Model) | 31.7 KB |
| RAM Usage(Main Model)   | 6.7  KB |
| Flash Usage(FE Model)   | 4.6  KB |

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
   - The model performs hardware-accelerated feature extraction aswell as inference using the TinyEngine™ NPU.
   - Once inference is complete, the NPU is powered down to save energy.
   - The LED corresponding to the detected class is turned on:
     - **Red**: Class 0 (Glass Break Event)
     - **Green**: Class 1 (Normal)
     - **Blue** : Class 2 (Sound Similar to Glass Break)

3. **Running the Example**
   - Compile, load, and run the application on your MSPM0 device.
   - Observe the LED indication for the classification result.
   - To test different input data, modify the test vector in `gearbox_fault_diagnosis_testvector.h`.

## Software Details

- **AI Inference**: Runs a pre-trained TCDSResNet model using TVM runtime with NPU acceleration.
- **NPU Management**: The application handles NPU power-up, interrupt configuration, and power-down.
- **LED Indication**: Classification result is indicated by toggling onboard LEDs.

## Notes

- This example uses a pre-loaded test vector for demonstration. For real-world applications, integrate with vibration sensors.
- The model is optimized for NPU deployment with aggressive quantization.
- The NPU is powered down after inference to minimize power consumption.

## References

- [ModelZoo Examples](https://github.com/TexasInstruments/tinyml-tensorlab/tree/main/tinyml-modelzoo/examples)
- MSPM0G5187 Technical Reference Manual [Link](https://www.ti.com/product/MSPM0G5187)
- [TI Neural Network Compiler Guide](https://software-dl.ti.com/mctools/nnc/mcu/users_guide/)
- TI Model Training Guide: [tinyml-tensorlab](https://github.com/TexasInstruments/tinyml-tensorlab/tree/main)
- EdgeAI Software Guide:[Link](https://dev.ti.com/tirex/explore/node?node=A__AKCnvqDed-Plz2JO5Umb3Q__MSPM0-SDK__a3PaaoK__LATEST)
- Tensorlab User Guide [Link](https://software-dl.ti.com/C2000/esd/mcu_ai/01_03_00/user_guide/index.html)
