## Example Summary

This example demonstrates classification of single digit character images into one of 10 categories (0-9). The application receives image pixel data as a 2D array from the UART back channel, performs model inference, and sends the detected digit class back to a host GUI for display.

In this example the inference happens using the M0+ CPU itself and no hardware accelerator is used.

## AI Model Information

| Property | Value |
| --- | --- |
| Model Architecture | CNN |
| Number of Parameters | 60,000 |
| Input Shape | (1, 28, 28) |
| Output Classes | 10 |
| Quantization | INT8 |

In the PyTorch training framework, neural networks are trained with optimizations (for example, aggressive quantization) that target TI MCUs. After training, the neural networks are compiled by the [TI Neural Network Compiler](https://software-dl.ti.com/mctools/nnc/mcu/users_guide/index.html). Options passed to the compiler determine which of the following actions the generated inference library performs:
1. Hardware accelerated inference using TinyEngine™ NPU.
2. Software-only inference using the CPU on the MCU.

The output from the TI Neural Network Compiler is an artifacts directory that will contain: A header file (for example, tvmgen_default.h), and a library file (for example, model.a). This makes the output from the compiler easier to integrate with the project.

## AI Performance

| Metric | Value |
| --- | --- |
| Accuracy | ~99% |
| Flash Usage | 68 kB |
| RAM Usage | 8.1 kB |
| Inference Latency (CPU) | 89.81 ms |

*Note: Performance metrics measured on LP-MSPM0G5187.*

### Device Migration Recommendations

This project was developed for a superset device included in the MSPM0 LaunchPad. Please visit the [CCS User's Guide](https://software-dl.ti.com/msp430/esd/MSPM0-SDK/latest/docs/english/tools/ccs_ide_guide/doc_guide/doc_guide-srcs/ccs_ide_guide.html#sysconfig-project-migration) for information about migrating to other MSPM0 devices.

### Low-Power Recommendations

TI recommends to terminate unused pins by setting the corresponding functions to GPIO and configure the pins to output low or input with internal pullup/pulldown resistor.

SysConfig allows developers to easily configure unused pins by selecting **Board**→**Configure Unused Pins**.

For more information about jumper configuration to achieve low-power using the MSPM0 LaunchPad, please visit the [LP-MSPM0G3507 User's Guide](https://www.ti.com/lit/slau873).

## Hardware Requirements

1. LP-MSPM0G3507 LaunchPad
2. USB cable for UART communication with host PC

## Example Usage

1. **Hardware Setup**
   - Connect the LaunchPad to the host PC via USB.
   - No additional external hardware required.

2. **Operation**
   - On startup, the application initializes UART and waits for image data.
   - The host GUI sends a 28x28 pixel grayscale image over UART.
   - The application performs inference using the CNN model.
   - The predicted digit class (0-9) is sent back to the GUI.

3. **Running the Example**
   - Compile, load, and run the application on your MSPM0 device.
   - Launch the Character Recognition GUI (guide available at SDK_INSTALL_DIR/tools/character_recognition/README).
   - Draw a digit (0-9) and use the GUI to send the data for inference.
   - The inferred class will be displayed in the GUI upon receiving the result.

## Software Details

- **Data Acquisition**: Image data received via UART from host PC.
- **AI Inference**: Runs a pre-trained CNN model using TVM runtime.
- **Communication**: UART at 115200 baud for bidirectional communication.
- **AI Model**: Model details are available on [Model Zoo](https://github.com/TexasInstruments/tinyml-tensorlab/tree/r1.3/tinyml-modelzoo)

## Notes

- The input image should be a 28x28 pixel grayscale image.
- Best recognition results are achieved with centered, well-formed digits.
- The GUI tool is required to send image data and display results.

## References

- [Character Recognition ModelZoo Example](https://github.com/TexasInstruments/tinyml-tensorlab/tree/main/tinyml-modelzoo/examples/MNIST_image_classification/readme.md)
- MSPM0G3507 Technical Reference Manual [Link](https://www.ti.com/product/MSPM0G3507)
- [TI Neural Network Compiler Guide](https://software-dl.ti.com/mctools/nnc/mcu/users_guide/)
- TI Model Training Guide: [tinyml-tensorlab](https://github.com/TexasInstruments/tinyml-tensorlab/tree/main)
- [AI Model Architecture - CNN_LENET5](https://github.com/TexasInstruments/tinyml-tensorlab/blob/main/tinyml-modelzoo/tinyml_modelzoo/models/image.py) 
- EdgeAI Software Guide:[Link](https://dev.ti.com/tirex/explore/node?node=A__AKCnvqDed-Plz2JO5Umb3Q__MSPM0-SDK__a3PaaoK__LATEST)
- Character Recognition GUI: SDK_INSTALL_DIR/tools/character_recognition/README
- Tensorlab User Guide [Link](https://software-dl.ti.com/C2000/esd/mcu_ai/01_03_00/user_guide/index.html)
