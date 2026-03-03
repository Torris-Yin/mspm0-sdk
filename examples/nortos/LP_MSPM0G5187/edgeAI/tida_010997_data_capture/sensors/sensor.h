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

#ifndef SENSOR_H_
#define SENSOR_H_

#include "pressure/sensor_BMP384.h"
#include "humidity_temperature/sensor_HDC3020.h"
#include "ambient_light/sensor_OPT4001.h"
#include "imu/sensor_BMI270.h"
#include "hall_effect/sensor_TMAG5170.h"

#include "ti_msp_dl_config.h"

/* Includes for DAP */
#include "uart_comm_target.h"
#include "dap_core.h"


/**
 * @brief Routes sample acquisition to appropriate sensor based on index
 * @param[in] index        Sensor index (SENSOR_IDX_PIR, SENSOR_IDX_TEMP, etc.)
 * @param[in] numSamples   Number of samples to acquire from selected sensor
 */
void Sensor_AcquireSamples(uint8_t index, uint32_t numSamples);


/* Flag to track number of PIR samples to send */
extern volatile uint32_t gSendPIRData;

/* Flag to indicate accelerometer/gyroscope data is ready */
extern volatile uint8_t gDataReady_Acc_Gyro;

/* Flag to indicate hall effect sensor data is ready */
extern volatile uint8_t gDataReady_HallEffect;

#endif /* SENSOR_H_ */
