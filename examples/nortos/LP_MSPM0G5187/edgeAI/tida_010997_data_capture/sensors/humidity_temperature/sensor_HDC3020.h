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

#ifndef SENSOR_HDC3020_H_
#define SENSOR_HDC3020_H_

#include <stdint.h>
#include <stdbool.h>
#include <ti/driverlib/m0p/dl_core.h>

/**
 * @brief HDC3020 I2C address (fixed for boosterpack)
 */
#define I2C_TARGET_ADDR_HDC3020         (0x46)

/**
 * @brief HDC3020 low power modes
 */
typedef enum {
    HDC3020_LPM_0 = 0,  /**< Low Power Mode 0: Lowest noise, 12.5ms conversion */
    HDC3020_LPM_1 = 1,  /**< Low Power Mode 1: Medium, 7.5ms conversion */
    HDC3020_LPM_2 = 2,  /**< Low Power Mode 2: Medium, 5.0ms conversion */
    HDC3020_LPM_3 = 3   /**< Low Power Mode 3: Lowest power, 3.7ms conversion */
} hdc3020_lpm_t;

/**
 * @brief HDC3020 measurement rates for auto measurement mode
 */
typedef enum {
    HDC3020_RATE_0_5HZ = 0,  /**< 0.5 measurements per second */
    HDC3020_RATE_1HZ   = 1,  /**< 1 measurement per second */
    HDC3020_RATE_2HZ   = 2,  /**< 2 measurements per second */
    HDC3020_RATE_4HZ   = 3,  /**< 4 measurements per second */
    HDC3020_RATE_10HZ  = 4   /**< 10 measurements per second */
} hdc3020_rate_t;

/**
 * @brief HDC3020 measurement mode
 */
typedef enum {
    HDC3020_MODE_TRIGGER_ON_DEMAND = 0,  /**< Single measurement on command */
    HDC3020_MODE_AUTO_MEASUREMENT  = 1   /**< Continuous measurements at specified rate */
} hdc3020_mode_t;

/**
 * @brief HDC3020 sensor type selection
 */
typedef enum {
    HDC3020_SENSOR_BOTH = 0,     /**< Both temperature and humidity sensors */
    HDC3020_SENSOR_TEMPERATURE,  /**< Temperature sensor only */
    HDC3020_SENSOR_HUMIDITY      /**< Humidity sensor only */
} hdc3020_sensor_t;

/**
 * @brief HDC3020 measurement data structure
 */
typedef struct {
    uint16_t temperature;  /**< Temperature in degrees Celsius */
    uint16_t humidity;     /**< Relative humidity in percent */
    bool valid;         /**< Flag indicating if the data is valid */
} hdc3020_data_t;

/**
 * @brief HDC3020 configuration structure
 */
typedef struct {
    hdc3020_mode_t mode;     /**< Measurement mode */
    hdc3020_lpm_t lpm;       /**< Low power mode */
    hdc3020_rate_t rate;     /**< Measurement rate (for auto mode) */
} hdc3020_config_t;

/**
 * @brief Command codes for HDC3020
 */
#define HDC3020_CMD_SOFT_RESET                  0x30A2
#define HDC3020_CMD_READ_MEASUREMENT            0xE000
#define HDC3020_CMD_READ_RH_ONLY                0xE001
#define HDC3020_CMD_EXIT_AUTO_MODE              0x3093

/**
 * @brief Error codes for HDC3020 functions
 */
#define HDC3020_OK                  0    /**< Success */
#define HDC3020_ERR_I2C            -1    /**< I2C communication error */
#define HDC3020_ERR_CRC            -2    /**< CRC verification failed */
#define HDC3020_ERR_INVALID_DATA   -3    /**< Invalid measurement data */

/**
 * @brief Initialize the HDC3020 sensor
 *
 * @param config Pointer to configuration structure
 * @return int 0 on success, error code otherwise
 */
int hdc3020_init(const hdc3020_config_t *config);

/**
 * @brief Reset the HDC3020 sensor
 *
 * @return int 0 on success, error code otherwise
 */
int hdc3020_reset(void);

/**
 * @brief Get data from the selected sensor(s)
 *
 * @param data Pointer to data structure to store the readings
 * @param sensor_type Which sensor to read (temperature, humidity, or both)
 * @return int 0 on success, error code otherwise
 */
int hdc3020_get_data(hdc3020_data_t *data, hdc3020_sensor_t sensor_type);

/**
 * @brief Trigger a single measurement (only in trigger-on-demand mode)
 *
 * @param lpm Low power mode to use for the measurement
 * @return int 0 on success, error code otherwise
 */
int hdc3020_trigger_measurement(hdc3020_lpm_t lpm);

/**
 * @brief Set the measurement mode
 *
 * @param mode Measurement mode
 * @param lpm Low power mode
 * @param rate Measurement rate (for auto mode)
 * @return int 0 on success, error code otherwise
 */
int hdc3020_set_mode(hdc3020_mode_t mode, hdc3020_lpm_t lpm, hdc3020_rate_t rate);

/**
 * @brief Calculate CRC-8 checksum for HDC3020 communication
 *
 * @param data Pointer to data bytes
 * @param length Number of bytes
 * @return uint8_t Calculated CRC value
 */
uint8_t hdc3020_calculate_crc(const uint8_t *data, uint8_t length);

#endif /* SENSOR_HDC3020_H_ */
