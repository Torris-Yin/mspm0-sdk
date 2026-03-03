/*
 * Copyright (c) 2026, Texas Instruments Incorporated
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

/*
 * Revision: 1.0
 */

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <ti/iqmath/include/IQmathLib.h>
#include "calc.h"
#include "crc.h"
#include "ti_msp_dl_config.h"

#define PI 3.141592654
#define N1 16  // Fine coil electrical periods
#define N2 15  // Coarse coil electrical periods

/*
 * Global variables
 */
uint8_t sector = 0;

volatile uint16_t gAdcResult;

volatile SinCosIn SinCosInput;

// Test & debug
int ADC0_counter = 0;
int ADC1_counter = 0;

// Global variables, default IQ data format IQ24
_iq angle            = _IQ(0);
_iq angle_abs        = _IQ(0);
_iq fine_angle       = _IQ(0);
_iq fine_angle_abs   = _IQ(0);
_iq coarse_angle     = _IQ(0);
_iq coarse_angle_abs = _IQ(0);
_iq diff             = _IQ(16);

_iq21 angle_abs_IQ21 = _IQ21(0);  // Reduce resolution for UART transmission

float fine_angle_f    = 0;
float coarse_angle_f  = 0;
float sector_f        = 0.0;
float angle_abs_360_f = 0.0;

volatile uint16_t timer_counter         = 0;  // test & debug
volatile uint16_t backgoundloop_counter = 0;  // test & debug

/* UART related parameter definition*/
#define UART_TX_DELAY (1600)  // 160000 for 5ms 1600 for 50us

/* Number of bytes for UART packet size */
#define UART_TX_PACKET_SIZE (6)
#define UART_RX_PACKET_SIZE (1)

/* UART transmit and receive data */
volatile uint8_t gRxPacket[UART_RX_PACKET_SIZE];
volatile uint8_t gTxPacket[UART_TX_PACKET_SIZE];

/* CRC related parameter*/
#define CRCData_Length 10  // CF+SF+DF0+DF1+DF2+DF2;
uint8_t CRCdata[10];

volatile bool gCheckUART, gDMADone, Rx_BUS, Tx_BUS;

const DL_MathACL_operationConfig gAtanOpConfig = {
    .opType      = DL_MATHACL_OP_TYPE_ARCTAN2,
    .opSign      = DL_MATHACL_OPSIGN_UNSIGNED,
    .iterations  = 40,
    .scaleFactor = 0,
    .qType       = DL_MATHACL_Q_TYPE_Q21};

int main(void)
{
    _iq buffer1, buffer2;
    uint16_t i, j;
    SYSCFG_DL_init();

#if 0
      //DL_SYSCTL_enableSleepOnExit();

      DL_GPIO_clearPins(GPIO_RS485_PORT, GPIO_RS485_DIR_PIN); //485 initial state: receive
      DL_GPIO_setPins(GPIO_RS485_PORT, GPIO_RS485_TERM_PIN); // enable termination for THVD1454
#endif

    NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);
    NVIC_EnableIRQ(ADC12_1_INST_INT_IRQN);
    NVIC_EnableIRQ(TIMER_1_PosCal_INST_INT_IRQN);
    NVIC_EnableIRQ(UART_0_INST_INT_IRQN);

    DL_TimerG_startCounter(TIMER_0_ADC_INST);
    DL_TimerG_startCounter(TIMER_1_PosCal_INST);

    //UART DMA config
    DL_DMA_setSrcAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t)(&UART_0_INST->RXDATA));
    DL_DMA_setDestAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t) &gRxPacket[0]);
    DL_DMA_setTransferSize(DMA, DMA_CH1_CHAN_ID, 1);
    DL_DMA_enableChannel(
        DMA, DMA_CH1_CHAN_ID);  // enable Rx DMA channel, wait for first Rx

    while (1) {
        // Background loop
        backgoundloop_counter++;
    }
}

