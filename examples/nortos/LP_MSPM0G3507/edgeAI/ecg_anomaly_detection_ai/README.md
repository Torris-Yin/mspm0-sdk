## Example Summary

This example demonstrates an ECG (Electrocardiogram) classification application using an AI model on MSPM0G3507 microcontroller. The system collects ECG data from an AFE1594 analog front-end, processes the signal, and performs inference using a pre-trained neural network model. The detected ECG class is indicated by toggling onboard LEDs.

In this example the inference happens using the M0+ CPU itself and no hardware accelerator is used.

The class to LED color mapping is shown below:
   - **Green**: Class 0 (Normal)
   - **Yellow**: Class 1 (Mild)
   - **Red**: Class 2 (Other)

## AI Model Information

| Property | Value |
| --- | --- |
| Model Architecture | CNN |
| Number of Parameters | ~55k |
| Input Shape | (1,1,2500,1) |
| Output Classes | 3 (Normal, Mild, Other) |
| Quantization | INT8 |

## AI Performance

| Metric | Value |
| --- | --- |
| Accuracy | ~97% |
| Flash Usage | 64.8 kB |
| RAM Usage | 18.4 kB |

*Note: Performance metrics measured on LP-MSPM0G5187.*

### Low-Power Recommendations

Terminate unused pins by configuring them as GPIO outputs low or inputs with
internal pull-up/pull-down resistors. Use SysConfig to easily configure unused pins.

SysConfig allows developers to easily configure unused pins by selecting **Board**→**Configure Unused Pins**.

For more information about jumper configuration to achieve low-power using the
MSPM0 LaunchPad, please visit the [LP-MSPM0G3507 User's Guide](https://www.ti.com/lit/slau873).

## Hardware requirements

1. MSPM0G3507 microcontroller
2. AFE1594 Analog Front-End for ECG acquisition
3. ECG electrodes and connectors

## Example Usage

1. **Hardware Setup**
   - Connect the AFE1594 analog front-end to the MSPM0 SPI interface:
     - Controller SCLK (PB18) -> AFE SCLK
     - Controller MOSI (PB17) -> AFE MOSI
     - Controller MISO (PB19) <- AFE MISO
     - Controller CS (PA0) -> AFE CS
   - Connect AFE control pins:
     - Reset (PA15) -> AFE Reset
     - Power Down (PA26) -> AFE PDN
   - Connect the AFE interrupt pin (Data Ready) to the designated GPIO interrupt pin (PA8).
   - Ensure LEDs are connected to the appropriate GPIO pins for class indication.
   - Connect ECG electrodes to the AFE1594 inputs according to the AFE1594 documentation.

2. **Operation**
   - On startup, the application initializes the AFE and peripherals.
   - The AFE1594 collects ECG signal data.
   - When new data is ready, an interrupt triggers data acquisition.
   - The raw ECG data is processed and fed to the AI model for inference.
   - The output class is determined and stored in a circular buffer.
   - The LED corresponding to the most frequently detected class in the buffer is toggled:
     - **LED1 (Green)**: Class 0 (Normal)
     - **LED2 (Yellow)**: Class 1 (Mild)
     - **LED3 (Red)**: Class 2 (Other)

3. **Running the Example**
   - Compile, load, and run the application on your MSPM0 device.
   - The system will continuously monitor ECG signals and indicate detected classes via LEDs.

## Software Details

- **Data Acquisition**: Efficient SPI data collection from AFE1594.
- **Signal Processing**: Raw ECG signal processing for feature extraction.
- **AI Inference**: Runs a pre-trained model using TVM runtime on the NPU.
- **Interrupt Handling**: Efficient data collection via GPIO interrupts.
- **LED Indication**: Classification result is indicated by toggling onboard LEDs.
- **AI model used**: Model details can be found in TensorLab repository.

## Notes

- Ensure the AFE1594 is properly powered and connected.
- Proper electrode placement is critical for accurate ECG measurements.
- Adjust LED and GPIO pin assignments as needed for your hardware.

## References

- [ECG ModelZoo Example](https://github.com/TexasInstruments/tinyml-tensorlab/tree/main/tinyml-modelzoo/examples/ecg_classification)
- MSPM0G3507 Technical Reference Manual [Link](https://www.ti.com/product/MSPM0G3507)
- [TI Neural Network Compiler Guide](https://software-dl.ti.com/mctools/nnc/mcu/users_guide/)
- TI Model Training Guide: [tinyml-tensorlab](https://github.com/TexasInstruments/tinyml-tensorlab/tree/main)
- [AI Model Architecture - CNN_TS_GEN_BASE_55K_NPU](https://github.com/TexasInstruments/tinyml-tensorlab/blob/main/tinyml-modelzoo/tinyml_modelzoo/models/classification.py) 
- EdgeAI Software Guide:[Link](https://dev.ti.com/tirex/explore/node?node=A__AKCnvqDed-Plz2JO5Umb3Q__MSPM0-SDK__a3PaaoK__LATEST)
- Tensorlab User Guide [Link](https://software-dl.ti.com/C2000/esd/mcu_ai/01_03_00/user_guide/index.html)
- For more information on the AFE1594, refer to the TI datasheet and user guides.