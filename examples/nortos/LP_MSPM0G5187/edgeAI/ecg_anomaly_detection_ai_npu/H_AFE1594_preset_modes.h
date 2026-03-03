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

#ifndef H_AFEFUNCTIONS_H_
#define H_AFEFUNCTIONS_H_

#include <stdint.h>

/**
 * @brief Configure AFE1594 for ECG-only mode
 *
 * Sets up the AFE1594 with appropriate settings for ECG signal acquisition,
 * including gain, filter settings, and sampling rate optimized for ECG.
 */
void mode_ECG();

/**
 * @brief Configure AFE1594 for combined ECG and respiration measurement
 *
 * Sets up the AFE1594 with settings for simultaneous ECG and respiration
 * measurement, including appropriate channels and filters.
 */
void mode_ECG_RESP();

/**
 * @brief Configure AFE1594 for ECG with pacemaker pulse detection
 *
 * Sets up the AFE1594 with settings optimized for ECG recording with
 * additional pacemaker pulse detection capability.
 */
void mode_ECG_PACE();

/**
 * @brief Configure AFE1594 for combined ECG, respiration, and pacemaker detection
 *
 * Sets up the AFE1594 for the most comprehensive mode that enables
 * simultaneous ECG, respiration, and pacemaker pulse detection.
 */
void mode_ECG_PACE_RESP();

#endif /* H_AFEFUNCTIONS_H_ */
