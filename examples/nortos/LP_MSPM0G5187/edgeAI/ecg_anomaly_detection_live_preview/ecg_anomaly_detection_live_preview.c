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

#include "ti_msp_dl_config.h"
#include <feature_extract.h>

/* UART communication and DAP (Debug Access Port) core headers */
#include "uart_comm_target.h"
#include "dap_core.h"

#include <stdint.h>
#include <stdbool.h>  /* For boolean datatype support */
#include <string.h>   /* For memory operations */

#include "H_AFE1594_functions.h"
#include "H_AFE1594_preset_modes.h"
#include "H_AFE1594_SPI_functions.h"
#include "model/tvmgen_default.h"

/** \brief Size of the class buffer */
#define SAMPLE_BUFFER_SIZE (10)

/** \brief Represents the index after the historical window (TINIE_INPUT_SIZE - 125) */
#define OFFSET (2375)

/** \brief Uart handler for DAP operations */
UART_Instance gUART;

/** \brief Output buffer for live preview data */
uint8_t livePrev_Output[5];

/** \brief Detected class for live preview */
volatile uint8_t livePrev_detClass = 0;

/** \brief Output vector from the model inference (4 classes) */
int8_t output_vector[4];

/** \brief Buffer to store ECG data for processing */
short ECG1data[TINIE_INPUT_SIZE];

/** \brief Temporary storage for ECG data during processing */
short ECG_intermidiant_data;

/** \brief normalized input feature vector **/
volatile int8_t input_data[TINIE_INPUT_SIZE];

/** \brief TVM input structure for model */
struct tvmgen_default_inputs inputs = {(void*) &input_data[0]};

/** \brief TVM output structure for model results */
struct tvmgen_default_outputs outputs = {(void*)&output_vector[0]};

/** \brief Circular class buffer to determine which class LED has to glow */
uint8_t class_buffer[SAMPLE_BUFFER_SIZE];

/** \brief Index for the class buffer */
uint8_t class_buffer_index = 0;

/** data ready flag which is set by AFE sensor interrupt" */
volatile int dataRDY = 0;

/**
 *  @brief     Converts the data type of the extracted features to signed 8-bit integer.
 *
 *  @param[in]  inputBuffer     A pointer to the input buffer.
 *  @param[in]  varIndex        The index of the variable being processed for feature extraction.
 *  @param[out] outputBuffer    A pointer to the output buffer.
 *
 *  @details    This function converts the data type of the extracted features to signed 8-bit integer.
 *              The function uses value generated from tvmgen_default.h file to scale the data.
 *
 */
void data_pre_processing(q15_t * inputBuffer, int8_t * outputBuffer, uint8_t varIndex)
{
    for(int i = 0; i < FE_FEATURE_SIZE_PER_FRAME ; i++)
    {
        q31_t inputVal_q31 = (q31_t) inputBuffer[i];

        inputVal_q31 = ((int32_t)((inputVal_q31 + tvmgen_default_bias_data[varIndex]) * tvmgen_default_scale_data[varIndex])) \
                >> tvmgen_default_shift_data[varIndex];

        if(inputVal_q31 > 127)
        {
            inputVal_q31 = 127;
        }
        else if(inputVal_q31 < -128)
        {
            inputVal_q31 = -128;
        }

        outputBuffer[i] = (int8_t) inputVal_q31;
    }
}

/** \brief Append output class with maximum frequency to the buffer */
void add_output_class()
{
    /* Find class with maximum +ve value */
    uint8_t output_class = 0;
    for(int i=1;i<4;i++){
        if(output_vector[output_class]<output_vector[i]){
            output_class = i;
        }
    }

    class_buffer[class_buffer_index] = output_class + 1;
    class_buffer_index = (class_buffer_index + 1)%SAMPLE_BUFFER_SIZE;
}

