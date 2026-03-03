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

#include "sensor_BMP384.h"
#include "i2c_comm.h"


/* Number of CPU cycles for 1ms delay */
#define CYCLES_PER_MS          (32000)

/* Macro to convert duration (ms) to hardware clock ticks */
#define MS_TO_CYCLES(ms)       ((ms) * (CYCLES_PER_MS))

/* Internal state */
static struct {
    bmp384_config_t config;        
} gBmp384State;

/* Measurement time in milliseconds for each OSR setting */
static const uint16_t bmp384_meas_time_ms[] = {
    1,   /* OSR x1 */
    2,   /* OSR x2 */
    4,   /* OSR x4 */
    8,   /* OSR x8 */
    16,  /* OSR x16 */
    32   /* OSR x32 */
};

/**
 * @brief Write data to a BMP384 register
 *
 * @param reg Register address
 * @param data Data to write
 * @return int 0 on success, error code otherwise
 */
static int bmp384_write_reg(uint8_t reg, uint8_t data) {
    uint8_t buf[2];
    buf[0] = reg;
    buf[1] = data;

    return I2C_write(I2C_TARGET_ADDR_BMP384, buf, 2);
}

/**
 * @brief Read data from a BMP384 register
 *
 * @param reg Register address
 * @param data Pointer to store the data
 * @param len Number of bytes to read
 * @return int 0 on success, error code otherwise
 */
static int bmp384_read_reg(uint8_t reg, uint8_t *data, uint8_t len) {
    int ret;

    /* Set register pointer */
    ret = I2C_write(I2C_TARGET_ADDR_BMP384, &reg, 1);
    if (ret != BMP384_OK) {
        return BMP384_ERR_COMM;
    }

    /* Read register data */
    ret = I2C_read(I2C_TARGET_ADDR_BMP384, data, len);
    if (ret != BMP384_OK) {
        return BMP384_ERR_COMM;
    }

    return BMP384_OK;
}


/**
 * @brief Initialize the BMP384 sensor
 *
 * @param config Pointer to configuration structure
 * @return int 0 on success, error code otherwise
 */
int bmp384_init(const bmp384_config_t *config) {
    uint8_t chip_id;
    int ret;

    /* Read chip ID to verify communication */
    ret = bmp384_read_reg(BMP384_REG_CHIP_ID, &chip_id, 1);
    if (ret != BMP384_OK) {
        return ret;
    }

    /* Verify chip ID */
    if (chip_id != BMP384_CHIP_ID) {
        return BMP384_ERR_INVALID_ID;
    }

    /* Soft reset the sensor */
    ret = bmp384_reset();
    if (ret != BMP384_OK) {
        return ret;
    }

    /* Wait 10ms for reset to complete */
    delay_cycles(MS_TO_CYCLES(10));

    /* Configure the sensor */
    return bmp384_configure(config);
}

/**
 * @brief Reset the BMP384 sensor
 *
 * @return int 0 on success, error code otherwise
 */
int bmp384_reset(void) {
    return bmp384_write_reg(BMP384_REG_CMD, BMP384_CMD_SOFT_RESET);
}

/**
 * @brief Configure the BMP384 sensor
 *
 * @param config Pointer to configuration structure
 * @return int 0 on success, error code otherwise
 */
