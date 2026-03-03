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

#include "dap_interface.h"

/* Inner macro: Converts its argument to a string literal. */
#define STRINGIFY_HELPER(x) #x

/* Outer macro: Expands its argument before passing it to STRINGIFY_HELPER. */
#define STRINGIFY(x) STRINGIFY_HELPER(x)

#ifdef DEVICE_NAME
const char deviceName[] = STRINGIFY(DEVICE_NAME);
#endif

const SensorInfo pirSensor = {
    .name = "PIR",
    .type = SENSOR_TYPE_SCALAR,
    .format = DATA_FORMAT_UINT8,
    .label = "\"x\"",
    .misc = ""
};

const SensorInfo tempSensor = {
    .name = "Temperature",
    .type = SENSOR_TYPE_SCALAR,
    .format = DATA_FORMAT_UINT16,
    .label = "\"x\"",
    .misc = ""
};

const SensorInfo humiditySensor = {
    .name = "Humidity",
    .type = SENSOR_TYPE_SCALAR,
    .format = DATA_FORMAT_UINT16,
    .label = "\"x\"",
    .misc = ""
};

const SensorInfo alsSensor = {
    .name = "Light",
    .type = SENSOR_TYPE_SCALAR,
    .format = DATA_FORMAT_UINT16,
    .label = "\"x\"",
    .misc = ""
};

const SensorInfo pressureSensor = {
    .name = "Pressure",
    .type = SENSOR_TYPE_SCALAR,
    .format = DATA_FORMAT_UINT32,
    .label = "\"x\"",
    .misc = ""
};

const SensorInfo digitalMicSensor = {
     .name = "Digital Microphone",
     .type = SENSOR_TYPE_AUDIO,
     .format = DATA_FORMAT_INT32,
     .label = "\"x\"",
     .misc = ""
};

const SensorInfo analogMicSensor = {
     .name = "Analog Microphone",
     .type = SENSOR_TYPE_AUDIO,
     .format = DATA_FORMAT_INT32,
     .label = "\"x\"",
     .misc = ""
};

const SensorInfo magnetSensor = {
     .name = "Magnetic Sensor",
     .type = SENSOR_TYPE_VECTOR,
     .format = DATA_FORMAT_INT16,
     .label = "[\"x\", \"y\", \"z\"]",
     .misc = "\"columns\":3"
};

const SensorInfo accelerometerSensor = {
     .name = "3-axis Accelerometer",
     .type = SENSOR_TYPE_VECTOR,
     .format = DATA_FORMAT_INT16,
     .label = "[\"x\", \"y\", \"z\"]",
     .misc = "\"columns\":3"
};

const SensorInfo gyroSensor = {
     .name = "3-axis Gyroscope",
     .type = SENSOR_TYPE_VECTOR,
     .format = DATA_FORMAT_INT16,
     .label = "[\"x\", \"y\", \"z\"]",
     .misc = "\"columns\":3"
};

const SensorInfo* const gSensors[TOTAL_SENSOR_COUNT] = \
                {
                    [SENSOR_IDX_PIR]      = &pirSensor,
                    [SENSOR_IDX_TEMP]     = &tempSensor,
                    [SENSOR_IDX_HUMID]    = &humiditySensor,
                    [SENSOR_IDX_PRESS]    = &pressureSensor,
                    [SENSOR_IDX_ALS]      = &alsSensor,
                    [SENSOR_IDX_MIC_DIG]  = &digitalMicSensor,
                    [SENSOR_IDX_MIC_ANA]  = &analogMicSensor,
                    [SENSOR_IDX_MAG]      = &magnetSensor,
                    [SENSOR_IDX_ACCEL]    = &accelerometerSensor,
                    [SENSOR_IDX_GYRO]     = &gyroSensor
                };



PropertyInfo property1 = {
    .name = "samples",
    .type = DATA_FORMAT_UINT32,
    .value.u32 = 0,  
};

PropertyInfo* gProperties[TOTAL_PROPERTY_COUNT] = {&property1};

PipelineConfig gPipelineConfig = {
    .mode = PIPELINE_MODE_DATA_ACQUISITION,
    .modelIndex = 0
};

const char *const gInferenceValues[TOTAL_INFERENCE_VALUES];
