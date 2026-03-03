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

#include "sensor.h"
#include <stdio.h>

/* Number of CPU cycles for 1ms delay */
#define CYCLES_PER_MS          (32000)

/* Macro to convert duration (ms) to hardware clock ticks */
#define MS_TO_CYCLES(ms)       ((ms) * (CYCLES_PER_MS))

/* Flag to track number of PIR samples to send */
volatile uint32_t gSendPIRData = 0;

/* Flag to indicate accelerometer/gyroscope data is ready */
volatile uint8_t gDataReady_Acc_Gyro = 0;

/* Flag to track data ready status */
volatile uint8_t gDataReady_HallEffect = 0;

/**
 * @brief Sends DAP Frame Header data over UART
 * @param[in] index        Sensor index
 * @param[in] len          Number of data samples to send per frame
 */
static void Sensor_sendHeader(SensorIndex index, uint8_t len)
{
    /* Send start byte */
    DL_UART_transmitDataBlocking(UART_DAP_INST,FRAME_START_BYTE);
    /* Send channel number */
    DL_UART_transmitDataBlocking(UART_DAP_INST, Serial_Resp_receiveData | Resp_CMD_Channel_sensor_signal);

    uint8_t numAxes = 1;

    if(gSensors[index]->type == SENSOR_TYPE_VECTOR)
    {
       /* Defaulting to 3 for standard 3-axis vector data */
       numAxes = 3;
    }

    uint32_t payloadLen = numAxes * (len) * getDataLen(gSensors[index]->format);

    /* Send Payload length */
    if(IN_RANGE(payloadLen, PAYLOAD_FIELD_1B_MIN_LEN, PAYLOAD_FIELD_1B_MAX_LEN))
    {
       DL_UART_transmitDataBlocking(UART_DAP_INST, (payloadLen) & 0xFF);
    }

    else if(IN_RANGE(payloadLen, PAYLOAD_FIELD_2B_MIN_LEN, PAYLOAD_FIELD_2B_MAX_LEN))
    {
       uint16_t dapLen = PAYLOAD_FIELD_2B_OFFSET + payloadLen;
       DL_UART_transmitDataBlocking(UART_DAP_INST, (dapLen >> 8) & 0xFF);
       DL_UART_transmitDataBlocking(UART_DAP_INST, (dapLen) & 0xFF);
    }

    else if(IN_RANGE(payloadLen, PAYLOAD_FIELD_3B_MIN_LEN, PAYLOAD_FIELD_3B_MAX_LEN))
    {
       uint32_t dapLen = PAYLOAD_FIELD_3B_OFFSET + payloadLen;
       DL_UART_transmitDataBlocking(UART_DAP_INST, (dapLen >> 16) & 0xFF);
       DL_UART_transmitDataBlocking(UART_DAP_INST, (dapLen >> 8) & 0xFF);
       DL_UART_transmitDataBlocking(UART_DAP_INST, (dapLen) & 0xFF);
    }

}

/**
 * @brief Sends sensor data over UART in big-endian format
 * @param[in] data     A pointer to data to be transmitted
 * @param[in] format   Data format type 
 */
static void Sensor_sendData(void *data , dataFormat format)
{
    uint8_t dataSize = getDataLen(format);
    uint8_t *ptr = (uint8_t *)data;

    for(int i = dataSize - 1; i >= 0 ;i--)
    {
        DL_UART_transmitDataBlocking(UART_DAP_INST, ptr[i]);
    }
}

/**
 * @brief Captures and streams PIR sensor data samples
 * @param[in] numSamples   Number of samples to capture
 */
static void Sensor_PIR_captureData(uint32_t numSamples)
{
    DL_Timer_startCounter(SAMPLING_TIMER_INST);

    uint8_t SamplesPerFrame = 1;
    uint8_t countPerFrame = 0;

    while (numSamples) {
           if(numSamples >= SamplesPerFrame)
           {
               countPerFrame = SamplesPerFrame;
               numSamples -= SamplesPerFrame;
           }

           else
           {
               countPerFrame = numSamples;
               numSamples = 0;
           }

           Sensor_sendHeader(SENSOR_IDX_PIR, countPerFrame);

           gSendPIRData = countPerFrame;

           /* ADC ISR handles the data streaming part */
           while(gSendPIRData){};

           /* Send End Byte */
           DL_UART_transmitDataBlocking(UART_DAP_INST, FRAME_END_BYTE);
    }

    DL_Timer_stopCounter(SAMPLING_TIMER_INST);

    return;
}

