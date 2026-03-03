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
#ifndef I2C_COMM_H_
#define I2C_COMM_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief I2C status codes
 */
#define I2C_COMM_OK                 0    /**< Success */
#define I2C_COMM_ERR               -1    /**< Error   */

/* I2C timeout duration; can be adjusted by the user */
#define I2C_TIMEOUT                 (1000000)

/**
 * @brief Initialize the I2C interface
 *
 * @return int 0 on success, error code otherwise
 */
int I2C_init(void);

/**
 * @brief Write data to an I2C device
 *
 * @param addr Device I2C address
 * @param data Pointer to data to be written
 * @param len Number of bytes to write
 * @return int 0 on success, error code otherwise
 */
int I2C_write(uint8_t addr, uint8_t *data, uint8_t len);

/**
 * @brief Read data from an I2C device
 *
 * @param addr Device I2C address
 * @param buf Pointer to store the read data
 * @param len Number of bytes to read
 * @return int 0 on success, error code otherwise
 */
int I2C_read(uint8_t addr, uint8_t *buf, uint8_t len);


#endif /* I2C_COMM_H_ */
