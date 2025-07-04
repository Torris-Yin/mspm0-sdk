## Example Summary

Toggles three GPIO pins using HW toggle register.

## Peripherals, Pin Functions, MCU Pins, Launchpad Pins
| Peripheral | Function | MCU Pin | Launchpad Pin | Launchpad Settings |
| --- | --- | --- | --- | --- |
| GPIOA | Standard Output | PB20 | LED2 Blue | Populate Jumper(s): J7[1:2] |
| GPIOA | Standard Output | PB24 | LED2 Red | Populate Jumper(s): J8[1:2] |
| GPIOA | Standard Output | PA30 | LED2 Green | Populate Jumper(s): J9[1:2] |
| GPIOA | Standard Output | PB19 | J1_28 |  |
| SYSCTL |  |  |  |  |
| EVENT |  |  |  |  |
| BOARD | Debug Clock | PA20 | J14_4 |  |
| BOARD | Debug Data In Out | PA19 | J14_2 |  |

### Device Migration Recommendations
This project was developed for a superset device included in the LaunchPad. Please
visit the [CCS User's Guide](https://software-dl.ti.com/msp430/esd/MSPM0-SDK/latest/docs/english/tools/ccs_ide_guide/doc_guide/doc_guide-srcs/ccs_ide_guide.html#sysconfig-project-migration)
for information about migrating to other MSPM0 devices.

### Low-Power Recommendations
TI recommends to terminate unused pins by setting the corresponding functions to
GPIO and configure the pins to output low or input with internal
pullup/pulldown resistor.

SysConfig allows developers to easily configure unused pins by selecting **Board**→**Configure Unused Pins**.

For more information about jumper configuration to achieve low-power using the
MSPM0 LaunchPad, please visit the LP-MSPM0C1106 User's Guide.

## Example Usage
Compile, load and run the example.
RGB LEDs will toggle with red being opposite of blue and green.