/**
 * @brief Captures temperature data from HDC3020 sensor
 * @param[in] numSamples   Number of temperature samples to capture
 */
static void Sensor_Temperature_captureData(uint32_t numSamples)
{
    hdc3020_config_t config;
    hdc3020_data_t data;
    
    int ret;

    /* Configurable based on user preference */
    config.mode = HDC3020_MODE_AUTO_MEASUREMENT;
    config.lpm = HDC3020_LPM_0;
    config.rate = HDC3020_RATE_10HZ;

    /* Initialize the sensor */
    ret = hdc3020_init(&config);
    if (ret != HDC3020_OK) {
     __BKPT(0);
    }

    /* Wait for 1 second before reading */
    delay_cycles(MS_TO_CYCLES(1000));

    uint8_t SamplesPerFrame = 1;
    uint8_t countPerFrame = 0;

    while (numSamples) {
       if(numSamples >= SamplesPerFrame)
       {
           countPerFrame = SamplesPerFrame;
           numSamples -= SamplesPerFrame;
       }

       else
       {
           countPerFrame = numSamples;
           numSamples = 0;
       }

       Sensor_sendHeader(SENSOR_IDX_TEMP, countPerFrame);

       while(countPerFrame)
       {
            ret = hdc3020_get_data(&data,HDC3020_SENSOR_TEMPERATURE);

            if (ret == HDC3020_OK && data.valid)
            {
                Sensor_sendData(&data.temperature, gSensors[SENSOR_IDX_TEMP]->format);
                countPerFrame--;

                /* Modify this value based on conversion rate */
                delay_cycles(MS_TO_CYCLES(100));    //For 10Hz, 100ms
            }
       }

       /* Send End Byte */
       DL_UART_transmitDataBlocking(UART_DAP_INST, FRAME_END_BYTE);
    }
}

/**
 * @brief Captures humidity data from HDC3020 sensor
 * @param[in] numSamples   Number of humidity samples to capture
 */
static void Sensor_Humidity_captureData(uint32_t numSamples)
{
    hdc3020_config_t config;
    hdc3020_data_t data;
    
    int ret;


    /* Configurable based on user preference */
    config.mode = HDC3020_MODE_AUTO_MEASUREMENT;
    config.lpm = HDC3020_LPM_0;
    config.rate = HDC3020_RATE_10HZ;

    /* Initialize the sensor */
    ret = hdc3020_init(&config);
    if (ret != HDC3020_OK) {
     __BKPT(0);
    }

    /* Wait for 1 second before reading */
    delay_cycles(MS_TO_CYCLES(1000));

    uint8_t SamplesPerFrame = 1;
    uint8_t countPerFrame = 0;

    while (numSamples) {
       if(numSamples >= SamplesPerFrame)
       {
           countPerFrame = SamplesPerFrame;
           numSamples -= SamplesPerFrame;
       }

       else
       {
           countPerFrame = numSamples;
           numSamples = 0;
       }

       Sensor_sendHeader(SENSOR_IDX_HUMID, countPerFrame);

       while(countPerFrame)
        {
            ret = hdc3020_get_data(&data,HDC3020_SENSOR_HUMIDITY);

            if (ret == HDC3020_OK && data.valid)
            {
                Sensor_sendData(&data.humidity, gSensors[SENSOR_IDX_HUMID]->format);
                countPerFrame--;
                /* Modify this value based on conversion rate */
                delay_cycles(MS_TO_CYCLES(100));    //For 10Hz, 100ms
            }
        }

       /* Send End Byte */
       DL_UART_transmitDataBlocking(UART_DAP_INST, FRAME_END_BYTE);
    }
}

/**
 * @brief Captures ambient light data from OPT4001 sensor
 * @param[in] numSamples   Number of light samples to capture
 */
