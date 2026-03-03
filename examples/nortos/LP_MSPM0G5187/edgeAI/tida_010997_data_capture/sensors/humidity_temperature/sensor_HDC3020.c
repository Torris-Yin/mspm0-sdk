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

#include "sensor_HDC3020.h"
#include "i2c_comm.h"

/* Number of CPU cycles for 1ms delay */
#define CYCLES_PER_MS          (32000)

/* Macro to convert duration (ms) to hardware clock ticks */
#define MS_TO_CYCLES(ms)       ((ms) * (CYCLES_PER_MS))

/* Command codes for different trigger modes */
static const uint16_t hdc3020_trigger_cmd[4] = {
    0x2400,  /* LPM0 */
    0x240B,  /* LPM1 */
    0x2416,  /* LPM2 */
    0x24FF   /* LPM3 */
};

/* Command codes for different auto modes and rates */
static const uint16_t hdc3020_auto_cmd[5][4] = {
    /* LPM0,    LPM1,    LPM2,    LPM3 */
    { 0x2032,  0x2024,  0x202F,  0x20FF },  /* 0.5Hz */
    { 0x2130,  0x2126,  0x212D,  0x21FF },  /* 1Hz */
    { 0x2236,  0x2220,  0x222B,  0x22FF },  /* 2Hz */
    { 0x2334,  0x2322,  0x2329,  0x23FF },  /* 4Hz */
    { 0x2737,  0x2721,  0x272A,  0x27FF }   /* 10Hz */
};

/* Conversion times in milliseconds for each LPM */
static const uint8_t hdc3020_conversion_time[4] = {
    13,  /* LPM0 */
    8,   /* LPM1 */
    5,   /* LPM2 */
    4    /* LPM3 */
};

/* Internal state */
static struct {
    hdc3020_config_t config;
} gHdc3020State;

/**
 * @brief Send a 16-bit command to the HDC3020
 *
 * @param cmd 16-bit command
 * @return int 0 on success, error code otherwise
 */
static int send_command(uint16_t cmd) {
    uint8_t buf[2];
    buf[0] = (cmd >> 8) & 0xFF;  // MSB
    buf[1] = cmd & 0xFF;         // LSB

    return I2C_write(I2C_TARGET_ADDR_HDC3020, buf, 2);
}

/**
 * @brief Calculate CRC-8 checksum for HDC3020 communication
 *
 * @param data Pointer to data bytes
 * @param length Number of bytes
 * @return uint8_t Calculated CRC value
 */
uint8_t hdc3020_calculate_crc(const uint8_t *data, uint8_t length) {
    uint8_t crc = 0xFF;  // Initial value
    uint8_t polynomial = 0x31;  // x^8 + x^5 + x^4 + 1

    for (uint8_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ polynomial;
            } else {
                crc = (crc << 1);
            }
        }
    }

    return crc;
}

/**
 * @brief Initialize the HDC3020 sensor
 *
 * @param config Pointer to configuration structure
 * @return int 0 on success, error code otherwise
 */
int hdc3020_init(const hdc3020_config_t *config) {
    int ret;

    /* Store configuration */
    memcpy(&gHdc3020State.config, config, sizeof(hdc3020_config_t));

    /* Reset the sensor */
    ret = hdc3020_reset();
    if (ret != HDC3020_OK) {
        return ret;
    }

    /* Wait (5ms) for reset to complete */
    delay_cycles(MS_TO_CYCLES(5));

    return hdc3020_set_mode(config->mode, config->lpm, config->rate);
}

/**
 * @brief Reset the HDC3020 sensor
 *
 * @return int 0 on success, error code otherwise
 */
int hdc3020_reset(void) {
    return send_command(HDC3020_CMD_SOFT_RESET);
}

/**
 * @brief Set the measurement mode
 *
 * @param mode Measurement mode
 * @param lpm Low power mode
 * @param rate Measurement rate (for auto mode)
 * @return int 0 on success, error code otherwise
 */
int hdc3020_set_mode(hdc3020_mode_t mode, hdc3020_lpm_t lpm, hdc3020_rate_t rate) {
    int ret;

    gHdc3020State.config.mode = mode;
    gHdc3020State.config.lpm = lpm;

    if (mode == HDC3020_MODE_AUTO_MEASUREMENT) {
        gHdc3020State.config.rate = rate;
        ret = send_command(hdc3020_auto_cmd[rate][lpm]);
    } else {
        ret = HDC3020_OK;
    }

    return ret;
}

/**
 * @brief Trigger a single measurement (only in trigger-on-demand mode)
 *
 * @param lpm Low power mode to use for the measurement
 * @return int 0 on success, error code otherwise
 */
