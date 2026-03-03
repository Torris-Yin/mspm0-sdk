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

/* UART communication and DAP (Debug Access Port) core headers */
#include "uart_comm_target.h"
#include "dap_core.h"

#include <stdint.h>
#include <stdbool.h>  /* For boolean datatype support */
#include <string.h>   /* For memory operations */

#include "H_AFE1594_functions.h"
#include "H_AFE1594_preset_modes.h"
#include "H_AFE1594_SPI_functions.h"

#define OFFSET (2375)

/** \brief Uart handler for DAP operations */
UART_Instance gUART;

/** \brief Number of vibration data to be transmitted */
volatile uint32_t gSendECGData = 0;

/** \brief Size of a single data point */
uint32_t dataPointSize = 1;

volatile short ECG_intermidiant_data;

volatile short ECG1data[TINIE_INPUT_SIZE];
volatile int dataRDY = 0;

/**
 * \brief Main application function
 *
 * Initializes hardware, processes ECG data directly from class data arrays
 * through the AI model for classification.
 */
int main(void)
{
    SYSCFG_DL_init();
    
    NVIC_EnableIRQ(DMA_INT_IRQn);

    UART_init(&gUART);

    DL_DMA_clearInterruptStatus(DMA, DL_DMA_FULL_CH_INTERRUPT_EARLY_CHANNEL1);
    DL_DMA_enableInterrupt(DMA, DL_DMA_FULL_CH_INTERRUPT_EARLY_CHANNEL1);
    DL_DMA_Full_Ch_setEarlyInterruptThreshold(DMA, DMA_CH0_CHAN_ID, DL_DMA_EARLY_INTERRUPT_THRESHOLD_64);


    DL_GPIO_clearInterruptStatus(AFE_PORT, AFE_GPIO_2_PIN);

    DL_GPIO_disableInterrupt(AFE_PORT, AFE_GPIO_2_PIN);

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
    while (1)
    {
        /* DAP: Check for incoming UART commands */
        UART_checkForCommand(&gUART);

        if(gUART.RxStatus == UART_STATUS_PROCESS_FRAME)
        {
            /* Process incoming DAP frame then reset status and prepare RX */
            DAP_processBuffer(&gUART);
            gUART.RxStatus = UART_STATUS_IDLE;
            DMA_RX_init(&gUART);
        }

        /* Calculates the payload and sends it over UART */
        if(gStartStream)
        {
            if(gPipelineConfig.mode == PIPELINE_MODE_DATA_ACQUISITION)
            {
                /* Wait if UART is already transmitting data */
                while(gUART.TxStatus == UART_STATUS_TX_TRANSMITTING)
                {};

                DL_GPIO_enableInterrupt(AFE_PORT, AFE_GPIO_2_PIN);

                /* Send start byte */
                DL_UART_transmitDataBlocking(UART_DAP_INST,FRAME_START_BYTE);
                /* Send channel number */
                DL_UART_transmitDataBlocking(UART_DAP_INST, Serial_Resp_receiveData | Resp_CMD_Channel_sensor_signal);
                uint32_t payloadLen = dataPointSize*(gProperties[0]->value.u16)*getDataLen(gProperties[0]->type);
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

                gUART.TxStatus = UART_STATUS_TX_TRANSMITTING;
                gSendECGData = gProperties[0]->value.u16;

                while(gSendECGData != 0){
                    if(dataRDY == 1){

                        /* SPI capture 125 data frames from FIFO */
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

                        int16_t ecgData;

                        /* Send data over UART */
                        for(short i=0; i<(TOTAL_FIFO_SPI_FRAMES); i++)
                        {
                            ecgData = (int16_t) ECG1data[2375+i];

                            DL_UART_transmitDataBlocking(UART_DAP_INST, (ecgData >> 8) & 0xFF);
                            DL_UART_transmitDataBlocking(UART_DAP_INST, ecgData & 0xFF);

                            gSendECGData--;

                            /* Send Frame End Byte after sending last data */
                            if(gSendECGData == 0)
                            {
                                DL_UART_transmitDataBlocking(UART_DAP_INST, FRAME_END_BYTE);
                                gUART.TxStatus = UART_STATUS_IDLE;
                                break;
                            }
                        }

                    }

                }
                DL_GPIO_disableInterrupt(AFE_PORT, AFE_GPIO_2_PIN);
            }
            gStartStream = 0;
        }
    }
}

/** \brief DMA interrupt handler for UART/DAP data transfer */
void DMA_IRQHandler(void)
{
    switch (DL_DMA_getPendingInterrupt(DMA))
    {
        case DL_DMA_FULL_CH_EVENT_IIDX_EARLY_IRQ_DMACH1:
            /* Early DMA threshold reached: prepare to determine frame length */
            DAP_getFrameLength(&gUART);
            gUART.RxStatus = UART_STATUS_RX_BUFFERING;
            break;

        case DL_DMA_EVENT_IIDX_DMACH0:
            /* RX DMA finished for channel 0 - mark UART idle */
            gUART.TxStatus = UART_STATUS_IDLE;
            break;

        default:
            /* Unknown/unused DMA event */
            break;
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
    switch (DL_GPIO_getPendingInterrupt(AFE_PORT)){

        case DL_GPIO_IIDX_DIO8:
            dataRDY = 1;
            DL_GPIO_clearInterruptStatus(AFE_PORT, AFE_GPIO_2_PIN);
            break;

        default :
            break;
    }
}
