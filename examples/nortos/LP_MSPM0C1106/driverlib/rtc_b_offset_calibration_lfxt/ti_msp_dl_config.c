/*
 * Copyright (c) 2023, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ============ ti_msp_dl_config.c =============
 *  Configured MSPM0 DriverLib module definitions
 *
 *  DO NOT EDIT - This file is generated for the LP_MSPM0C1106
 *  by the SysConfig tool.
 */

#include "ti_msp_dl_config.h"

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform any initialization needed before using any board APIs
 */
SYSCONFIG_WEAK void SYSCFG_DL_init(void)
{
    SYSCFG_DL_initPower();
    SYSCFG_DL_GPIO_init();
    /* Module-Specific Initializations*/
    SYSCFG_DL_SYSCTL_init();
    SYSCFG_DL_RTC_B_init();
}

SYSCONFIG_WEAK void SYSCFG_DL_initPower(void)
{
    DL_GPIO_reset(GPIOA);
    DL_GPIO_reset(GPIOB);

    DL_GPIO_enablePower(GPIOA);
    DL_GPIO_enablePower(GPIOB);
    delay_cycles(POWER_STARTUP_DELAY);
}

SYSCONFIG_WEAK void SYSCFG_DL_GPIO_init(void)
{

    DL_GPIO_initDigitalInputFeatures(GPIO_SWITCHES_USER_SWITCH_1_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(GPIO_SWITCHES_USER_SWITCH_2_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_setLowerPinsPolarity(GPIO_SWITCHES_PORT, DL_GPIO_PIN_14_EDGE_RISE_FALL);
    DL_GPIO_setUpperPinsPolarity(GPIO_SWITCHES_PORT, DL_GPIO_PIN_18_EDGE_RISE_FALL);
    DL_GPIO_clearInterruptStatus(GPIO_SWITCHES_PORT, GPIO_SWITCHES_USER_SWITCH_1_PIN |
		GPIO_SWITCHES_USER_SWITCH_2_PIN);
    DL_GPIO_enableInterrupt(GPIO_SWITCHES_PORT, GPIO_SWITCHES_USER_SWITCH_1_PIN |
		GPIO_SWITCHES_USER_SWITCH_2_PIN);

    /* Configure RTC_B OUT pin */
    DL_GPIO_initPeripheralOutputFunction(
        GPIO_RTC_B_IOMUX_RTCOUT, GPIO_RTC_B_IOMUX_RTCOUT_FUNC);

}



SYSCONFIG_WEAK void SYSCFG_DL_SYSCTL_init(void)
{

	//Low Power Mode is configured to be SLEEP0
    DL_SYSCTL_setBORThreshold(DL_SYSCTL_BOR_THRESHOLD_LEVEL_0);

    
	DL_SYSCTL_setSYSOSCFreq(DL_SYSCTL_SYSOSC_FREQ_BASE);
	/* Set default configuration */
	DL_SYSCTL_disableHFXT();

}


static const DL_RTC_B_Calendar gRTCCalendarConfig = {
		.seconds    = 58,   /* Seconds = 58 */
		.minutes    = 32,   /* Minute = 32 */
		.hours      = 10,   /* Hour = 10 */
		.dayOfWeek  = 1,    /* Day of week = 1 (Monday) */
		.dayOfMonth = 5,    /* Day of month = 5*/
		.month      = 4,    /* Month = 4 (April) */
		.year       = 2022, /* Year = 2022 */
};

SYSCONFIG_WEAK void SYSCFG_DL_RTC_B_init(void)
{
	/* Initialize RTC Calendar */
	DL_RTC_B_initCalendar(RTC_B , gRTCCalendarConfig, DL_RTC_B_FORMAT_BINARY);

	/* Configure the Crystal Offset Error Calibration */
	DL_RTC_B_setOffsetCalibrationFrequency(RTC_B, DL_RTC_B_OFFSET_CALIBRATION_FREQUENCY_512);
}

