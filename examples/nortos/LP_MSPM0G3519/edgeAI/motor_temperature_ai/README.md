## Example Summary

This example demonstrates motor temperature prediction using an AI regression model on the MSPM0G3519 microcontroller. The system predicts the permanent magnet surface temperature of a Permanent Magnet Synchronous Motor (PMSM) based on sensor measurements. The prediction accuracy is indicated by toggling onboard LEDs.

The dataset comprises 185 hours of sensor measurements collected at 2 Hz from a PMSM deployed on a test bench at Paderborn University, featuring 69 measurement profiles where the motor is excited by hand-designed driving cycles that denote reference speed and torque through random walks in the speed-torque plane to imitate real-world driving conditions. The dataset includes sensor attributes: currents in d/q-coordinates (i_d, i_q), voltages in d/q-coordinates (u_d, u_q), motor speed, torque, ambient temperature, coolant temperature, and stator temperatures.

In this example the inference happens using the M0+ CPU itself and no hardware accelerator is used.

The LED indication for regression validation:
   - **Green**: Predicted temperature matches expected value
   - **Red**: Predicted temperature does not match expected value

## AI Model Information

| Property                 | Value                                 |
| ------------------------ | ------------------------------------- |
| Model Architecture       | 5-layer CNN + 2 pooling layers        |
| Number of Parameters     | ~25,022                               |
| Input Shape              | Time-series motor sensor data         |
| Output                   | Single temperature value (regression) |
| Quantization             | INT8                                  |

A 5-layer quantized convolutional network with 2 pooling layers and steady channel expansion (20→24→24→28→28).

In the PyTorch training framework, neural networks are trained with optimizations (for example, aggressive quantization) that target TI MCUs. After training, the neural networks are compiled by the [TI Neural Network Compiler](https://software-dl.ti.com/mctools/nnc/mcu/users_guide/index.html). Options passed to the compiler determine which of the following actions the generated inference library performs:
1. Hardware accelerated inference using TinyEngine™ NPU.
2. Software-only inference using the CPU on the MCU.

This example is built for the software-only inference mode using the CPU on MSPM0.

The output from the TI Neural Network Compiler is an artifacts directory that will contain: A header file (for example, tvmgen_default.h), and a library file (for example, model.a). This makes the output from the compiler easier to integrate with the project.

## AI Performance

| Metric                   | Value     |
| ------------------------ | --------- |
| Accuracy (R square)      | ~96.9     |
| Flash Usage(KB)          | 34.93     |
| RAM Usage(KB)            | 58.7      |

*Note: Performance metrics measured on MSPM0G3519 at 80 MHz.*

### Device Migration Recommendations

This project was developed for a superset device included in the MSPM0 LaunchPad. Please visit the [CCS User's Guide](https://software-dl.ti.com/msp430/esd/MSPM0-SDK/latest/docs/english/tools/ccs_ide_guide/doc_guide/doc_guide-srcs/ccs_ide_guide.html#sysconfig-project-migration) for information about migrating to other MSPM0 devices.

### Low-Power Recommendations

TI recommends to terminate unused pins by setting the corresponding functions to GPIO and configure the pins to output low or input with internal pullup/pulldown resistor.

SysConfig allows developers to easily configure unused pins by selecting **Board**→**Configure Unused Pins**.

For more information about jumper configuration to achieve low-power using the MSPM0 LaunchPad, please visit the [LP-MSPM0G3519 User's Guide](https://www.ti.com/lit/slav947).

## Hardware Requirements

1. LP-MSPM0G3519 LaunchPad

## Example Usage

1. **Hardware Setup**
   - Connect the LP-MSPM0G3519 LaunchPad to your PC via USB.
   - Ensure the LED connections are properly configured.

2. **Operation**
   - On startup, the application initializes the peripherals.
   - The pre-loaded test vector containing motor sensor data is fed to the AI model.
   - The model performs inference and predicts the motor temperature.
   - The predicted value is compared against the expected value:
     - **Green**: Prediction matches expected temperature
     - **Red**: Prediction does not match expected temperature

3. **Running the Example**
   - Compile, load, and run the application on your MSPM0 device.
   - Observe the LED indication for the prediction validation result.
   - To test different input data, modify the test vector in `motor_temperature_testvector.h`.

## Software Details

- **AI Inference**: Runs a pre-trained 5-layer CNN regression model using TVM runtime.
- **LED Indication**: Prediction accuracy is indicated by toggling onboard LEDs.
- **AI Model**: Model details are available on [Model Zoo](https://github.com/TexasInstruments/tinyml-tensorlab/tree/r1.3/tinyml-modelzoo)

## Notes

- This example uses a pre-loaded test vector for demonstration. For real-world applications, integrate with motor current, voltage, and temperature sensors.
- The model is optimized for low-power MCU deployment with aggressive quantization.
- This is a regression task that predicts continuous temperature values, not a classification task.

## References

- [ModelZoo Examples](https://github.com/TexasInstruments/tinyml-tensorlab/tree/main/tinyml-modelzoo/examples)
- MSPM0G3519 Technical Reference Manual [Link](https://www.ti.com/product/MSPM0G3519)
- [TI Neural Network Compiler Guide](https://software-dl.ti.com/mctools/nnc/mcu/users_guide/)
- TI Model Training Guide: [tinyml-tensorlab](https://github.com/TexasInstruments/tinyml-tensorlab/tree/main)
- EdgeAI Software Guide: SDK_INSTALL_DIR/docs/english/middleware/edgeAI/MSPM0_EdgeAI_User_Guide.html
- Electric Motor Temperature Dataset: [Kaggle](https://www.kaggle.com/datasets/wkirgsn/electric-motor-temperature)
