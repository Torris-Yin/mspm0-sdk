## Example Summary

This example demonstrates time series classification of waveform signals into one of three categories: sawtooth wave, sine wave, or square wave. The application uses pre-defined test data, performs feature extraction, and runs model inference to classify the waveform type. The predicted class is indicated by toggling onboard LEDs.

In this example the inference happens using the M0+ CPU itself and no hardware accelerator is used.

The class to LED color mapping is shown below:
   - **Red**: Class 0 (Sawtooth)
   - **Green**: Class 1 (Sine)
   - **Blue**: Class 2 (Square)

In the PyTorch training framework, neural networks are trained with optimizations (for example, aggressive quantization) that target TI MCUs. After training, the neural networks are compiled by the [TI Neural Network Compiler](https://software-dl.ti.com/mctools/nnc/mcu/users_guide/index.html). Options passed to the compiler determine which of the following actions the generated inference library performs:
1. Hardware accelerated inference using TinyEngine™ NPU.
2. Software-only inference using the CPU on the MCU.

The output from the TI Neural Network Compiler is an artifacts directory that will contain: A header file (for example, tvmgen_default.h), and a library file (for example, model.a). This makes the output from the compiler easier to integrate with the project.

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
   - No external hardware connections required for this example.
   - Ensure LED jumpers (J5, J6, J7) are populated to enable LED indication.

2. **Operation**
   - On startup, the application initializes the peripherals.
   - Pre-defined waveform test data is processed through the AI model.
   - The model classifies the waveform type (sawtooth, sine, or square).
   - The corresponding LED is toggled to indicate the predicted class.

3. **Running the Example**
   - Compile, load, and run the application on your MSPM0 device.
   - By default, the example builds for a sinusoidal wave (Green LED).
   - Modify the `#define` in main.c to test other waveform classes.

## Software Details

- **Feature Extraction**: Pre-processed waveform data is used as input.
- **AI Inference**: Runs a pre-trained CNN model using TVM runtime.
- **LED Indication**: Classification result is indicated by toggling onboard LEDs.
- **AI Model**: Model details are available on [Model Zoo](https://github.com/TexasInstruments/tinyml-tensorlab/tree/r1.3/tinyml-modelzoo)

## Notes

- This is a simple "Hello World" example to demonstrate the AI inference workflow.
- The example uses built-in test data rather than real-time sensor input.
- Modify the test data selection in main.c to experiment with different waveform types.

## References

- [TimeSeries ModelZoo Example](https://github.com/TexasInstruments/tinyml-tensorlab/blob/main/tinyml-modelzoo/examples/generic_timeseries_classification)
- MSPM0G3507 Technical Reference Manual [Link](https://www.ti.com/product/MSPM0G3507)
- [TI Neural Network Compiler Guide](https://software-dl.ti.com/mctools/nnc/mcu/users_guide/)
- TI Model Training Guide: [tinyml-tensorlab](https://github.com/TexasInstruments/tinyml-tensorlab/tree/main)
- [AI Model Architecture - CNN_TS_GEN_BASE_13K_NPU](https://github.com/TexasInstruments/tinyml-tensorlab/blob/main/tinyml-modelzoo/tinyml_modelzoo/models/classification.py) 
- EdgeAI Software Guide:[Link](https://dev.ti.com/tirex/explore/node?node=A__AKCnvqDed-Plz2JO5Umb3Q__MSPM0-SDK__a3PaaoK__LATEST)
- Tensorlab User Guide [Link](https://software-dl.ti.com/C2000/esd/mcu_ai/01_03_00/user_guide/index.html)