int hdc3020_trigger_measurement(hdc3020_lpm_t lpm) {
    if (gHdc3020State.config.mode != HDC3020_MODE_TRIGGER_ON_DEMAND) {

        int ret = send_command(HDC3020_CMD_EXIT_AUTO_MODE);
        if (ret != HDC3020_OK) {
            return ret;
        }

        /* Wait for 5ms before sending trigger-on-demand command */
        delay_cycles(MS_TO_CYCLES(5));

        gHdc3020State.config.mode = HDC3020_MODE_TRIGGER_ON_DEMAND;
    }

    return send_command(hdc3020_trigger_cmd[lpm]);
}

/**
 * @brief Get data from the selected sensor(s)
 *
 * @param data Pointer to data structure to store the readings
 * @param sensor_type Which sensor to read (temperature, humidity, or both)
 * @return int 0 on success, error code otherwise
 */
int hdc3020_get_data(hdc3020_data_t *data, hdc3020_sensor_t sensor_type) {
    uint8_t cmd[2];
    uint8_t buf[6];
    int ret;

    data->valid = false;

    /* Select the appropriate command based on sensor type */
    if (sensor_type == HDC3020_SENSOR_BOTH) {
        cmd[0] = 0xE0;
        cmd[1] = 0x00;  /* Read both temperature and humidity */
    } else if (sensor_type == HDC3020_SENSOR_HUMIDITY) {
        cmd[0] = 0xE0;
        cmd[1] = 0x01;  /* Read humidity only */
    } else {
        /* For temperature only, both needs to be read but process only temperature */
        cmd[0] = 0xE0;
        cmd[1] = 0x00;
    }

    if (gHdc3020State.config.mode == HDC3020_MODE_TRIGGER_ON_DEMAND) {
        ret = hdc3020_trigger_measurement(gHdc3020State.config.lpm);
        if (ret != HDC3020_OK) {
            return ret;
        }

        /* Wait for conversion to complete */
        delay_cycles(MS_TO_CYCLES(hdc3020_conversion_time[gHdc3020State.config.lpm]));
    }

    ret = I2C_write(I2C_TARGET_ADDR_HDC3020, cmd, 2);
    if (ret != HDC3020_OK) {
        return HDC3020_ERR_I2C;
    }

    /* Read the measurement data */
    if (sensor_type == HDC3020_SENSOR_BOTH) {
        /* Read both temperature and humidity (6 bytes total) */
        ret = I2C_read(I2C_TARGET_ADDR_HDC3020, buf, 6);
        if (ret != HDC3020_OK) {
            return HDC3020_ERR_I2C;
        }

        /* Verify CRC */
        uint8_t temp_data[2] = {buf[0], buf[1]};
        uint8_t rh_data[2] = {buf[3], buf[4]};

        if (hdc3020_calculate_crc(temp_data, 2) != buf[2] ||
            hdc3020_calculate_crc(rh_data, 2) != buf[5]) {
            return HDC3020_ERR_CRC;
        }

        uint16_t temp_raw = (uint16_t)buf[0] << 8 | buf[1];
        uint16_t rh_raw = (uint16_t)buf[3] << 8 | buf[4];

        /* Check for invalid readings */
        if (temp_raw == 0 || rh_raw == 0xFFFF) {
            return HDC3020_ERR_INVALID_DATA;
        }

        data->temperature = temp_raw;
        data->humidity = rh_raw;
        data->valid = true;
    }
    else if (sensor_type == HDC3020_SENSOR_HUMIDITY) {
        /* Read humidity only (3 bytes: RH_MSB, RH_LSB, RH_CRC) */
        ret = I2C_read(I2C_TARGET_ADDR_HDC3020, buf, 3);
        if (ret != HDC3020_OK) {
            return HDC3020_ERR_I2C;
        }

        /* Verify CRC */
        uint8_t rh_data[2] = {buf[0], buf[1]};
        if (hdc3020_calculate_crc(rh_data, 2) != buf[2]) {
            return HDC3020_ERR_CRC;
        }

        uint16_t rh_raw = (uint16_t)buf[0] << 8 | buf[1];

        /* Check for invalid reading */
        if (rh_raw == 0xFFFF) {
            return HDC3020_ERR_INVALID_DATA;
        }

        data->humidity = rh_raw;
        data->temperature = 0;  // Not measured
        data->valid = true;
    }
    else {
        /* Read temperature (first 3 bytes: T_MSB, T_LSB, T_CRC) */
        ret = I2C_read(I2C_TARGET_ADDR_HDC3020, buf, 3);
        if (ret != HDC3020_OK) {
            return HDC3020_ERR_I2C;
        }

        /* Verify CRC */
        uint8_t temp_data[2] = {buf[0], buf[1]};
        if (hdc3020_calculate_crc(temp_data, 2) != buf[2]) {
            return HDC3020_ERR_CRC;
        }

        uint16_t temp_raw = (uint16_t)buf[0] << 8 | buf[1];

        /* Check for invalid reading */
        if (temp_raw == 0) {
            return HDC3020_ERR_INVALID_DATA;
        }

        data->temperature = temp_raw;
        data->humidity = 0;  // Not measured
        data->valid = true;
    }

    return HDC3020_OK;
}