static void Sensor_AmbientLight_captureData(uint32_t numSamples)
{
    int ret;
    opt4001_data_t data;
    opt4001_config_t config;

    /* Configurable based on user preference */
    config.mode = OPT4001_MODE_CONTINUOUS;
    config.convTime = OPT4001_CONV_TIME_100MS;
    config.range = OPT4001_RANGE_AUTO;

    /* Initialize the sensor */
    ret = opt4001_init(&config);
    if (ret != OPT4001_OK) {
        __BKPT(0);
    }

    /* Wait for 1 second before reading */
    delay_cycles(MS_TO_CYCLES(1000));

    uint8_t SamplesPerFrame = 1;
    uint8_t countPerFrame = 0;
    while (numSamples) {

        if(numSamples >= SamplesPerFrame)
        {
            countPerFrame = SamplesPerFrame;
            numSamples -= SamplesPerFrame;
        }

        else
        {
            countPerFrame = numSamples;
            numSamples = 0;
        }

        Sensor_sendHeader(SENSOR_IDX_ALS, countPerFrame);

        while(countPerFrame)
        {
            ret = opt4001_get_data(&data);

            if (ret == OPT4001_OK && data.valid) {
                uint32_t adcCode = data.mantissa << data.exponent;
                Sensor_sendData(&adcCode, gSensors[SENSOR_IDX_ALS]->format);
                countPerFrame--;

                /* Modify this value based on conversion rate */
                delay_cycles(MS_TO_CYCLES(100));
            }
        }
        /* Send End Byte */
        DL_UART_transmitDataBlocking(UART_DAP_INST, FRAME_END_BYTE);
    }
}

/**
 * @brief Captures pressure data from BMP384 sensor
 * @param[in] numSamples   Number of pressure samples to capture
 */
static void Sensor_Pressure_captureData(uint32_t numSamples)
{
    bmp384_config_t config;
    bmp384_data_t data;
    int ret;

    /* Configurable based on user preference */
    config.mode = BMP384_MODE_NORMAL;
    config.pressOsr = BMP384_OSR_x4;
    config.odr = BMP384_ODR_50HZ;
    config.filter = BMP384_FILTER_3;
    config.pressEn = true;
    config.intMode = BMP384_INT_DISABLE;

    /* Initialize the sensor */
    ret = bmp384_init(&config);
    if (ret != BMP384_OK) {
        __BKPT(0);
    }

    uint8_t SamplesPerFrame = 1;
    uint8_t countPerFrame = 0;
    while (numSamples) {

        if(numSamples >= SamplesPerFrame)
        {
            countPerFrame = SamplesPerFrame;
            numSamples -= SamplesPerFrame;
        }

        else
        {
            countPerFrame = numSamples;
            numSamples = 0;
        }

        Sensor_sendHeader(SENSOR_IDX_PRESS, countPerFrame);

        while(countPerFrame)
        {
            ret = bmp384_get_data(&data);

            if (ret == BMP384_OK && data.valid)
            {
                Sensor_sendData(&data.pressure, gSensors[SENSOR_IDX_PRESS]->format);
                countPerFrame--;
                /* Modify this value based on conversion rate */
                delay_cycles(MS_TO_CYCLES(20));     //For 50Hz, 20ms
            }
        }
        /* Send End Byte */
        DL_UART_transmitDataBlocking(UART_DAP_INST, FRAME_END_BYTE);
    }
}

/**
 * @brief Initializes and configures ICS43434 digital microphone
 * @param[in] numSamples   Number of audio samples to capture
 */
static void Sensor_DigitalMic_captureData(uint32_t numSamples)
{
    /* Enable I2S */
    DL_I2S_enable(I2S_0_INST);

    uint32_t rxData;

    uint8_t SamplesPerFrame = 20;
    uint8_t countPerFrame = 0;
    while (numSamples) {

        if(numSamples >= SamplesPerFrame)
        {
            countPerFrame = SamplesPerFrame;
            numSamples -= SamplesPerFrame;
        }

        else
        {
            countPerFrame = numSamples;
            numSamples = 0;
        }

        Sensor_sendHeader(SENSOR_IDX_MIC_DIG, countPerFrame);

        while(countPerFrame)
        {

            /* Read I2S Rx DATA */
            /* Sending only 16 bits of data */
            rxData = DL_I2S_receiveDataBlocking32(I2S_0_INST);

            Sensor_sendData(&rxData, gSensors[SENSOR_IDX_MIC_DIG]->format);
            countPerFrame--;

        }

        /* Send End Byte */
        DL_UART_transmitDataBlocking(UART_DAP_INST, FRAME_END_BYTE);
    }
}

/**
 * @brief Captures 3-axis accelerometer data from BMI270 sensor
 * @param[in] numSamples   Number of accelerometer samples to capture
 */