/** \brief Glow LED based on which class has the maximum frequency */
void glow_led()
{
    uint8_t freq[4] = {0, 0, 0, 0};

    /* Calculate the frequency of the classes */
    for(int i=0;i<SAMPLE_BUFFER_SIZE;i++)
    {
        if(class_buffer[i])
        {
            freq[class_buffer[i]-1]++;
        }
    }

    /* Find class with maximum frequency */
    uint8_t maximum_freq_class = 0;
    for(int i=1;i<4;i++){
        if(freq[maximum_freq_class]<freq[i]){
            maximum_freq_class = i;
        }
    }

    livePrev_detClass = maximum_freq_class;

    /* Run only if the buffer is completely filled */
    if(freq[0]+freq[1]+freq[2]+freq[3] == SAMPLE_BUFFER_SIZE)
    {
        DL_GPIO_clearPins(GPIO_LEDS_LED1_PORT, GPIO_LEDS_LED1_PIN);
        DL_GPIO_clearPins(GPIO_LEDS_LED2_PORT, GPIO_LEDS_LED2_PIN);
        DL_GPIO_clearPins(GPIO_LEDS_LED3_PORT, GPIO_LEDS_LED3_PIN);

        switch(maximum_freq_class)
        {
            case 0:
                DL_GPIO_setPins(GPIO_LEDS_LED3_PORT, GPIO_LEDS_LED3_PIN);
                break;
            case 1:
                DL_GPIO_setPins(GPIO_LEDS_LED2_PORT, GPIO_LEDS_LED2_PIN);
                DL_GPIO_setPins(GPIO_LEDS_LED3_PORT, GPIO_LEDS_LED3_PIN);
                break;
            case 2:
                DL_GPIO_setPins(GPIO_LEDS_LED2_PORT, GPIO_LEDS_LED2_PIN);
                DL_GPIO_setPins(GPIO_LEDS_LED3_PORT, GPIO_LEDS_LED3_PIN);
                break;
            case 3:
                DL_GPIO_setPins(GPIO_LEDS_LED3_PORT, GPIO_LEDS_LED2_PIN);
                break;
            default:
                break;
        }
    }
}

/**
 * \brief Main application function
 *
 * Initializes hardware, processes ECG data directly from class data arrays
 * through the AI model for classification.
 */
