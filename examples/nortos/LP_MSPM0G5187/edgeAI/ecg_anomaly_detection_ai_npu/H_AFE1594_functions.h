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

#ifndef H_AFE1594_FUNCTIONS_H_
#define H_AFE1594_FUNCTIONS_H_

#include <stdint.h>

/**
 * @brief Perform hardware reset of the AFE1594 chip
 *
 * Toggles the reset pin to reset the AFE1594 to its initial state.
 */
void AFE_hardware_reset();

/**
 * @brief Initialize the SPI interface for communication with AFE1594
 *
 * Configures SPI parameters like clock rate, mode, and pin connections.
 */
void AFE_SPI_setup();

/**
 * @brief Configure the GPIO pins for AFE1594 operation
 *
 * Sets up interrupt pins and other GPIO connections to the AFE.
 */
void AFE_setup_GPIO();

/**
 * @brief Initialize the FIFO settings for AFE1594
 *
 * Configures the FIFO threshold, watermark, and data format settings.
 */
void AFE_setup_FIFO();

/**
 * @brief Finalize the respiration measurement settings
 *
 * Completes the setup for respiration measurement on AFE1594.
 */
void RESP_finalize();

#endif /* H_AFE1594_FUNCTIONS_H_ */
