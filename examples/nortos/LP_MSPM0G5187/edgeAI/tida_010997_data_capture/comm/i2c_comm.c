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

#include "i2c_comm.h"
#include "ti_msp_dl_config.h"

/**
 * @brief Initialize the I2C interface
 *
 * @return int 0 on success, error code otherwise
 */
int I2C_init(void) {
    /* Initialization handled via SysConfig */
    return I2C_COMM_OK;
}

/**
 * @brief Write data to an I2C device
 *
 * @param addr Device I2C address
 * @param data Pointer to data to be written
 * @param len Number of bytes to write
 * @return int 0 on success, error code otherwise
 */
int I2C_write(uint8_t addr, uint8_t *data, uint8_t len)
{
    uint32_t timeout = 0;
    volatile uint8_t bytesWritten = 0;

    DL_I2CC_clearInterruptStatus(I2C_INST, DL_I2CC_INTERRUPT_TX_DONE);

    /* Wait till bus is busy */
    while (DL_I2CC_getStatus(I2C_INST) & DL_I2CC_STATUS_BUSY_BUS)
    {
        if (timeout++ > I2C_TIMEOUT)
        {
            timeout = 0;
            return I2C_COMM_ERR;
        }
    }

    /* Flush Tx FIFO */
    DL_I2CC_startFlushTXFIFO(I2C_INST);
    while(!(DL_I2CC_isTXFIFOEmpty(I2C_INST)));
    DL_I2CC_stopFlushTXFIFO(I2C_INST);

    while (!(DL_I2CC_getStatus(I2C_INST) & DL_I2CC_STATUS_IDLE));

    /* Fill Tx FIFO */
    bytesWritten += DL_I2CC_fillTXFIFO(I2C_INST, &data[bytesWritten], len);

    DL_I2CC_startTransfer(I2C_INST, addr, DL_I2CC_DIRECTION_TX, len);

    while (bytesWritten < len)
    {
        bytesWritten += DL_I2CC_fillTXFIFO(I2C_INST, &data[bytesWritten], len - bytesWritten);
    }

    while (DL_I2CC_getStatus(I2C_INST) & DL_I2CC_STATUS_BUSY_BUS);

    while(DL_I2CC_getRawInterruptStatus(I2C_INST, DL_I2CC_INTERRUPT_TX_DONE) == 0);

    DL_I2CC_clearInterruptStatus(I2C_INST, DL_I2CC_INTERRUPT_TX_DONE);

    return I2C_COMM_OK;
}

/**
 * @brief Read data from an I2C device
 *
 * @param addr Device I2C address
 * @param buf Pointer to store the read data
 * @param len Number of bytes to read
 * @return int 0 on success, error code otherwise
 */
int I2C_read(uint8_t addr, uint8_t *buf, uint8_t len)
{
    uint32_t timeout = 0;
    
    /* Wait till bus is busy */
    while (DL_I2CC_getStatus(I2C_INST) & DL_I2CC_STATUS_BUSY_BUS)
    {
        if (timeout++ > I2C_TIMEOUT)
        {
            timeout = 0;
            return I2C_COMM_ERR;
        }
    }

    /* Flush Tx FIFO */
    DL_I2CC_startFlushRXFIFO(I2C_INST);
    while(!(DL_I2CC_isRXFIFOEmpty(I2C_INST)));
    DL_I2CC_stopFlushRXFIFO(I2C_INST);

    while (!(DL_I2CC_getStatus(I2C_INST) & DL_I2CC_STATUS_IDLE));

    /* Send I2C Read Command */
    DL_I2CC_startTransfer(I2C_INST, addr, DL_I2CC_DIRECTION_RX, len);

    uint32_t waitTimeout = 0;
    while(DL_I2CC_getRawInterruptStatus(I2C_INST, DL_I2CC_INTERRUPT_RXFIFO_TRIGGER) == 0)
    {
        if (waitTimeout++ > I2C_TIMEOUT)
        {
            waitTimeout = 0;
            return I2C_COMM_ERR;
        }
    }

    waitTimeout = 0;
    for (uint8_t i = 0; i < len; i++) {
        while (DL_I2CC_isRXFIFOEmpty(I2C_INST))
        {
            if (waitTimeout++ > I2C_TIMEOUT)
            {
                waitTimeout = 0;
                return I2C_COMM_ERR;
            }
        }
        *buf = DL_I2CC_receiveData(I2C_INST);
        buf++;
    }

    while (DL_I2CC_getStatus(I2C_INST) & DL_I2CC_STATUS_BUSY_BUS);

    return I2C_COMM_OK;
}

