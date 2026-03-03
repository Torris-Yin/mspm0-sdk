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

#ifndef SENSOR_BMI270_H_
#define SENSOR_BMI270_H_

#include <stdint.h>
#include <stdbool.h>
#include <ti/driverlib/m0p/dl_core.h>

/* BMI270 Register addresses */
#define BMI270_REG_CHIP_ID          0x00
#define BMI270_REG_ERR_REG          0x02
#define BMI270_REG_STATUS           0x03
#define BMI270_REG_DATA_8           0x0C  /* ACC_X (LSB) */
#define BMI270_REG_DATA_19          0x17  /* GYR_Z (MSB) */
#define BMI270_REG_SENSORTIME_0     0x18
#define BMI270_REG_SENSORTIME_2     0x1A
#define BMI270_REG_EVENT            0x1B
#define BMI270_REG_INT_STATUS_0     0x1C
#define BMI270_REG_INT_STATUS_1     0x1D
#define BMI270_REG_TEMPERATURE_0    0x22
#define BMI270_REG_TEMPERATURE_1    0x23
#define BMI270_REG_FIFO_LENGTH_0    0x24
#define BMI270_REG_FIFO_LENGTH_1    0x25
#define BMI270_REG_FIFO_DATA        0x26
#define BMI270_REG_FEAT_PAGE        0x2F
#define BMI270_REG_FEATURES         0x30
#define BMI270_REG_ACC_CONF         0x40
#define BMI270_REG_ACC_RANGE        0x41
#define BMI270_REG_GYR_CONF         0x42
#define BMI270_REG_GYR_RANGE        0x43
#define BMI270_REG_FIFO_DOWNS       0x45
#define BMI270_REG_FIFO_WTM_0       0x46
#define BMI270_REG_FIFO_WTM_1       0x47
#define BMI270_REG_FIFO_CONFIG_0    0x48
#define BMI270_REG_FIFO_CONFIG_1    0x49
#define BMI270_REG_INT_MAP_DATA     0x58
#define BMI270_REG_INIT_CTRL        0x59
#define BMI270_REG_INIT_ADDR_0      0x5B
#define BMI270_REG_INIT_ADDR_1      0x5C
#define BMI270_REG_INIT_DATA        0x5E
#define BMI270_REG_INT1_IO_CTRL     0x53
#define BMI270_REG_INT2_IO_CTRL     0x54
#define BMI270_REG_INT_LATCH        0x55
#define BMI270_REG_INT1_MAP_FEAT    0x56
#define BMI270_REG_INT2_MAP_FEAT    0x57
#define BMI270_REG_PWR_CONF         0x7C
#define BMI270_REG_PWR_CTRL         0x7D
#define BMI270_REG_CMD              0x7E

/* BMI270 Commands */
#define BMI270_CMD_SOFT_RESET       0xB6
#define BMI270_CMD_FIFO_FLUSH       0xB0

/* BMI270 Chip ID */
#define BMI270_CHIP_ID              0x24

/* Accelerometer ranges */
typedef enum {
    BMI270_ACC_RANGE_2G = 0,   /* �2g */
    BMI270_ACC_RANGE_4G = 1,   /* �4g */
    BMI270_ACC_RANGE_8G = 2,   /* �8g */
    BMI270_ACC_RANGE_16G = 3   /* �16g */
} bmi270_acc_range_t;

/* Gyroscope ranges */
typedef enum {
    BMI270_GYR_RANGE_2000DPS = 0,  /* �2000 dps */
    BMI270_GYR_RANGE_1000DPS = 1,  /* �1000 dps */
    BMI270_GYR_RANGE_500DPS = 2,   /* �500 dps */
    BMI270_GYR_RANGE_250DPS = 3,   /* �250 dps */
    BMI270_GYR_RANGE_125DPS = 4    /* �125 dps */
} bmi270_gyr_range_t;

/* Output Data Rate (ODR) */
typedef enum {
    BMI270_ODR_0_78HZ = 0x01,
    BMI270_ODR_1_56HZ = 0x02,
    BMI270_ODR_3_12HZ = 0x03,
    BMI270_ODR_6_25HZ = 0x04,
    BMI270_ODR_12_5HZ = 0x05,
    BMI270_ODR_25HZ = 0x06,
    BMI270_ODR_50HZ = 0x07,
    BMI270_ODR_100HZ = 0x08,
    BMI270_ODR_200HZ = 0x09,
    BMI270_ODR_400HZ = 0x0A,
    BMI270_ODR_800HZ = 0x0B,
    BMI270_ODR_1600HZ = 0x0C,
    BMI270_ODR_3200HZ = 0x0D  /* Only for gyroscope */
} bmi270_odr_t;

/* Power modes */
typedef enum {
    BMI270_PWR_MODE_SUSPEND,
    BMI270_PWR_MODE_CONFIGURATION,
    BMI270_PWR_MODE_LOW_POWER,
    BMI270_PWR_MODE_NORMAL,
    BMI270_PWR_MODE_PERFORMANCE
} bmi270_power_mode_t;

/* Bandwidth Parameter */
typedef enum {
    BMI270_BWP_OSR4 = 0x00,    /* OSR4 mode */
    BMI270_BWP_OSR2 = 0x01,    /* OSR2 mode */
    BMI270_BWP_NORMAL = 0x02,  /* Normal mode */
    BMI270_BWP_CIC = 0x03      /* CIC mode */
} bmi270_bwp_t;

