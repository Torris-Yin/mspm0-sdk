## Example Summary

This is the test code example for the high-resolution, low-latency and low-power absolute inductive angle encoder reference design TIDA-010961.

### Device Migration Recommendations
This project was developed for a superset device included in the LP_MSPM0G3507 LaunchPad. Please
visit the [CCS User's Guide](https://software-dl.ti.com/msp430/esd/MSPM0-SDK/latest/docs/english/tools/ccs_ide_guide/doc_guide/doc_guide-srcs/ccs_ide_guide.html#manual-migration)
for information about migrating to other MSPM0 devices.

### Low-Power Recommendations
TI recommends to terminate unused pins by setting the corresponding functions to
GPIO and configure the pins to output low or input with internal
pullup/pulldown resistor.

SysConfig allows developers to easily configure unused pins by selecting **Board**→**Configure Unused Pins**.

For more information about jumper configuration to achieve low-power using the
MSPM0 LaunchPad, please visit the [LP-MSPM0G3507 User's Guide](https://www.ti.com/lit/slau873).

## Example Usage

### List of key variables
Here is a list of key variables to monitor the absolute mechanical angle and the electrical angles in the Code Composer expression window.

Variable: angle_abs
Description: Absolute mechanical angle in IQ24 format
Scaling: 0 to 1.0, scaling factor 1/360 degree

Variable: angle_abs_IQ21
Description: Absolute mechanical angle in IQ21 format
Scaling: 0 to 1.0, scaling factor 1/360 degree

Variable: angle_abs_360_f
Description: Absolute mechanical angle in 32-bit float format
Scaling: 0 to 360 degrees

Variable: fine_angle_abs
Description: Electrical angle of the outer sense coil with 16 electrical periods per revolution in IQ24 format
Scaling: 0 to 1.0, scaling factor 1/360 degree

Variable: coarse_angle_abs
Description: Electrical angle of the inner sense coil with 15 electrical periods per revolution in IQ24 format
Scaling: 0 to 1.0, scaling factor 1/360 degree

For more information on the software flowchart and peripheral configurations used in the application code example refer to the TIDA-010961 design guide at https://www.ti.com/lit/ug/tiduff8/tiduff8.pdf, chapter 3.2 and chapter 3.3.

For more information on the reference design refer to the TIDA-010961 reference design landing page at https://www.ti.com/tool/TIDA-010961
