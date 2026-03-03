## Example Summary

This example demonstrates PIR (Passive Infrared) motion detection with live preview on EdgeAI Studio using MSPM0 devices. The application captures PIR sensor data via the ADC, extracts spectral features using FFT-based feature extraction, and performs inference using the NPU to classify motion into three categories:

- **Human Motion** - RED LED
- **Background Motion** - BLUE LED
- **Dog Motion** - GREEN LED

The classification results are streamed to EdgeAI Studio for real-time visualization.

This example uses hardware-accelerated inference via the Neural Network Processing Unit (NPU).

### Device Migration Recommendations
This project was developed for a superset device included in the MSPM0 LaunchPad. Please
visit the [CCS User's Guide](https://software-dl.ti.com/msp430/esd/MSPM0-SDK/latest/docs/english/tools/ccs_ide_guide/doc_guide/doc_guide-srcs/ccs_ide_guide.html#sysconfig-project-migration)
for information about migrating to other MSPM0 devices.

### Low-Power Recommendations
TI recommends to terminate unused pins by setting the corresponding functions to
GPIO and configure the pins to output low or input with internal
pullup/pulldown resistor.

SysConfig allows developers to easily configure unused pins by selecting **Board**→**Configure Unused Pins**.

For more information about jumper configuration to achieve low-power using the
MSPM0 LaunchPad, please visit the LP-MSPM0G5187 User's Guide.

## Example Usage

To initiate the live preview:
1. Configure the serial port in EdgeAI Studio. Confirm that the port and baud rate (115200 bps) are selected to match the device's UART baudrate.
2. Before starting, check the status bar in EdgeAI studio for the message "**Hardware connected**"
3. Navigate to the **Live preview** tab
4. Click **Start Preview** to begin viewing the real-time model output.

## References

- [PIR Detection ModelZoo Example](https://github.com/TexasInstruments/tinyml-tensorlab/tree/main/tinyml-modelzoo/examples/pir_detection)
- MSPM0G5187 Technical Reference Manual [Link](https://www.ti.com/product/MSPM0G5187)
- [TI Neural Network Compiler Guide](https://software-dl.ti.com/mctools/nnc/mcu/users_guide/)
- TI Model Training Guide: [tinyml-tensorlab](https://github.com/TexasInstruments/tinyml-tensorlab/tree/main)
- EdgeAI Software Guide: SDK_INSTALL_DIR/docs/english/middleware/edgeAI/MSPM0_EdgeAI_User_Guide.html