int main(void)
{
    struct tvmgen_default_inputs inputs;
    int8_t input_data[TINIE_INPUT_SIZE];
    q15_t* current_data;

    SYSCFG_DL_init();

    NVIC_EnableIRQ(DMA_INT_IRQn);

    UART_init(&gUART);

    DL_DMA_clearInterruptStatus(DMA, DL_DMA_FULL_CH_INTERRUPT_EARLY_CHANNEL1);
    DL_DMA_enableInterrupt(DMA, DL_DMA_FULL_CH_INTERRUPT_EARLY_CHANNEL1);
    DL_DMA_Full_Ch_setEarlyInterruptThreshold(DMA, DMA_CH0_CHAN_ID, DL_DMA_EARLY_INTERRUPT_THRESHOLD_64);


    /* Power up the neural processing unit (NPU) module.
     * Clear and enable the NPU interrupt.
     * NPU interrupts will be handled inside the NPU
     * library created during model compilation (outside of this project).
     * NOTE: In future releases, the NPU initialization calls made below
     * may be transitioned to SysConfig generated calls placed in
     * the auto-generatead ti_msp_dl_config.h/.c files, at which point
     * they will be handled inside of the SYSCFG_DL_init() call.
     */
    DL_NPU_reset(NPU);
    DL_NPU_enablePower(NPU);
    while (!(DL_SYSCTL_getStatus() & DL_SYSCTL_STATUS_NPU_READY));
    DL_NPU_clearInterruptStatus(NPU, DL_NPU_INTERRUPT_DONE);
    DL_NPU_enableInterrupt(NPU, DL_NPU_INTERRUPT_DONE);
    NVIC_EnableIRQ(NPU_INT_IRQn);

    /* Set GPIO2 interrupt for FIFO data ready*/
    NVIC_EnableIRQ(AFE_INT_IRQN);

    /* Initialize the SPI drivers */
    AFE_SPI_setup();

    /* Hardware Reset AFE */
    AFE_hardware_reset();

    /* Select mode configuration to program registers ======== Only select 1 */
    mode_ECG();

    /* INTERRUPTS =============== GPIO setup GPIO2 FIFO reached threshold */
    AFE_setup_GPIO();

    /* FIFO setup ============== Set FIFO mask values for SPI transfer ============================= */
    AFE_setup_FIFO();

    /* Delay 1 ms before starting */
    delay_cycles(80000);

    /* Main processing loop */
    /* If write and read were successful, toggle LED */
    while (1)
    {
        /* DAP: Check for incoming UART commands */
        UART_checkForCommand(&gUART);

        if(gUART.RxStatus == UART_STATUS_PROCESS_FRAME)
        {
            DAP_processBuffer(&gUART);
            gUART.RxStatus = UART_STATUS_IDLE;
            DMA_RX_init(&gUART);
        }

        if(gStartStream)
        {
            if(gPipelineConfig.mode == PIPELINE_MODE_SENSOR_INFERENCE)
            {
                /* Prepare and send live preview output over UART */
                livePrev_Output[0] = livePrev_detClass;
                livePrev_Output[1] = (int8_t) output_vector[0] ;
                livePrev_Output[2] = (int8_t) output_vector[1] ;
                livePrev_Output[3] = (int8_t) output_vector[2] ;
                livePrev_Output[4] = (int8_t) output_vector[3] ;

                DAP_receivedDataResponse(&gUART, Resp_CMD_Channel_inferencing_value, 5, &livePrev_Output[0]);
            }
        }

        if(dataRDY)
        {
            /* Get data */
            AFE_FIFO_readout(TRANSMIT_ADDRESS,TOTAL_FIFO_SPI_FRAMES);

            dataRDY = 0;
            /* Shift ECG array to make room for new data */
            for(short i=0; i<(TINIE_INPUT_SIZE-125); i+=125)
            {
                memmove(&ECG1data[i],&ECG1data[i+125],250);
            }

            /* Process captured data and place at end of buffer */
            for(short i=0; i<(TOTAL_FIFO_SPI_FRAMES); i++)
            {
                /* Reconstruct 16-bit ECG sample from two 8-bit SPI bytes (Big-Endian) */
                ECG_intermidiant_data = (uint16_t)(receiveBuffer[3*i+5]<<8)|(uint16_t)(receiveBuffer[3*i+6]<<0);
                
                /* Invert the signal: Required if the AFE or leads are in an inverted configuration */
                ECG_intermidiant_data *= -1;

                /* Store the processed sample at the buffer's tail (starting at OFFSET) */
                ECG1data[OFFSET+i] = ECG_intermidiant_data;
            }

            current_data = (q15_t*) ECG1data;

            /* Feature extraction */
            data_pre_processing(current_data, input_data,0);

            /* Set up input to point directly to the class data */
            inputs.onnx__Add_0 = (void *)input_data;

            /* Run inference on the ECG data */
            (void)tvmgen_default_run(&inputs, &outputs);

            /* Wait for the model to process data */
            while(!tvmgen_default_finished);

            /* Add the output class with maximum value to the class_buffer */
            add_output_class();

            /* Glow the respective led for the class with maximum frequency */
            glow_led();

        }
    }
}

/**
 * \brief Interrupt handler for GROUP1 interrupts
 *
 * Processes interrupts from the AFE1594 sensor when data is ready.
 * Sets the dataRDY flag to trigger data processing in the main loop.
 */
void GROUP1_IRQHandler(void)
{
    /* Check if interrupt from AFE GPIO pin */
    if (DL_GPIO_readPins(AFE_PORT,AFE_GPIO_2_PIN)) {
        /* SPI capture 125 data frames from FIFO */
        dataRDY = 1;
    }
}

/** \brief DMA interrupt handler for UART/DAP data transfer */
void DMA_IRQHandler(void)
{
    switch (DL_DMA_getPendingInterrupt(DMA))
    {
        case DL_DMA_FULL_CH_EVENT_IIDX_EARLY_IRQ_DMACH1:
            DAP_getFrameLength(&gUART);
            gUART.RxStatus = UART_STATUS_RX_BUFFERING;
            break;

        case DL_DMA_EVENT_IIDX_DMACH0:
            gUART.TxStatus = UART_STATUS_IDLE;
            break;

        default:
            break;
    }
}
