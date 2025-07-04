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
 *  DO NOT EDIT - This file is generated for the LP_MSPM0C1106
 *  by the SysConfig tool.
 */
#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_LP_MSPM0C1106
#define CONFIG_MSPM0C1106

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



/* Defines for SPI_0 */
#define SPI_0_INST                                                         SPI0
#define SPI_0_INST_IRQHandler                                   SPI0_IRQHandler
#define SPI_0_INST_INT_IRQN                                       SPI0_INT_IRQn
#define GPIO_SPI_0_PICO_PORT                                              GPIOA
#define GPIO_SPI_0_PICO_PIN                                      DL_GPIO_PIN_10
#define GPIO_SPI_0_IOMUX_PICO                                   (IOMUX_PINCM15)
#define GPIO_SPI_0_IOMUX_PICO_FUNC                   IOMUX_PINCM15_PF_SPI0_PICO
#define GPIO_SPI_0_POCI_PORT                                              GPIOA
#define GPIO_SPI_0_POCI_PIN                                       DL_GPIO_PIN_9
#define GPIO_SPI_0_IOMUX_POCI                                   (IOMUX_PINCM14)
#define GPIO_SPI_0_IOMUX_POCI_FUNC                   IOMUX_PINCM14_PF_SPI0_POCI
/* GPIO configuration for SPI_0 */
#define GPIO_SPI_0_SCLK_PORT                                              GPIOA
#define GPIO_SPI_0_SCLK_PIN                                      DL_GPIO_PIN_25
#define GPIO_SPI_0_IOMUX_SCLK                                   (IOMUX_PINCM43)
#define GPIO_SPI_0_IOMUX_SCLK_FUNC                   IOMUX_PINCM43_PF_SPI0_SCLK
#define GPIO_SPI_0_CS_PORT                                                GPIOA
#define GPIO_SPI_0_CS_PIN                                         DL_GPIO_PIN_8
#define GPIO_SPI_0_IOMUX_CS                                     (IOMUX_PINCM13)
#define GPIO_SPI_0_IOMUX_CS_FUNC                      IOMUX_PINCM13_PF_SPI0_CS0



/* Defines for DMA_CH0 */
#define DMA_CH0_CHAN_ID                                                      (0)
#define SPI_0_INST_DMA_TRIGGER_0                              (DMA_SPI0_TX_TRIG)
/* Defines for DMA_CH1 */
#define DMA_CH1_CHAN_ID                                                      (1)
#define SPI_0_INST_DMA_TRIGGER_1                              (DMA_SPI0_RX_TRIG)


/* Port definition for Pin Group GPIO_LEDS */
#define GPIO_LEDS_PORT                                                   (GPIOA)

/* Defines for USER_LED_1: GPIOA.0 with pinCMx 1 on package pin 1 */
#define GPIO_LEDS_USER_LED_1_PIN                                 (DL_GPIO_PIN_0)
#define GPIO_LEDS_USER_LED_1_IOMUX                                (IOMUX_PINCM1)
/* Defines for USER_TEST: GPIOA.15 with pinCMx 27 on package pin 30 */
#define GPIO_LEDS_USER_TEST_PIN                                 (DL_GPIO_PIN_15)
#define GPIO_LEDS_USER_TEST_IOMUX                                (IOMUX_PINCM27)

/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);
void SYSCFG_DL_SPI_0_init(void);
void SYSCFG_DL_DMA_init(void);



#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