void ADC12_0_INST_IRQHandler(void)
{
    switch (DL_ADC12_getPendingInterrupt(ADC12_0_INST)) {
        case DL_ADC12_IIDX_MEM1_RESULT_LOADED:
            SinCosInput.RawData1.Adc[0] = DL_ADC12_getMemResult(
                ADC12_0_INST, DL_ADC12_MEM_IDX_0);  //A0_0 for fine coil COS
            SinCosInput.RawData2.Adc[0] = DL_ADC12_getMemResult(
                ADC12_0_INST, DL_ADC12_MEM_IDX_1);  //A0_1 for coarse coil COS
            break;
        default:
            break;
    }
}

void ADC12_1_INST_IRQHandler(void)
{
    switch (DL_ADC12_getPendingInterrupt(ADC12_1_INST)) {
        case DL_ADC12_IIDX_MEM1_RESULT_LOADED:
            SinCosInput.RawData1.Adc[1] = DL_ADC12_getMemResult(
                ADC12_1_INST, DL_ADC12_MEM_IDX_0);  //A1_0 for fine coil SIN
            SinCosInput.RawData2.Adc[1] = DL_ADC12_getMemResult(
                ADC12_1_INST, DL_ADC12_MEM_IDX_1);  //A1_1 for coarse coil SIN
            break;
        default:
            break;
    }
}

// TIMER1 -- calculate angle position
void TIMER_1_PosCal_INST_IRQHandler(void)
{
    timer_counter++;  // Timer interrupt counter
    //----------------------------------------------------------------------
    // Angle calculation
    //----------------------------------------------------------------------

    // read over sampled ADC data and compensate SIN/COS gain and offset
    SinCosInput.SinCos1.Sin = _IQ12toIQ(SinCosInput.RawData1.Adc[1] - 2083);
    SinCosInput.SinCos1.Cos =
        _IQmpy(_IQ12toIQ(SinCosInput.RawData1.Adc[0] - 2003), _IQ(0.997));
    SinCosInput.SinCos2.Sin = _IQ12toIQ(SinCosInput.RawData2.Adc[1] - 1956);
    SinCosInput.SinCos2.Cos =
        _IQmpy(_IQ12toIQ(SinCosInput.RawData2.Adc[0] - 2114), _IQ(0.986));

    // atan2 calculation fine angle
    fine_angle = _IQatan2PU(SinCosInput.SinCos1.Sin, SinCosInput.SinCos1.Cos);
    if (fine_angle < 0)
        fine_angle_abs = fine_angle + _IQ(1);
    else
        fine_angle_abs = fine_angle;

    fine_angle_f = _IQtoF(fine_angle_abs);

    // atan2 calculation coarse angle
    coarse_angle =
        _IQatan2PU(SinCosInput.SinCos2.Sin, SinCosInput.SinCos2.Cos);
    if (coarse_angle < 0)
        coarse_angle_abs = coarse_angle + _IQ(1);
    else
        coarse_angle_abs = coarse_angle;
    coarse_angle_f = _IQtoF(coarse_angle_abs);

    if (fine_angle_abs > coarse_angle_abs)
        sector_f = _IQtoF(_IQmpy((fine_angle_abs - coarse_angle_abs), diff));
    else
        sector_f =
            _IQtoF(_IQmpy((fine_angle_abs - coarse_angle_abs + _IQ(1)), diff));

    // Absolute mechanical angle calculation (Nonius)
    if ((sector_f - round(sector_f)) > -0.1 &&
        (sector_f - round(sector_f)) < 0) {
        if (fine_angle_abs < coarse_angle_abs)
            sector = round(sector_f);
        else
            sector = round(sector_f) - 1;
    } else
        sector = floor(sector_f);
    if (sector == 16) {
        if (fine_angle_f > 0.5)
            sector = 15;
        else
            sector = 0;
    }

    angle = (fine_angle_abs >> 4) + _IQmpy(_IQ(0.0625), _IQ(sector));
    if (angle < 0)
        angle_abs = angle + _IQ(1);
    else
        angle_abs = angle;
    angle_abs_IQ21  = _IQtoIQ21(angle_abs);  // converter to IQ21
    angle_abs_360_f = _IQtoF(angle_abs) * 360;
}

/*
 * UART transmission
*/