int bmp384_configure(const bmp384_config_t *config) {
    int ret;

    /* Store configuration */
    memcpy(&gBmp384State.config, config, sizeof(bmp384_config_t));

    /* Configure power control */
    uint8_t pwr_ctrl = ((config->pressEn ? 1 : 0) << 0) |
                       ((config->mode & 0x03) << 4);
    ret = bmp384_write_reg(BMP384_REG_PWR_CTRL, pwr_ctrl);
    if (ret != BMP384_OK) {
        return ret;
    }

    /* Configure oversampling */
    uint8_t osr = ((config->pressOsr & 0x07) << 0);
    ret = bmp384_write_reg(BMP384_REG_OSR, osr);
    if (ret != BMP384_OK) {
        return ret;
    }

    /* Configure output data rate */
    uint8_t odr = config->odr & 0x1F;
    ret = bmp384_write_reg(BMP384_REG_ODR, odr);
    if (ret != BMP384_OK) {
        return ret;
    }

    /* Configure filter */
    uint8_t config_reg = (config->filter & 0x07) << 1;
    ret = bmp384_write_reg(BMP384_REG_CONFIG, config_reg);
    if (ret != BMP384_OK) {
        return ret;
    }

    /* Configure interrupts if needed */
    if (config->intMode != BMP384_INT_DISABLE) {
        uint8_t int_ctrl = 0;

        /* Configure interrupt sources */
        if (config->intMode & BMP384_INT_DRDY) {
            int_ctrl |= (1 << 6);  /* drdy_en */
        }
        if (config->intMode & BMP384_INT_FIFO_WM) {
            int_ctrl |= (1 << 3);  /* fwtm_en */
        }
        if (config->intMode & BMP384_INT_FIFO_FULL) {
            int_ctrl |= (1 << 4);  /* ffull_en */
        }

        /* Configure interrupt pin properties */
        int_ctrl |= (1 << 2);  /* int_level (active high) */

        ret = bmp384_write_reg(BMP384_REG_INT_CTRL, int_ctrl);
        if (ret != BMP384_OK) {
            return ret;
        }
    }

    return BMP384_OK;
}

/**
 * @brief Get pressure data
 *
 * @param data Pointer to data structure to store the readings
 * @return int 0 on success, error code otherwise
 */
int bmp384_get_data(bmp384_data_t *data) {
    uint8_t raw_data[6];
    uint8_t status;
    int ret;

    /* Initialize data as invalid */
    data->valid = false;

    /* If in forced mode, trigger a measurement */
    if (gBmp384State.config.mode == BMP384_MODE_FORCED) {
        ret = bmp384_trigger_measurement();
        if (ret != BMP384_OK) {
            return ret;
        }

        /* Wait for measurement to complete */
        uint16_t wait_time = bmp384_meas_time_ms[gBmp384State.config.pressOsr];        
        delay_cycles(MS_TO_CYCLES(wait_time));
    }

    /* Check if data is ready */
    ret = bmp384_get_status(&status);
    if (ret != BMP384_OK) {
        return ret;
    }

    if (!(status & 0x60)) {  /* Check drdy_press and drdy_temp bits */
        return BMP384_ERR_INVALID_DATA;
    }

    /* Read raw pressure and temperature data */
    ret = bmp384_read_reg(BMP384_REG_DATA, raw_data, 6);
    if (ret != BMP384_OK) {
        return ret;
    }

    /* Extract raw pressure values */
    data->pressure = (uint32_t)raw_data[2] << 16 | (uint32_t)raw_data[1] << 8 | raw_data[0];
    data->valid = true;
    return BMP384_OK;
}

/**
 * @brief Trigger a single measurement (only in forced mode)
 *
 * @return int 0 on success, error code otherwise
 */
int bmp384_trigger_measurement(void) {
    uint8_t pwr_ctrl;
    int ret;

    /* Read current power control register */
    ret = bmp384_read_reg(BMP384_REG_PWR_CTRL, &pwr_ctrl, 1);
    if (ret != BMP384_OK) {
        return ret;
    }

    /* Set mode to forced mode while preserving other bits */
    pwr_ctrl &= ~(0x03 << 4);  /* Clear mode bits */
    pwr_ctrl |= (BMP384_MODE_FORCED & 0x03) << 4;  /* Set forced mode */

    /* Write updated power control register */
    return bmp384_write_reg(BMP384_REG_PWR_CTRL, pwr_ctrl);
}

/**
 * @brief Get sensor status
 *
 * @param status Pointer to store the status
 * @return int 0 on success, error code otherwise
 */
int bmp384_get_status(uint8_t *status) {
    return bmp384_read_reg(BMP384_REG_STATUS, status, 1);
}