static void Sensor_Accelerometer_captureData(uint32_t numSamples)
{
    int ret;
    bmi270_config_t config;
    bmi270_accel_t data;

    /* Configure BMI270 for accelerometer only */
    config.powerMode = BMI270_PWR_MODE_NORMAL;
    config.sensorEnable = BMI270_SENSOR_ACC;  //Enable only accelerometer
    config.accRange = BMI270_ACC_RANGE_8G;
    config.accOdr = BMI270_ODR_100HZ;
    config.accBwp = BMI270_BWP_NORMAL;
    config.accFilterPerf = true;

    /* Initialize the sensor */
    ret = bmi270_init(&config);
    if (ret != BMI270_OK) {
        __BKPT();
    }

    /* Configure data ready interrupt on INT1 pin */
    ret = bmi270_configure_interrupt(BMI270_INT_DATA_RDY, BMI270_INT_PIN_1, true);
    if (ret != BMI270_OK) {
        __BKPT();
    }

    uint8_t SamplesPerFrame = 1;
    uint8_t countPerFrame = 0;
    while (numSamples) {

        if(numSamples >= SamplesPerFrame)
        {
            countPerFrame = SamplesPerFrame;
            numSamples -= SamplesPerFrame;
        }

        else
        {
            countPerFrame = numSamples;
            numSamples = 0;
        }

        Sensor_sendHeader(SENSOR_IDX_ACCEL, countPerFrame);

        while(countPerFrame)
        {
            /* Wait till the data is ready */
            while(gDataReady_Acc_Gyro == false){};
            gDataReady_Acc_Gyro = false;

            /* Read accelerometer data */
            ret = bmi270_get_accel(&data);
            if (ret == BMI270_OK) {
                Sensor_sendData(&data.xRaw, gSensors[SENSOR_IDX_ACCEL]->format);
                Sensor_sendData(&data.yRaw, gSensors[SENSOR_IDX_ACCEL]->format);
                Sensor_sendData(&data.zRaw, gSensors[SENSOR_IDX_ACCEL]->format);
                countPerFrame--;
            }
        }

        /* Send End Byte */
        DL_UART_transmitDataBlocking(UART_DAP_INST, FRAME_END_BYTE);
    }
}

/**
 * @brief Captures 3-axis gyroscope data from BMI270 sensor
 * @param[in] numSamples   Number of gyroscope samples to capture
 */
static void Sensor_Gyro_captureData(uint32_t numSamples)
{
    int ret;
    bmi270_config_t config;
    bmi270_gyro_t data;

    /* Configure BMI270 for gyroscope only */
    config.powerMode = BMI270_PWR_MODE_NORMAL;
    config.sensorEnable = BMI270_SENSOR_GYR;  // Only enable gyroscope
    config.gyrRange = BMI270_GYR_RANGE_2000DPS;
    config.gyrOdr = BMI270_ODR_100HZ;
    config.gyrBwp = BMI270_BWP_NORMAL;
    config.gyrFilterPerf = true;
    config.gyrNoisePerf = false;  // Set to true for lower noise (higher power)

    /* Initialize BMI270 */
    ret = bmi270_init(&config);
    if (ret != BMI270_OK) {
        __BKPT();
    }

    /* Configure data ready interrupt on INT1 pin */
    ret = bmi270_configure_interrupt(BMI270_INT_DATA_RDY, BMI270_INT_PIN_1, true);
    if (ret != BMI270_OK) {
        __BKPT();
    }

    uint8_t SamplesPerFrame = 1;
    uint8_t countPerFrame = 0;
    while (numSamples) {

        if(numSamples >= SamplesPerFrame)
        {
            countPerFrame = SamplesPerFrame;
            numSamples -= SamplesPerFrame;
        }

        else
        {
            countPerFrame = numSamples;
            numSamples = 0;
        }

        Sensor_sendHeader(SENSOR_IDX_GYRO, countPerFrame);

        while(countPerFrame)
        {

            /* Wait till the data is ready */
            while(gDataReady_Acc_Gyro == false){};
            gDataReady_Acc_Gyro = false;

            /* Read gyroscope data */
            ret = bmi270_get_gyro(&data);
            if (ret == BMI270_OK) {
                Sensor_sendData(&data.xRaw, gSensors[SENSOR_IDX_GYRO]->format);
                Sensor_sendData(&data.yRaw, gSensors[SENSOR_IDX_GYRO]->format);
                Sensor_sendData(&data.zRaw, gSensors[SENSOR_IDX_GYRO]->format);
                countPerFrame--;
            }
        }

        /* Send End Byte */
        DL_UART_transmitDataBlocking(UART_DAP_INST, FRAME_END_BYTE);
    }
}

