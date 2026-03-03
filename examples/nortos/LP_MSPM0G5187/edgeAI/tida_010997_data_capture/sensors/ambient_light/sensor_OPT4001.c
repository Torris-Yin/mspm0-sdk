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

#include "sensor_OPT4001.h"
#include "i2c_comm.h"

/* Number of CPU cycles for 1ms delay */
#define CYCLES_PER_MS          (32000)

/* Macro to convert duration (ms) to hardware clock ticks */
#define MS_TO_CYCLES(ms)       ((ms) * (CYCLES_PER_MS))

/* Conversion time in milliseconds for each setting */
static const uint16_t opt4001_conv_time_ms[] = {
    1,    // 600us
    1,    // 1ms
    2,    // 1.8ms
    4,    // 3.4ms
    7,    // 6.5ms
    13,   // 12.7ms
    25,   // 25ms
    50,   // 50ms
    100,  // 100ms
    200,  // 200ms
    400,  // 400ms
    800   // 800ms
};

/* Internal state */
static struct {
    opt4001_config_t config;
} gOpt4001State;

/**
 * @brief Write data to an OPT4001 register
 *
 * @param reg Register address
 * @param data 16-bit data to write
 * @return int 0 on success, error code otherwise
 */
static int opt4001_write_reg(uint8_t reg, uint16_t data) {
    uint8_t buf[3];
    buf[0] = reg;
    buf[1] = (data >> 8) & 0xFF;  // MSB
    buf[2] = data & 0xFF;         // LSB

    return I2C_write(I2C_TARGET_ADDR_OPT4001, buf, 3);
}

/**
 * @brief Read data from an OPT4001 register
 *
 * @param reg Register address
 * @param data Pointer to store the 16-bit data
 * @return int 0 on success, error code otherwise
 */
static int opt4001_read_reg(uint8_t reg, uint16_t *data) {
    uint8_t buf[2];
    int ret;

    ret = I2C_write(I2C_TARGET_ADDR_OPT4001, &reg, 1);
    if (ret != OPT4001_OK) {
        return OPT4001_ERR_I2C;
    }

    ret = I2C_read(I2C_TARGET_ADDR_OPT4001, buf, 2);
    if (ret != OPT4001_OK) {
        return OPT4001_ERR_I2C;
    }

    *data = ((uint16_t)buf[0] << 8) | buf[1];
    return OPT4001_OK;
}

/**
 * @brief Initialize the OPT4001 sensor
 *
 * @param addr I2C address of the sensor
 * @param config Pointer to configuration structure
 * @return int 0 on success, error code otherwise
 */
int opt4001_init(const opt4001_config_t *config) {
    uint16_t device_id;
    int ret;

    memcpy(&gOpt4001State.config, config, sizeof(opt4001_config_t));

    /* Read device ID to verify communication */
    ret = opt4001_read_reg(OPT4001_REG_DEVICE_ID, &device_id);
    if (ret != OPT4001_OK) {
        return ret;
    }

    /* Verify device ID  */
    if ((device_id & 0x0FFF) != OPT4001_DEVICE_ID) {
        return OPT4001_ERR_INVALID_DATA;
    }

    return opt4001_configure(config);
}

/**
 * @brief Configure the OPT4001 sensor
 *
 * @param config Pointer to configuration structure
 * @return int 0 on success, error code otherwise
 */
int opt4001_configure(const opt4001_config_t *config) {
    uint16_t config_reg = 0;

    memcpy(&gOpt4001State.config, config, sizeof(opt4001_config_t));

    /* Configuration register value */
    config_reg |= (config->quickWake ? 1U : 0U) << 15;
    config_reg |= ((uint16_t)config->range & 0xF) << 10;
    config_reg |= ((uint16_t)config->convTime & 0xF) << 6;
    config_reg |= ((uint16_t)config->mode & 0x3) << 4;
    config_reg |= ((uint16_t)config->latch & 0x1) << 3;
    config_reg |= (config->intPol ? 1U : 0U) << 2;

    return opt4001_write_reg(OPT4001_REG_CONFIG, config_reg);
}

/**
 * @brief Configure the interrupt settings
 *
 * @param int_cfg Interrupt configuration
 * @param i2c_burst Enable I2C burst mode
 * @return int 0 on success, error code otherwise
 */
int opt4001_configure_interrupt(opt4001_int_cfg_t int_cfg, bool i2c_burst) {
    uint16_t int_config_reg = 0;

    gOpt4001State.config.intCfg = int_cfg;

    /* Interrupt Configuration register value */
    int_config_reg |= 1U << 15;  // Required bit
    int_config_reg |= 1U << 4;   // INT_DIR = 1 (output)
    int_config_reg |= ((uint16_t)int_cfg & 0x3) << 2;
    int_config_reg |= (i2c_burst ? 1U : 0U);

    return opt4001_write_reg(OPT4001_REG_INT_CONFIG, int_config_reg);
}

/**
 * @brief Get light sensor data
 *
 * @param data Pointer to data structure to store the reading
 * @return int 0 on success, error code otherwise
 */
int opt4001_get_data(opt4001_data_t *data) {
    uint16_t result_msb, result_lsb;
    int ret;

    data->valid = false;

    /* If mode is one-shot, trigger a measurement */
    if (gOpt4001State.config.mode == OPT4001_MODE_ONESHOT) {
        ret = opt4001_trigger_measurement();
        if (ret != OPT4001_OK) {
            return ret;
        }

        /* Wait for conversion to complete */
        delay_cycles(MS_TO_CYCLES(opt4001_conv_time_ms[gOpt4001State.config.convTime]));
    }

    ret = opt4001_read_reg(OPT4001_REG_RESULT, &result_msb);
    if (ret != OPT4001_OK) {
        return ret;
    }

    ret = opt4001_read_reg(OPT4001_REG_RESULT + 1, &result_lsb);
    if (ret != OPT4001_OK) {
        return ret;
    }

    /* Extract exponent and mantissa */
    data->exponent = (result_msb >> 12) & 0xF;
    data->mantissa = ((result_msb & 0xFFF) << 8) | (result_lsb >> 8);

    data->valid = true;

    return OPT4001_OK;
}

/**
 * @brief Trigger a single measurement (only in one-shot mode)
 *
 * @return int 0 on success, error code otherwise
 */
int opt4001_trigger_measurement(void) {
    uint16_t config_reg;
    int ret;

    ret = opt4001_read_reg(OPT4001_REG_CONFIG, &config_reg);
    if (ret != OPT4001_OK) {
        return ret;
    }

    /* Set one-shot mode (bits 5:4 = 01) */
    config_reg &= ~(0x3 << 4);  // Clear mode bits
    config_reg |= 0x1 << 4;     // Set one-shot mode

    return opt4001_write_reg(OPT4001_REG_CONFIG, config_reg);
}

