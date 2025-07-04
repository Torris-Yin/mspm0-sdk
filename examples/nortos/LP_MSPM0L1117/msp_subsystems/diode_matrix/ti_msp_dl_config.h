/*
 * Copyright (c) 2023, Texas Instruments Incorporated - http://www.ti.com
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
 *  ============ ti_msp_dl_config.h =============
 *  Configured MSPM0 DriverLib module declarations
 *
 *  DO NOT EDIT - This file is generated for the LP_MSPM0L1117
 *  by the SysConfig tool.
 */
#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_LP_MSPM0L1117
#define CONFIG_MSPM0L1117

#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform all required MSP DL initialization
 *
 *  This function should be called once at a point before any use of
 *  MSP DL.
 */


/* clang-format off */

#define POWER_STARTUP_DELAY                                                (16)


#define CPUCLK_FREQ                                                     32000000



/* Defines for LED_STATE */
#define LED_STATE_INST                                                   (TIMG0)
#define LED_STATE_INST_IRQHandler                               TIMG0_IRQHandler
#define LED_STATE_INST_INT_IRQN                                 (TIMG0_INT_IRQn)
#define LED_STATE_INST_LOAD_VALUE                                       (19999U)




/* Port definition for Pin Group COLUMN */
#define COLUMN_PORT                                                      (GPIOA)

/* Defines for COLUMN_1: GPIOA.5 with pinCMx 10 on package pin 11 */
#define COLUMN_COLUMN_1_PIN                                      (DL_GPIO_PIN_5)
#define COLUMN_COLUMN_1_IOMUX                                    (IOMUX_PINCM10)
/* Defines for COLUMN_2: GPIOA.4 with pinCMx 9 on package pin 10 */
#define COLUMN_COLUMN_2_PIN                                      (DL_GPIO_PIN_4)
#define COLUMN_COLUMN_2_IOMUX                                     (IOMUX_PINCM9)
/* Defines for COLUMN_3: GPIOA.3 with pinCMx 8 on package pin 9 */
#define COLUMN_COLUMN_3_PIN                                      (DL_GPIO_PIN_3)
#define COLUMN_COLUMN_3_IOMUX                                     (IOMUX_PINCM8)
/* Port definition for Pin Group ROW */
#define ROW_PORT                                                         (GPIOA)

/* Defines for ROW_1: GPIOA.12 with pinCMx 34 on package pin 27 */
#define ROW_ROW_1_PIN                                           (DL_GPIO_PIN_12)
#define ROW_ROW_1_IOMUX                                          (IOMUX_PINCM34)
/* Defines for ROW_2: GPIOA.13 with pinCMx 35 on package pin 28 */
#define ROW_ROW_2_PIN                                           (DL_GPIO_PIN_13)
#define ROW_ROW_2_IOMUX                                          (IOMUX_PINCM35)
/* Defines for ROW_3: GPIOA.27 with pinCMx 60 on package pin 47 */
#define ROW_ROW_3_PIN                                           (DL_GPIO_PIN_27)
#define ROW_ROW_3_IOMUX                                          (IOMUX_PINCM60)

/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);
void SYSCFG_DL_LED_STATE_init(void);



#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
