## Example Summary

This example demonstrates the blank verify command to verify if a flash word is blank. The example does this by successfully verifying a blank word, and returning a failure when a flash word is not blank. A blank flash word is defined as a flash word which has been successfully erased with the ERASE command and not yet programmed away from that non-erased state with the PROGRAM command.

## Peripherals, Pin Functions, MCU Pins, Launchpad Pins
| Peripheral | Function | MCU Pin | Launchpad Pin | Launchpad Settings |
| --- | --- | --- | --- | --- |
| GPIOA | Open-Drain Output | PA0 | Red LED1 | Populate Jumper(s): J4[1:2] |
| GPIOA | Standard Output | PA15 | J4_38 |  |
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
The flash operations will start automatically when the example is run.
The application will hit a breakpoint if there is a failure.
LED0 will blink after all flash operations are completed successfully.