/**
 * @brief Captures magnetic field data from TMAG5170 sensor
 * @param[in] numSamples   Number of magnetic field samples to capture
 */
static void Sensor_MagneticField_captureData(uint32_t numSamples)
{
    int ret;
    tmag5170_config_t config;
    tmag5170_data_t data;

    /* Configure TMAG5170 sensor */
    config.operatingMode = TMAG5170_MODE_ACTIVE_MEASURE;
    config.magRange = TMAG5170_RANGE_50mT;
    config.chEn = TMAG5170_CH_XYZ;           /* Enable all three axes */
    config.convAvg = TMAG5170_CONV_AVG_4;    /* Average 4 samples */
    config.dataRate = 2;                     /* 1kHz sampling rate */
    config.crcEn = false;                     /* Disable CRC */
    config.lowPower = false;                 /* Normal power mode */

    /* Initialize the sensor */
    ret = tmag5170_init(&config);
    if (ret != TMAG5170_OK) {
        __BKPT(0);
    }

    /* Configure data ready interrupt */
    ret = tmag5170_set_data_ready_interrupt(true);
    if (ret != TMAG5170_OK) {
        __BKPT(0);
    }

    /* Wait for sensor to stabilize */
    delay_cycles(MS_TO_CYCLES(10));


    uint8_t SamplesPerFrame = 1;
    uint8_t countPerFrame = 0;
    while (numSamples) {

        if(numSamples >= SamplesPerFrame)
        {
            countPerFrame = SamplesPerFrame;
            numSamples -= SamplesPerFrame;
        }

        else
        {
            countPerFrame = numSamples;
            numSamples = 0;
        }

        Sensor_sendHeader(SENSOR_IDX_MAG, countPerFrame);

        while(countPerFrame)
        {
            /* Wait for the flag to be set */
            while(gDataReady_HallEffect == 0) {}
            gDataReady_HallEffect = 0;

            /* Read magnetic sensor data */
            ret = tmag5170_get_data(&data);

            if (ret == TMAG5170_OK && data.valid)
            {
                /* Send X, Y, Z raw data */
                Sensor_sendData(&data.xRaw, gSensors[SENSOR_IDX_MAG]->format);
                Sensor_sendData(&data.yRaw, gSensors[SENSOR_IDX_MAG]->format);
                Sensor_sendData(&data.zRaw, gSensors[SENSOR_IDX_MAG]->format);

                countPerFrame--;
            }
        }

        /* Send End Byte */
        DL_UART_transmitDataBlocking(UART_DAP_INST, FRAME_END_BYTE);
    }
}

/**
 * @brief Configures appropriate sensor based on index and captures data
 * @param[in] index        Sensor index 
 * @param[in] numSamples   Number of data samples to send
 */
void Sensor_AcquireSamples(SensorIndex index, uint32_t numSamples)
{
    switch(index) {
    case SENSOR_IDX_PIR:
             Sensor_PIR_captureData(numSamples);
             break;
    case SENSOR_IDX_TEMP:
             Sensor_Temperature_captureData(numSamples);
             break;
    case SENSOR_IDX_HUMID:
             Sensor_Humidity_captureData(numSamples);
             break;
    case SENSOR_IDX_ALS:
             Sensor_AmbientLight_captureData(numSamples);
             break;
    case SENSOR_IDX_PRESS:
             Sensor_Pressure_captureData(numSamples);
             break;

    /* Support for the following sensors will be added in the next release */        
    case SENSOR_IDX_MIC_DIG:
              Sensor_DigitalMic_captureData(numSamples);
             break;
    case SENSOR_IDX_MIC_ANA:
             break;
    case SENSOR_IDX_ACCEL:
              Sensor_Accelerometer_captureData(numSamples);
             break;
    case SENSOR_IDX_GYRO:
              Sensor_Gyro_captureData(numSamples);
             break;
    case SENSOR_IDX_MAG:
              Sensor_MagneticField_captureData(numSamples);
             break;

    }
}
