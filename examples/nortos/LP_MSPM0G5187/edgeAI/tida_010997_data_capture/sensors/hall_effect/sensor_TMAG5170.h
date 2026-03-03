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

#ifndef SENSOR_TMAG5170_H_
#define SENSOR_TMAG5170_H_

#include <stdint.h>
#include <stdbool.h>
#include <ti/driverlib/m0p/dl_core.h>

/* TMAG5170 Register addresses */
#define TMAG5170_REG_DEVICE_CONFIG     0x0
#define TMAG5170_REG_SENSOR_CONFIG     0x1
#define TMAG5170_REG_SYSTEM_CONFIG     0x2
#define TMAG5170_REG_ALERT_CONFIG      0x3
#define TMAG5170_REG_X_THRX_CONFIG     0x4
#define TMAG5170_REG_Y_THRX_CONFIG     0x5
#define TMAG5170_REG_Z_THRX_CONFIG     0x6
#define TMAG5170_REG_T_THRX_CONFIG     0x7
#define TMAG5170_REG_CONV_STATUS       0x8
#define TMAG5170_REG_X_CH_RESULT       0x9
#define TMAG5170_REG_Y_CH_RESULT       0xA
#define TMAG5170_REG_Z_CH_RESULT       0xB
#define TMAG5170_REG_TEMP_RESULT       0xC
#define TMAG5170_REG_AFE_STATUS        0xD
#define TMAG5170_REG_SYS_STATUS        0xE
#define TMAG5170_REG_TEST_CONFIG       0xF


/* SPI read/write bit */
#define TMAG5170_SPI_READ              0x80
#define TMAG5170_SPI_WRITE             0x00

/* Operating modes */
typedef enum {
    TMAG5170_MODE_CONFIGURATION = 0,      /* Configuration mode */
    TMAG5170_MODE_STANDBY = 1,            /* Standby mode */
    TMAG5170_MODE_ACTIVE_MEASURE = 2,     /* Active measure mode (continuous) */
    TMAG5170_MODE_ACTIVE_TRIGGER = 3,     /* Active trigger mode */
    TMAG5170_MODE_WAKE_UP_SLEEP = 4,      /* Wake-up and sleep mode */
    TMAG5170_MODE_SLEEP = 5,              /* Sleep mode */
    TMAG5170_MODE_DEEP_SLEEP = 6          /* Deep sleep mode */
} tmag5170_operating_mode_t;

/* Magnetic field measurement ranges */
typedef enum {
    TMAG5170_RANGE_50mT = 0,   /* ±50 mT */
    TMAG5170_RANGE_25mT = 1,   /* ±25 mT */
    TMAG5170_RANGE_100mT = 2   /* ±100 mT */
} tmag5170_mag_range_t;


/* Magnetic channel enable configuration */
typedef enum {
    TMAG5170_CH_NONE = 0x00,   /* No channels enabled */
    TMAG5170_CH_X = 0x01,      /* Only X channel enabled */
    TMAG5170_CH_Y = 0x02,      /* Only Y channel enabled */
    TMAG5170_CH_Z = 0x04,      /* Only Z channel enabled */
    TMAG5170_CH_XY = 0x03,     /* X and Y channels enabled */
    TMAG5170_CH_XZ = 0x05,     /* X and Z channels enabled */
    TMAG5170_CH_YZ = 0x06,     /* Y and Z channels enabled */
    TMAG5170_CH_XYZ = 0x07     /* All channels enabled */
} tmag5170_ch_en_t;

/* Conversion averaging options */
typedef enum {
    TMAG5170_CONV_AVG_1 = 0,   /* No averaging */
    TMAG5170_CONV_AVG_2 = 1,   /* Average 2 conversions */
    TMAG5170_CONV_AVG_4 = 2,   /* Average 4 conversions */
    TMAG5170_CONV_AVG_8 = 3,   /* Average 8 conversions */
    TMAG5170_CONV_AVG_16 = 4,  /* Average 16 conversions */
    TMAG5170_CONV_AVG_32 = 5   /* Average 32 conversions */
} tmag5170_conv_avg_t;


/* Magnetic sensor data structure */
typedef struct {
    int16_t xRaw;              /* X-axis raw ADC value */
    int16_t yRaw;              /* Y-axis raw ADC value */
    int16_t zRaw;              /* Z-axis raw ADC value */
    bool valid;                /* Data validity flag */
} tmag5170_data_t;

/* TMAG5170 configuration structure */
typedef struct {
    tmag5170_operating_mode_t operatingMode;  /* Operating mode (per DEVICE_CONFIG bits 6-4) */
    tmag5170_mag_range_t magRange;            /* Magnetic range (per SENSOR_CONFIG bits 1-0) */
    tmag5170_ch_en_t chEn;                    /* Channel enable (per SENSOR_CONFIG bits 4-2) */
    tmag5170_conv_avg_t convAvg;              /* Conversion averaging (per DEVICE_CONFIG bits 14-12) */
    uint8_t dataRate;                         /* UNUSED - Reserved for future use */
    bool crcEn;                               /* Enable CRC for SPI communication (per TEST_CONFIG bit 0) */
    bool lowPower;                            /* UNUSED - Reserved for future use */
} tmag5170_config_t;

/* Error codes */
#define TMAG5170_OK                  0
#define TMAG5170_ERR_SPI            -1

/**
 * @brief Initialize the TMAG5170 sensor
 *
 * @param config Pointer to configuration structure
 * @return int 0 on success, error code otherwise
 */
int tmag5170_init(const tmag5170_config_t *config);

/**
 * @brief Reset the TMAG5170 sensor
 *
 * @return int 0 on success, error code otherwise
 */
int tmag5170_reset(void);

/**
 * @brief Get magnetic field raw data
 *
 * @param data Pointer to structure to store sensor data
 * @return int 0 on success, error code otherwise
 */
int tmag5170_get_data(tmag5170_data_t *data);

/**
 * @brief Trigger a single measurement (in trigger mode)
 *
 * @return int 0 on success, error code otherwise
 */
int tmag5170_trigger_measurement(void);

/**
 * @brief Set operating mode for the sensor
 *
 * @param mode Operating mode to set
 * @return int 0 on success, error code otherwise
 */
int tmag5170_set_operating_mode(tmag5170_operating_mode_t mode);

/**
 * @brief Set magnetic field measurement range
 *
 * @param range Measurement range to set
 * @return int 0 on success, error code otherwise
 */
int tmag5170_set_mag_range(tmag5170_mag_range_t range);

/**
 * @brief Configure which channels to enable
 *
 * @param ch_en Channel enable configuration
 * @return int 0 on success, error code otherwise
 */
int tmag5170_set_channels(tmag5170_ch_en_t ch_en);

/**
 * @brief Configure conversion averaging
 *
 * @param avg_mode Averaging mode to set
 * @return int 0 on success, error code otherwise
 */
int tmag5170_set_conversion_averaging(tmag5170_conv_avg_t avg_mode);

/**
 * @brief Enable or disable the data ready interrupt
 *
 * @param enable true to enable data ready interrupt, false to disable
 * @return int 0 on success, error code otherwise
 */
int tmag5170_set_data_ready_interrupt(bool enable);

/**
 * @brief Check if data ready interrupt is active
 *
 * @return int 1 if data ready, 0 if not, negative error code on failure
 */
int tmag5170_check_data_ready(void);

/**
 * @brief Clear data ready interrupt status
 *
 * @return int 0 on success, error code otherwise
 */
int tmag5170_clear_data_ready(void);

#endif /* SENSOR_TMAG5170_H_ */
