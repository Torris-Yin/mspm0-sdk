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

#ifndef H_SPIDRIVER_H_
#define H_SPIDRIVER_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/** \brief Number of data frames to transfer in each SPI operation */
#define NUM_DATA_FRAMES_TO_TRANSFER     (125)
/** \brief Number of data types per transfer */
#define NUM_DATA_TYPES_PER_TRANSFTER    (1)
/** \brief Total number of frames in FIFO SPI transfer */
#define TOTAL_FIFO_SPI_FRAMES           (NUM_DATA_FRAMES_TO_TRANSFER*NUM_DATA_TYPES_PER_TRANSFTER)

/** \brief Default address used for FIFO data transmission */
#define TRANSMIT_ADDRESS     (1023)

/** \brief Size of the input buffer for ECG data */
#define TINIE_INPUT_SIZE     (2500)

/** \brief Buffer to store received SPI data */
extern uint8_t receiveBuffer[TOTAL_FIFO_SPI_FRAMES*3+5];

/** \brief Buffer for SPI transmit operations */
extern uint8_t transmitBuffer[5];

/** \brief Flag indicating SPI DMA receive completion */
extern volatile bool SPIDMARXdone;

/**
 * @brief Configure AFE register via SPI
 * @param addr Register address
 * @param data Data to write
 * @return Status code (0 for success)
 */
uint8_t AFE_register_config (uint16_t addr, uint32_t data);

/**
 * @brief Read data from AFE FIFO via SPI
 * @param addr Start address (FIFO address to read from)
 * @param length Number of bytes to read
 */
void AFE_FIFO_readout (uint16_t addr ,uint32_t length);

#endif /* H_SPIDRIVER_H_ */
