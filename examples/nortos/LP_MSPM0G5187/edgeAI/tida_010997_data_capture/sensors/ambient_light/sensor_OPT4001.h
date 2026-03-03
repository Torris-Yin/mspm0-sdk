/*
 * Copyright (c) 2025, Texas Instruments Incorporated
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

#ifndef SENSOR_OPT4001_H_
#define SENSOR_OPT4001_H_

#include <stdint.h>
#include <stdbool.h>
#include <ti/driverlib/m0p/dl_core.h>

/**
 * @brief OPT4001 I2C address (fixed for boosterpack)
 */
#define I2C_TARGET_ADDR_OPT4001         (0x44)

/**
 * @brief OPT4001 Device ID
 */
#define OPT4001_DEVICE_ID               (0x121)

/**
 * @brief OPT4001 operating modes
 */
typedef enum {
    OPT4001_MODE_SHUTDOWN   = 0,  /**< Power-down mode */
    OPT4001_MODE_ONESHOT    = 1,  /**< One-shot measurement mode */
    OPT4001_MODE_CONTINUOUS = 3   /**< Continuous conversion mode */
} opt4001_mode_t;

/**
 * @brief OPT4001 conversion time settings
 */
typedef enum {
    OPT4001_CONV_TIME_600US  = 0x0,  /**< 600us conversion time */
    OPT4001_CONV_TIME_1MS    = 0x1,  /**< 1ms conversion time */
    OPT4001_CONV_TIME_1_8MS  = 0x2,  /**< 1.8ms conversion time */
    OPT4001_CONV_TIME_3_4MS  = 0x3,  /**< 3.4ms conversion time */
    OPT4001_CONV_TIME_6_5MS  = 0x4,  /**< 6.5ms conversion time */
    OPT4001_CONV_TIME_12_7MS = 0x5,  /**< 12.7ms conversion time */
    OPT4001_CONV_TIME_25MS   = 0x6,  /**< 25ms conversion time */
    OPT4001_CONV_TIME_50MS   = 0x7,  /**< 50ms conversion time */
    OPT4001_CONV_TIME_100MS  = 0x8,  /**< 100ms conversion time */
    OPT4001_CONV_TIME_200MS  = 0x9,  /**< 200ms conversion time */
    OPT4001_CONV_TIME_400MS  = 0xA,  /**< 400ms conversion time */
    OPT4001_CONV_TIME_800MS  = 0xB   /**< 800ms conversion time */
} opt4001_conv_time_t;

/**
 * @brief OPT4001 range settings
 */
typedef enum {
    OPT4001_RANGE_0 = 0x0,  /**< Range 0: ~419 lux */
    OPT4001_RANGE_1 = 0x1,  /**< Range 1: ~839 lux */
    OPT4001_RANGE_2 = 0x2,  /**< Range 2: ~1678 lux */
    OPT4001_RANGE_3 = 0x3,  /**< Range 3: ~3355 lux */
    OPT4001_RANGE_4 = 0x4,  /**< Range 4: ~6711 lux */
    OPT4001_RANGE_5 = 0x5,  /**< Range 5: ~13422 lux */
    OPT4001_RANGE_6 = 0x6,  /**< Range 6: ~26844 lux */
    OPT4001_RANGE_7 = 0x7,  /**< Range 7: ~53687 lux */
    OPT4001_RANGE_8 = 0x8,  /**< Range 8: ~107374 lux */
    OPT4001_RANGE_AUTO = 0xC /**< Automatic range selection */
} opt4001_range_t;

/**
 * @brief OPT4001 interrupt configuration
 */
typedef enum {
    OPT4001_INT_CFG_THRESHOLDS = 0,  /**< INT pin indicates threshold events */
    OPT4001_INT_CFG_CONVERSION = 1,  /**< INT pin pulses after every conversion */
    OPT4001_INT_CFG_FIFO_FULL  = 3   /**< INT pin pulses when FIFO is full (4 conversions) */
} opt4001_int_cfg_t;

/**
 * @brief OPT4001 latch settings
 */
typedef enum {
    OPT4001_LATCH_TRANSPARENT = 0,  /**< Transparent hysteresis mode */
    OPT4001_LATCH_WINDOW     = 1   /**< Latched window mode */
} opt4001_latch_t;

/**
 * @brief OPT4001 measurement data structure
 */
typedef struct {
    float lux;         /**< Light level in lux */
    uint8_t exponent;  /**< Raw exponent value */
    uint32_t mantissa; /**< Raw mantissa value */
    bool valid;        /**< Flag indicating if the data is valid */
} opt4001_data_t;

/**
 * @brief OPT4001 configuration structure
 */
typedef struct {
    opt4001_mode_t mode;          /**< Operating mode */
    opt4001_conv_time_t convTime; /**< Conversion time */
    opt4001_range_t range;        /**< Range setting */
    opt4001_latch_t latch;        /**< Latch mode */
    opt4001_int_cfg_t intCfg;    /**< Interrupt configuration */
    bool intPol;                 /**< Interrupt polarity (true = active high) */
    bool quickWake;              /**< Quick wake-up in one-shot mode */
} opt4001_config_t;

/**
 * @brief OPT4001 register addresses
 */
#define OPT4001_REG_RESULT        0x00  /**< Result register */
#define OPT4001_REG_CONFIG        0x0A  /**< Configuration register */
#define OPT4001_REG_INT_CONFIG    0x0B  /**< Interrupt configuration register */
#define OPT4001_REG_STATUS        0x0C  /**< Status register */
#define OPT4001_REG_DEVICE_ID     0x11  /**< Device ID register */

/**
 * @brief Error codes for OPT4001 functions
 */
#define OPT4001_OK                  0    /**< Success */
#define OPT4001_ERR_I2C            -1    /**< I2C communication error */
#define OPT4001_ERR_INVALID_DATA   -2    /**< Invalid data */
#define OPT4001_ERR_TIMEOUT        -3    /**< Operation timed out */

/**
 * @brief Initialize the OPT4001 sensor
 *
 * @param config Pointer to configuration structure
 * @return int 0 on success, error code otherwise
 */
int opt4001_init(const opt4001_config_t *config);

/**
 * @brief Configure the OPT4001 sensor
 *
 * @param config Pointer to configuration structure
 * @return int 0 on success, error code otherwise
 */
int opt4001_configure(const opt4001_config_t *config);

/**
 * @brief Get light sensor data
 *
 * @param data Pointer to data structure to store the reading
 * @return int 0 on success, error code otherwise
 */
int opt4001_get_data(opt4001_data_t *data);

/**
 * @brief Trigger a single measurement (only in one-shot mode)
 *
 * @return int 0 on success, error code otherwise
 */
int opt4001_trigger_measurement(void);


#endif /* SENSOR_OPT4001_H_ */