void UART_0_INST_IRQHandler(void)
{
    switch (DL_UART_Main_getPendingInterrupt(UART_0_INST)) {
        case DL_UART_MAIN_IIDX_DMA_DONE_TX:
            DL_DMA_setSrcAddr(
                DMA, DMA_CH1_CHAN_ID, (uint32_t)(&UART_0_INST->RXDATA));
            DL_DMA_setDestAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t) &gRxPacket[0]);
            DL_DMA_setTransferSize(DMA, DMA_CH1_CHAN_ID, 1);
            DL_DMA_enableChannel(DMA,
                DMA_CH1_CHAN_ID);  // enable Rx DMA channel, wait for first Rx
            break;
        case DL_UART_MAIN_IIDX_TX:
            break;
        case DL_UART_MAIN_IIDX_DMA_DONE_RX:
            switch (gRxPacket[0]) {
                case 0x1A:  // send IQ21 scaled angle data

                    gTxPacket[0] = 0x1A;                      //command field
                    gTxPacket[1] = 0x20;                      //status field
                    gTxPacket[2] = angle_abs_IQ21 & 0x000FF;  //data field0
                    gTxPacket[3] =
                        angle_abs_IQ21 >> 8 & 0x000FF;  //data field1
                    gTxPacket[4] =
                        angle_abs_IQ21 >> 16 & 0x0001F;  //data field2
                    gTxPacket[5] = 0x5A;                 //ENID

                    DL_DMA_setSrcAddr(
                        DMA, DMA_CH0_CHAN_ID, (uint32_t) &gTxPacket[0]);
                    DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID,
                        (uint32_t)(&UART_0_INST->TXDATA));
                    DL_DMA_setTransferSize(
                        DMA, DMA_CH0_CHAN_ID, UART_TX_PACKET_SIZE);
                    DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);
                    //CRCField = CalcCRC(gTxPacket, CRCData_Length);
                    //gTxPacket[10] = CRCField;                       //CRC field

                    break;
                case 0x1B:  //send fine coil SIN COS

                    gTxPacket[0] = SinCosInput.RawData1.Adc[1] & 0xFF;
                    gTxPacket[1] = (SinCosInput.RawData1.Adc[1] >> 8) & 0x0F;
                    gTxPacket[2] = SinCosInput.RawData1.Adc[0] & 0xFF;
                    gTxPacket[3] = (SinCosInput.RawData1.Adc[0] >> 8) & 0x0F;

                    DL_DMA_setSrcAddr(
                        DMA, DMA_CH0_CHAN_ID, (uint32_t) &gTxPacket[0]);
                    DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID,
                        (uint32_t)(&UART_0_INST->TXDATA));
                    DL_DMA_setTransferSize(DMA, DMA_CH0_CHAN_ID, 4);
                    DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);
                    break;

                case 0x1C:  //send coarse coil SIN COS

                    gTxPacket[0] = SinCosInput.RawData2.Adc[1] & 0xFF;
                    gTxPacket[1] = (SinCosInput.RawData2.Adc[1] >> 8) & 0x0F;
                    gTxPacket[2] = SinCosInput.RawData2.Adc[0] & 0xFF;
                    gTxPacket[3] = (SinCosInput.RawData2.Adc[0] >> 8) & 0x0F;

                    DL_DMA_setSrcAddr(
                        DMA, DMA_CH0_CHAN_ID, (uint32_t) &gTxPacket[0]);
                    DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID,
                        (uint32_t)(&UART_0_INST->TXDATA));
                    DL_DMA_setTransferSize(DMA, DMA_CH0_CHAN_ID, 4);
                    DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);
                    break;

                default:
                    DL_DMA_setSrcAddr(DMA, DMA_CH1_CHAN_ID,
                        (uint32_t)(&UART_0_INST->RXDATA));
                    DL_DMA_setDestAddr(
                        DMA, DMA_CH1_CHAN_ID, (uint32_t) &gRxPacket[0]);
                    DL_DMA_setTransferSize(DMA, DMA_CH1_CHAN_ID, 1);
                    DL_DMA_enableChannel(DMA, DMA_CH1_CHAN_ID);
                    break;
            }
        default:
            break;
    }
}
