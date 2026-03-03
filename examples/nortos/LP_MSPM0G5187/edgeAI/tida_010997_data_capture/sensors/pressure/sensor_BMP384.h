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

#ifndef SENSOR_BMP384_H
#define SENSOR_BMP384_H

#include <stdint.h>
#include <stdbool.h>
#include <ti/driverlib/m0p/dl_core.h>

/**
 * @brief BMP384 I2C address (fixed for boosterpack)
 */
#define I2C_TARGET_ADDR_BMP384         (0x77)

/**
 * @brief BMP384 chip ID
 */
#define BMP384_CHIP_ID            (0x50)

/**
 * @brief BMP384 register addresses
 */
#define BMP384_REG_CHIP_ID        (0x00)
#define BMP384_REG_ERR_REG        (0x02)
#define BMP384_REG_STATUS         (0x03)
#define BMP384_REG_DATA           (0x04)  /* Pressure XSB, LSB, MSB, Temp XSB, LSB, MSB */
#define BMP384_REG_EVENT          (0x10)
#define BMP384_REG_INT_STATUS     (0x11)
#define BMP384_REG_FIFO_LENGTH    (0x12)  /* 0x12, 0x13 */
#define BMP384_REG_FIFO_DATA      (0x14)
#define BMP384_REG_FIFO_WTM       (0x15)  /* 0x15, 0x16 */
#define BMP384_REG_FIFO_CONFIG_1  (0x17)
#define BMP384_REG_FIFO_CONFIG_2  (0x18)
#define BMP384_REG_INT_CTRL       (0x19)
#define BMP384_REG_IF_CONF        (0x1A)
#define BMP384_REG_PWR_CTRL       (0x1B)
#define BMP384_REG_OSR            (0x1C)
#define BMP384_REG_ODR            (0x1D)
#define BMP384_REG_CONFIG         (0x1F)
#define BMP384_REG_CMD            (0x7E)

/**
 * @brief BMP384 command values
 */
#define BMP384_CMD_FIFO_FLUSH     (0xB0)
#define BMP384_CMD_SOFT_RESET     (0xB6)

/**
 * @brief BMP384 power modes
 */
typedef enum {
    BMP384_MODE_SLEEP  = 0,  /**< Sleep mode */
    BMP384_MODE_FORCED = 1,  /**< Forced mode */
    BMP384_MODE_NORMAL = 3   /**< Normal mode */
} bmp384_mode_t;

/**
 * @brief BMP384 oversampling settings
 */
typedef enum {
    BMP384_OSR_x1  = 0,  /**< No oversampling */
    BMP384_OSR_x2  = 1,  /**< x2 oversampling */
    BMP384_OSR_x4  = 2,  /**< x4 oversampling */
    BMP384_OSR_x8  = 3,  /**< x8 oversampling */
    BMP384_OSR_x16 = 4,  /**< x16 oversampling */
    BMP384_OSR_x32 = 5   /**< x32 oversampling */
} bmp384_osr_t;

/**
 * @brief BMP384 output data rate settings
 */
typedef enum {
    BMP384_ODR_200HZ    = 0,   /**< 200 Hz */
    BMP384_ODR_100HZ    = 1,   /**< 100 Hz */
    BMP384_ODR_50HZ     = 2,   /**< 50 Hz */
    BMP384_ODR_25HZ     = 3,   /**< 25 Hz */
    BMP384_ODR_12_5HZ   = 4,   /**< 12.5 Hz */
    BMP384_ODR_6_25HZ   = 5,   /**< 6.25 Hz */
    BMP384_ODR_3_1HZ    = 6,   /**< 3.1 Hz */
    BMP384_ODR_1_5HZ    = 7,   /**< 1.5 Hz */
    BMP384_ODR_0_78HZ   = 8,   /**< 0.78 Hz */
    BMP384_ODR_0_39HZ   = 9,   /**< 0.39 Hz */
    BMP384_ODR_0_2HZ    = 10,  /**< 0.2 Hz */
    BMP384_ODR_0_1HZ    = 11,  /**< 0.1 Hz */
    BMP384_ODR_0_05HZ   = 12,  /**< 0.05 Hz */
    BMP384_ODR_0_02HZ   = 13,  /**< 0.02 Hz */
    BMP384_ODR_0_01HZ   = 14,  /**< 0.01 Hz */
    BMP384_ODR_0_006HZ  = 15,  /**< 0.006 Hz */
    BMP384_ODR_0_003HZ  = 16,  /**< 0.003 Hz */
    BMP384_ODR_0_0015HZ = 17   /**< 0.0015 Hz */
} bmp384_odr_t;