/* Sensor enable configuration */
typedef enum {
    BMI270_SENSOR_NONE = 0x00,         /* No sensors enabled */
    BMI270_SENSOR_ACC = 0x01,          /* Only accelerometer enabled */
    BMI270_SENSOR_GYR = 0x02,          /* Only gyroscope enabled */
    BMI270_SENSOR_ACC_GYR = 0x03       /* Both accelerometer and gyroscope enabled */
} bmi270_sensor_enable_t;

/* Interrupt types */
typedef enum {
    BMI270_INT_FIFO_FULL = 0,
    BMI270_INT_FIFO_WM,
    BMI270_INT_DATA_RDY,
    BMI270_INT_ERROR,
    BMI270_INT_ANY_MOTION,
    BMI270_INT_NO_MOTION
} bmi270_int_type_t;

/* Interrupt pin selection */
typedef enum {
    BMI270_INT_PIN_1 = 0,
    BMI270_INT_PIN_2 = 1
} bmi270_int_pin_t;

/* Accelerometer data structure */
typedef struct {
    int16_t xRaw;
    int16_t yRaw;
    int16_t zRaw;
    bool valid;
} bmi270_accel_t;

/* Gyroscope data structure */
typedef struct {
    int16_t xRaw;
    int16_t yRaw;
    int16_t zRaw;
    bool valid;
} bmi270_gyro_t;

/* BMI270 configuration structure */
typedef struct {
    bmi270_power_mode_t powerMode;
    bmi270_sensor_enable_t sensorEnable;   /* Which sensors to enable */
    bmi270_acc_range_t accRange;
    bmi270_gyr_range_t gyrRange;
    bmi270_odr_t accOdr;
    bmi270_odr_t gyrOdr;
    bmi270_bwp_t accBwp;
    bmi270_bwp_t gyrBwp;
    bool accFilterPerf;
    bool gyrFilterPerf;
    bool gyrNoisePerf;
} bmi270_config_t;

/**
 * @brief Initialize the BMI270 sensor
 *
 * @param config Pointer to configuration structure
 * @return int 0 on success, error code otherwise
 */
int bmi270_init(const bmi270_config_t *config);

/**
 * @brief Reset the BMI270 sensor
 *
 * @return int 0 on success, error code otherwise
 */
int bmi270_reset(void);

/**
 * @brief Get accelerometer data
 *
 * @param accel Pointer to structure to store accelerometer data
 * @return int 0 on success, error code otherwise
 */
int bmi270_get_accel(bmi270_accel_t *accel);

/**
 * @brief Get gyroscope data
 *
 * @param gyro Pointer to structure to store gyroscope data
 * @return int 0 on success, error code otherwise
 */
int bmi270_get_gyro(bmi270_gyro_t *gyro);

/**
 * @brief Get both accelerometer and gyroscope data
 *
 * @param accel Pointer to structure to store accelerometer data
 * @param gyro Pointer to structure to store gyroscope data
 * @return int 0 on success, error code otherwise
 */
int bmi270_get_data(bmi270_accel_t *accel, bmi270_gyro_t *gyro);

/**
 * @brief Set power mode for the sensor
 *
 * @param mode Power mode to set
 * @return int 0 on success, error code otherwise
 */
int bmi270_set_power_mode(bmi270_power_mode_t mode);

/**
 * @brief Enable or disable specific sensors
 *
 * @param sensor_enable Which sensors to enable
 * @return int 0 on success, error code otherwise
 */
int bmi270_set_sensor_enable(bmi270_sensor_enable_t sensor_enable);

/**
 * @brief Configure interrupt for the BMI270
 *
 * @param int_type Type of interrupt to configure
 * @param int_pin Which interrupt pin to use (INT1 or INT2)
 * @param enable true to enable, false to disable
 * @return int 0 on success, error code otherwise
 */
int bmi270_configure_interrupt(bmi270_int_type_t int_type, bmi270_int_pin_t int_pin, bool enable);

/**
 * @brief Enable or disable the FIFO
 *
 * @param enable true to enable, false to disable
 * @param accel_en true to include accelerometer data in FIFO
 * @param gyro_en true to include gyroscope data in FIFO
 * @return int 0 on success, error code otherwise
 */
int bmi270_fifo_enable(bool enable, bool accel_en, bool gyro_en);

/**
 * @brief Read data from the FIFO
 *
 * @param buffer Buffer to store FIFO data
 * @param length Maximum length of data to read
 * @param bytes_read Actual number of bytes read
 * @return int 0 on success, error code otherwise
 */
int bmi270_fifo_read(uint8_t *buffer, uint16_t length, uint16_t *bytes_read);

/**
 * @brief Flush the FIFO
 *
 * @return int 0 on success, error code otherwise
 */
int bmi270_fifo_flush(void);

/* Error codes */
#define BMI270_OK                  0
#define BMI270_ERR_SPI            -1
#define BMI270_ERR_INVALID_ID     -2
#define BMI270_ERR_INVALID_PARAM  -3
#define BMI270_ERR_INIT_FAILED    -4
#define BMI270_ERR_TIMEOUT        -5
#define BMI270_ERR_FEATURE        -6
#define BMI270_ERR_SENSOR_DISABLED -7

#endif /* SENSOR_BMI270_H_ */
