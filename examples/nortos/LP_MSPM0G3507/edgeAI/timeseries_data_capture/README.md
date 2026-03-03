## Example Summary

This example demonstrates the process of capturing timeseries data from MSPM0 devices using CCStudio™ Edge AI Studio. This example configures the ADC at 5kHz sampling rate, converts the samples to Q15 format, and streams the data to CCStudio™ Edge AI Studio for data acquisition.

### Device Migration Recommendations
This project was developed for a superset device included in the MSPM0 LaunchPad. Please
visit the [CCS User's Guide](https://software-dl.ti.com/msp430/esd/MSPM0-SDK/latest/docs/english/tools/ccs_ide_guide/doc_guide/doc_guide-srcs/ccs_ide_guide.html#sysconfig-project-migration)
for information about migrating to other MSPM0 devices.

### Low-Power Recommendations
TI recommends to terminate unused pins by setting the corresponding functions to
GPIO and configure the pins to output low or input with internal
pullup/pulldown resistor.

SysConfig allows developers to easily configure unused pins by selecting **Board**→**Configure Unused Pins**.

For more information about jumper configuration to achieve low-power using the MSPM0 LaunchPad, please visit the [LP-MSPM0G3507 User's Guide](https://www.ti.com/lit/slau873).

## Hardware Requirements

1. LP-MSPM0G3507 LaunchPad
2. Function Generator

## Example Usage

To capture data:
1. Configure the serial port in CCStudio™ Edge AI Studio. Confirm that the port and baud rate (115200 bps) are selected to match the device's UART baudrate.
2. Before capturing, check the status bar in CCStudio™ Edge AI Studio for the message "**Hardware connected**"
3. Navigate to the **Capture** tab
    1. Specify the number of samples to capture and a label for the dataset.
    2. Click **Start Capture** to start data acquisition.

## References

- [ModelZoo Examples](https://github.com/TexasInstruments/tinyml-tensorlab/tree/main/tinyml-modelzoo/examples)
- MSPM0G3507 Technical Reference Manual [Link](https://www.ti.com/product/MSPM0G3507)
- [TI Neural Network Compiler Guide](https://software-dl.ti.com/mctools/nnc/mcu/users_guide/)
- TI Model Training Guide: [tinyml-tensorlab](https://github.com/TexasInstruments/tinyml-tensorlab/tree/main)
- EdgeAI Software Guide: SDK_INSTALL_DIR/docs/english/middleware/edgeAI/MSPM0_EdgeAI_User_Guide.html