/**
 * @brief BMP384 IIR filter settings
 */
typedef enum {
    BMP384_FILTER_OFF = 0,  /**< Filter off */
    BMP384_FILTER_1   = 1,  /**< Coefficient 1 */
    BMP384_FILTER_3   = 2,  /**< Coefficient 3 */
    BMP384_FILTER_7   = 3,  /**< Coefficient 7 */
    BMP384_FILTER_15  = 4,  /**< Coefficient 15 */
    BMP384_FILTER_31  = 5,  /**< Coefficient 31 */
    BMP384_FILTER_63  = 6,  /**< Coefficient 63 */
    BMP384_FILTER_127 = 7   /**< Coefficient 127 */
} bmp384_filter_t;

/**
 * @brief BMP384 interrupt settings
 */
typedef enum {
    BMP384_INT_DISABLE     = 0,  /**< Interrupt disabled */
    BMP384_INT_DRDY        = 1,  /**< Data ready interrupt */
    BMP384_INT_FIFO_WM     = 2,  /**< FIFO watermark interrupt */
    BMP384_INT_FIFO_FULL   = 4   /**< FIFO full interrupt */
} bmp384_int_t;

/**
 * @brief BMP384 sensor data structure
 */
typedef struct {    
    uint32_t pressure;    
    bool valid;         /**< Flag indicating if the data is valid */
} bmp384_data_t;


/**
 * @brief BMP384 configuration structure
 */
typedef struct {
    bmp384_mode_t mode;        /**< Power mode */
    bmp384_osr_t pressOsr;    /**< Pressure oversampling */    
    bmp384_odr_t odr;          /**< Output data rate for normal mode */
    bmp384_filter_t filter;    /**< IIR filter coefficient */
    bool pressEn;             /**< Enable pressure measurement */    
    bmp384_int_t intMode;     /**< Interrupt mode */
} bmp384_config_t;

/**
 * @brief Error codes for BMP384 functions
 */
#define BMP384_OK                  0    /**< Success */
#define BMP384_ERR_COMM           -1    /**< Communication error */
#define BMP384_ERR_INVALID_ID     -2    /**< Invalid chip ID */
#define BMP384_ERR_INVALID_DATA   -3    /**< Invalid data */

/**
 * @brief Initialize the BMP384 sensor
 *
 * @param config Pointer to configuration structure
 * @return int 0 on success, error code otherwise
 */
int bmp384_init(const bmp384_config_t *config);

/**
 * @brief Reset the BMP384 sensor
 *
 * @return int 0 on success, error code otherwise
 */
int bmp384_reset(void);

/**
 * @brief Configure the BMP384 sensor
 *
 * @param config Pointer to configuration structure
 * @return int 0 on success, error code otherwise
 */
int bmp384_configure(const bmp384_config_t *config);

/**
 * @brief Get pressure data
 *
 * @param data Pointer to data structure to store the readings
 * @return int 0 on success, error code otherwise
 */
int bmp384_get_data(bmp384_data_t *data);

/**
 * @brief Trigger a single measurement (only in forced mode)
 *
 * @return int 0 on success, error code otherwise
 */
int bmp384_trigger_measurement(void);

/**
 * @brief Get sensor status
 *
 * @param status Pointer to store the status
 * @return int 0 on success, error code otherwise
 */
int bmp384_get_status(uint8_t *status);

#endif /* BMP384_H */
