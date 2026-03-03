/*
 * Copyright (c) 2023, Texas Instruments Incorporated - http://www.ti.com
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
 * MSPM0 LIN Responder Implementation
 *
 * This file implements the LIN 2.2A responder functionality including:
 *   - Break field detection and validation
 *   - Sync field measurement and automatic baud rate synchronization
 *   - PID reception and message table lookup
 *   - Data field transmission and reception with checksum validation
 *   - Timeout handling for incomplete frames
 *
 * Supports both UART Extended and UNICOMM UART peripherals.
 */

#include "lin_config.h"

/* Diagnostic information for debugging and error tracking */
volatile LIN_diag_t gLINDiag = { 0 };

/* Timing and auto-baud rate context with default values */
volatile Lin_timing_t gLINT = {
    /* Break field timing */
    .brkW = 0,
    .brkCmp = LIN_0_COUNTER_COMPARE_VALUE,
    /* Sync field measurements */
    .syncStart = true,
    .syncBt = { 0 },
    .syncOkCnt = 0,
    .syncErrCnt = 0,
    /* Bit time calculations */
    .btSum = 0,
    .btAvg = 0,
    .btWidth = LIN_0_TBIT_WIDTH,
    .btWMax = LIN_0_TBIT_WIDTH * LIN_AUTO_BAUD_MAX / 100,
    .btWMin = LIN_0_TBIT_WIDTH * LIN_AUTO_BAUD_MIN / 100,
    /* Baud rate tracking */
    .brMeas = 0,
    .brPrev = LIN_0_BAUD_RATE,
    .brCurr = LIN_0_BAUD_RATE,
    /* Auto-baud state */
    .autoBaud = false,
    /* Response timing */
    .respDelay = LIN_RESPONSE_LAPSE
};

/* LIN transmit/receive context with default values */
Lin_TxRxCtx_t gLIN = {
    .state = LIN_STATE_WAIT_FOR_BREAK,
    .respMsgTbl = responderMessageTable,
    .msgTblIdx = 0,
    .rxIdx = 0,
    .txIdx = 1,
    .rxBuf = { 0 },
    .txBuf = { 0 },
    .chkSum.word = 0,
    .timing = &gLINT
};

/**
 * @brief   Configure LIN counter for sync field measurement
 *
 * Disables compare match, enables both capture channels,
 * and configures counter for zero-on-rising-edge operation.
 *
 * @param[in] peripheral  Pointer to LIN peripheral instance
 */
__STATIC_INLINE void LIN_switchCounterToSyncMode(LIN_Peripheral_t peripheral)
{
    DL_UART_enableLINSyncFieldValidationCounterControl(peripheral);
    DL_UART_enableLINFallingEdgeCapture(peripheral);
    DL_UART_disableLINCountWhileLow(peripheral);
}

/**
 * @brief   Configure LIN counter for break field detection
 *
 * Enables compare match for break detection, disables falling edge capture,
 * and configures counter to count while RX is low.
 *
 * @param[in] peripheral  Pointer to LIN peripheral instance
 */
__STATIC_INLINE void LIN_switchCounterToBreakMode(LIN_Peripheral_t peripheral)
{
    DL_UART_enableLINCounterCompareMatch(peripheral);
    DL_UART_Extend_enableLINCountWhileLow(peripheral);
    DL_UART_Extend_enableLINCounterClearOnFallingEdge(peripheral);
}

/**
 * @brief   Reset LIN state machine to wait for break (clean exit)
 *
 * This function provides a clean exit from any error condition:
 *   - Wrong PID received
 *   - No RX data (timeout)
 *   - Baud rate mismatch
 *   - Counter overflow during sync/data
 *
 * Resets all context, flushes FIFO, reconfigures for break detection.
 *
 * @param[in] peripheral  Pointer to LIN peripheral instance
 * @param[in] lin         Pointer to LIN context structure
 */
static void LIN_Responder_resetState(LIN_Peripheral_t peripheral, Lin_TxRxCtx_t *lin)
{
    /* Reset state machine */
    lin->state = LIN_STATE_WAIT_FOR_BREAK;
    lin->rxIdx = 0;
    lin->txIdx = 1;
    lin->chkSum.word = 0;

    /* Reset timing context */
    gLINT.brkW = 0;
    gLINT.syncOkCnt = 0;
    gLINT.syncErrCnt = 0;
    gLINT.btSum = 0;
    gLINT.syncStart = true;

    /* Flush RX FIFO */
    while (!DL_UART_isRXFIFOEmpty(peripheral))
    {
        DL_UART_Extend_receiveData(peripheral);
    }

    /* Disable all data/sync interrupts */
    DL_UART_Extend_disableInterrupt(peripheral, DL_UART_EXTEND_INTERRUPT_RX);
    DL_UART_Extend_disableInterrupt(peripheral, DL_UART_EXTEND_INTERRUPT_TX);
    DL_UART_Extend_disableInterrupt(peripheral, DL_UART_EXTEND_INTERRUPT_RXD_NEG_EDGE);
    DL_UART_Extend_disableInterrupt(peripheral, DL_UART_EXTEND_INTERRUPT_RXD_POS_EDGE);
    DL_UART_Extend_disableInterrupt(peripheral, DL_UART_EXTEND_INTERRUPT_LIN_RISING_EDGE);

    /* Reconfigure LIN counter for break detection */
    LIN_switchCounterToBreakMode(peripheral);
    DL_UART_setLINCounterCompareValue(peripheral, gLINT.brkCmp);

    /* Re-enable break detection interrupt */
    DL_UART_Extend_clearInterruptStatus(peripheral, DL_UART_EXTEND_INTERRUPT_LIN_FALLING_EDGE);
    DL_UART_Extend_enableInterrupt(peripheral, DL_UART_EXTEND_INTERRUPT_LIN_FALLING_EDGE);

    /* Stop timeout timer */
    DL_Timer_stopCounter(TIMER_0_INST);
}

/**
 * @brief   Prepare for break detection after successful frame completion
 *
 * Reconfigures LIN counter and interrupts for next break field detection.
 *
 * @param[in] peripheral  Pointer to LIN peripheral instance
 */
static void LIN_prepareForNextFrame(LIN_Peripheral_t peripheral)
{
    LIN_switchCounterToBreakMode(peripheral);
    DL_UART_setLINCounterCompareValue(peripheral, gLINT.brkCmp);
    DL_UART_Extend_clearInterruptStatus(peripheral, DL_UART_EXTEND_INTERRUPT_LIN_FALLING_EDGE);
    DL_UART_Extend_enableInterrupt(peripheral, DL_UART_EXTEND_INTERRUPT_LIN_FALLING_EDGE);
    DL_Timer_stopCounter(TIMER_0_INST);
}

/**
 * @brief   Process received Protected Identifier (PID)
 *
 * Searches the message table for a matching PID. If found, transitions
 * to DATA_FIELD state for either reception or transmission. If not found,
 * performs a clean exit to wait for next break.
 *
 * @param[in] peripheral  Pointer to LIN peripheral instance
 * @param[in] rxByte      Received PID byte
 * @param[in] lin         Pointer to LIN context structure
 */
void LIN_Responder_receivePID(LIN_Peripheral_t peripheral, uint8_t rxByte, Lin_TxRxCtx_t *lin)
{
    uint8_t tableIndex;

    /* Search message table for matching PID */
    for (tableIndex = 0; tableIndex < LIN_RESPONDER_NUM_MSGS; tableIndex++)
    {
        if (lin->respMsgTbl[tableIndex].msgID == rxByte)
        {
            lin->msgTblIdx = tableIndex;
            break;
        }
    }

    /* No matching PID found - clean exit */
    if (tableIndex == LIN_RESPONDER_NUM_MSGS)
    {
        LIN_Responder_resetState(peripheral, lin);
        return;
    }

    /* Receive mode - callback function defined */
    if (lin->respMsgTbl[tableIndex].callbackFunction != NULL)
    {
        lin->state = LIN_STATE_DATA_FIELD;
    }
    /* Transmit mode - no callback function */
    else
    {
        lin->state = LIN_STATE_DATA_FIELD;
        DL_UART_Extend_disableInterrupt(peripheral, DL_UART_EXTEND_INTERRUPT_RX);

        delay_cycles(lin->timing->respDelay);

        DL_UART_Extend_transmitData(peripheral, lin->txBuf[0]);
        DL_UART_Extend_clearInterruptStatus(peripheral, DL_UART_EXTEND_INTERRUPT_TX);
        DL_UART_Extend_enableInterrupt(peripheral, DL_UART_EXTEND_INTERRUPT_TX);
        DL_Timer_setTimerCount(TIMER_0_INST, TIMEOUT);
        DL_Timer_stopCounter(TIMER_0_INST);
    }
}

/**
 * @brief   Receive data bytes and validate checksum
 *
 * Accumulates received data bytes and validates enhanced checksum
 * when all bytes are received. Calls the registered callback on
 * successful reception.
 *
 * @param[in] peripheral  Pointer to LIN peripheral instance
 * @param[in] rxByte      Received data byte
 * @param[in] lin         Pointer to LIN context structure
 */
void LIN_Responder_receiveMessage(LIN_Peripheral_t peripheral, uint8_t rxByte, Lin_TxRxCtx_t *lin)
{
    uint8_t checksum;
    uint8_t rxChecksum;
    LIN_function_ptr_t callbackFunction;

    /* Receiving data bytes */
    if (lin->rxIdx < lin->respMsgTbl[lin->msgTblIdx].msgSize)
    {
        lin->rxBuf[lin->rxIdx] = rxByte;
        lin->chkSum.word += lin->rxBuf[lin->rxIdx++];
        return;
    }

    /* Receiving checksum byte */
    rxChecksum = rxByte;
    DL_UART_Extend_disableInterrupt(peripheral, DL_UART_EXTEND_INTERRUPT_RX);

    /* Calculate enhanced checksum (LIN 2.x) */
    lin->chkSum.word += lin->respMsgTbl[lin->msgTblIdx].msgID;
    lin->chkSum.word = lin->chkSum.byte[0] + lin->chkSum.byte[1];
    checksum = lin->chkSum.byte[0];
    checksum += lin->chkSum.byte[1];
    checksum = 0xFF - checksum;

    if (rxChecksum == checksum)
    {
        callbackFunction = lin->respMsgTbl[lin->msgTblIdx].callbackFunction;
        callbackFunction();
    }

    /* Reset for next frame */
    lin->rxIdx = 0;
    lin->chkSum.word = 0;
    lin->state = LIN_STATE_WAIT_FOR_BREAK;

    LIN_prepareForNextFrame(peripheral);
}

/**
 * @brief   Transmit data bytes and checksum
 *
 * Transmits data bytes from the TX buffer followed by the enhanced
 * checksum. Calls the TX callback after transmission is complete.
 *
 * @param[in] peripheral  Pointer to LIN peripheral instance
 * @param[in] lin         Pointer to LIN context structure
 */
void LIN_Responder_transmitMessage(LIN_Peripheral_t peripheral, Lin_TxRxCtx_t *lin)
{
    uint8_t checksum;

    /* More data bytes to send */
    if (lin->txIdx < lin->respMsgTbl[lin->msgTblIdx].msgSize)
    {
        lin->chkSum.word += lin->txBuf[lin->txIdx];
        DL_UART_Extend_transmitData(peripheral, lin->txBuf[lin->txIdx++]);
        return;
    }

    /* All data bytes sent - transmit checksum */
    if (lin->txIdx == lin->respMsgTbl[lin->msgTblIdx].msgSize)
    {
        /* Calculate enhanced checksum */
        lin->chkSum.word = lin->chkSum.word + lin->respMsgTbl[lin->msgTblIdx].msgID + lin->txBuf[0];
        lin->chkSum.word = lin->chkSum.byte[0] + lin->chkSum.byte[1];
        checksum = lin->chkSum.byte[0];
        checksum += lin->chkSum.byte[1];
        checksum = 0xFF - checksum;

        DL_UART_Extend_transmitData(peripheral, checksum);
        lin->txIdx++;
        return;
    }

    /* Checksum sent - transmission complete */
    DL_UART_Extend_disableInterrupt(peripheral, DL_UART_EXTEND_INTERRUPT_TX);

    lin->txIdx = 1;
    lin->chkSum.word = 0;

    while (!DL_UART_isRXFIFOEmpty(peripheral))
    {
        DL_UART_Extend_receiveData(peripheral);
    }

    lin->state = LIN_STATE_WAIT_FOR_BREAK;

    LIN_prepareForNextFrame(peripheral);
    LIN_processMessage_Tx();
}

/**
 * @brief   LIN UART interrupt service routine
 *
 * Handles all LIN-related interrupts:
 *   - Break field detection (falling edge)
 *   - Break field validation (rising edge)
 *   - Sync field measurement (negative/positive edges)
 *   - Data reception and transmission
 */
void LIN_0_INST_IRQHandler(void)
{
    uint8_t rxByte;

    switch (DL_UART_Extend_getPendingInterrupt(LIN_0_INST))
    {
    case DL_UART_EXTEND_IIDX_LIN_FALLING_EDGE:
        if (gLIN.state != LIN_STATE_WAIT_FOR_BREAK)
        {
            break;
        }

        gLIN.state = LIN_STATE_BREAK_FIELD;

        DL_UART_enableLINRisingEdgeCapture(LIN_0_INST);
        while (!DL_UART_isRXFIFOEmpty(LIN_0_INST))
        {
            DL_UART_Extend_receiveData(LIN_0_INST);
        }

        DL_UART_Extend_clearInterruptStatus(
                LIN_0_INST, DL_UART_EXTEND_INTERRUPT_LIN_RISING_EDGE);
        DL_UART_Extend_enableInterrupt(
                LIN_0_INST, DL_UART_EXTEND_INTERRUPT_LIN_RISING_EDGE);
        break;

    case DL_UART_EXTEND_IIDX_LIN_RISING_EDGE:
        if (gLIN.state != LIN_STATE_BREAK_FIELD)
        {
            break;
        }
        gLINT.brkW += DL_UART_Extend_getLINRisingEdgeCaptureValue(LIN_0_INST);
        if ((gLINT.brkW < (gLINT.btWidth * LIN_TBIT_BREAK_FIELD_MAX)) || (AUTO_BAUD_ENABLED))
        {
            /* Valid break field - prepare for sync field measurement */
            gLINT.brkW = 0;
            gLINT.syncOkCnt = 0;
            gLINT.syncErrCnt = 0;
            gLINT.btSum = 0;
            gLINT.syncStart = true;

            DL_UART_Extend_disableInterrupt(LIN_0_INST, DL_UART_EXTEND_INTERRUPT_LIN_FALLING_EDGE);
            DL_UART_Extend_clearInterruptStatus(LIN_0_INST, DL_UART_EXTEND_INTERRUPT_RXD_NEG_EDGE);
            DL_UART_Extend_enableInterrupt(LIN_0_INST, DL_UART_EXTEND_INTERRUPT_RXD_NEG_EDGE);
            DL_UART_Extend_clearInterruptStatus(LIN_0_INST, DL_UART_EXTEND_INTERRUPT_RX);
            DL_UART_Extend_enableInterrupt(LIN_0_INST, DL_UART_EXTEND_INTERRUPT_RX);

            LIN_switchCounterToSyncMode(LIN_0_INST);
            gLIN.state = LIN_STATE_SYNC_FIELD_NEG_EDGE;
        }
        else
        {
            /* Invalid break field - reset */
            gLINT.brkW = 0;
            gLIN.state = LIN_STATE_WAIT_FOR_BREAK;
        }
        DL_UART_Extend_disableInterrupt(LIN_0_INST, DL_UART_EXTEND_INTERRUPT_LIN_RISING_EDGE);
        break;

    case DL_UART_EXTEND_IIDX_LIN_COUNTER_OVERFLOW:
        if (gLIN.state == LIN_STATE_BREAK_FIELD)
        {
            gLINT.brkW += 65536;
            if (gLINT.brkW > (65536 * 2))
            {
                gLINT.brkW = 0;
                gLIN.state = LIN_STATE_WAIT_FOR_BREAK;
                DL_UART_Extend_disableInterrupt(LIN_0_INST, DL_UART_EXTEND_INTERRUPT_LIN_RISING_EDGE);
            }
        }
        else
        {
            /* Counter overflow during sync/data - clean exit */
            LIN_Responder_resetState(LIN_0_INST, &gLIN);
        }
        break;

    case DL_UART_EXTEND_IIDX_RXD_NEG_EDGE:
        if (gLIN.state != LIN_STATE_SYNC_FIELD_NEG_EDGE)
        {
            break;
        }

        gLINT.syncBt[gLINT.syncOkCnt].posEdge = DL_UART_Extend_getLINRisingEdgeCaptureValue(LIN_0_INST);
        gLINT.syncBt[gLINT.syncOkCnt].negEdge = DL_UART_Extend_getLINFallingEdgeCaptureValue(LIN_0_INST);

        if ((gLINT.syncBt[gLINT.syncOkCnt].negEdge >> 1) > gLINT.btWMin
         && (gLINT.syncBt[gLINT.syncOkCnt].negEdge >> 1) < gLINT.btWMax)
        {
            gLINT.syncOkCnt++;
        }
        else if (!gLINT.syncStart)
        {
            gLINT.syncErrCnt++;
            gLINT.btSum += gLINT.syncBt[gLINT.syncOkCnt].negEdge;
        }
        else
        {
            gLINT.syncStart = false;
        }

        /* All 4 sync cycles measured */
        if ((gLINT.syncErrCnt + gLINT.syncOkCnt) == LIN_RESPONDER_SYNC_CYCLES)
        {
            DL_UART_Extend_disableInterrupt(LIN_0_INST, DL_UART_EXTEND_INTERRUPT_RXD_NEG_EDGE);
            DL_UART_Extend_clearInterruptStatus(LIN_0_INST, DL_UART_EXTEND_INTERRUPT_RXD_POS_EDGE);
            DL_UART_Extend_enableInterrupt(LIN_0_INST, DL_UART_EXTEND_INTERRUPT_RXD_POS_EDGE);

            if (gLINT.syncOkCnt == LIN_RESPONDER_SYNC_CYCLES)
            {
                gLINT.brPrev = gLINT.brCurr;
                gLINT.autoBaud = false;
            }

            gLIN.state = LIN_STATE_SYNC_FIELD_POS_EDGE;
        }
        /* Auto-baud recalibration triggered */
        else if ((gLINT.syncErrCnt == AUTO_BAUD_THRESHOLD) && AUTO_BAUD_ENABLED)
        {
            gLINT.btAvg = (gLINT.btSum >> 1) / gLINT.syncErrCnt;
            gLINT.brMeas = LIN_0_INST_FREQUENCY / gLINT.btAvg;

            DL_UART_Extend_disable(LIN_0_INST);
            delay_cycles(AUTO_BAUD_CONFIG_DELAY);
            DL_UART_Extend_disableFIFOs(LIN_0_INST);

            gLINT.respDelay = LIN_0_INST_FREQUENCY / (2 * gLINT.brMeas);
            gLINT.btWidth = gLINT.btAvg;
            gLINT.btWMin = (uint16_t)(gLINT.btWidth * LIN_AUTO_BAUD_MIN / 100);
            gLINT.btWMax = (uint16_t)(gLINT.btWidth * LIN_AUTO_BAUD_MAX / 100);
            gLINT.brkCmp = (uint16_t)(gLINT.btWidth * LIN_0_TBIT_COUNTER_COEFFICIENT);

            DL_UART_Extend_configBaudRate(LIN_0_INST, LIN_0_INST_FREQUENCY, gLINT.brMeas);
            DL_UART_Extend_enable(LIN_0_INST);

            gLINT.brPrev = gLINT.brCurr;
            gLINT.brCurr = gLINT.brMeas;
            gLINT.autoBaud = true;
        }
        break;

    case DL_UART_EXTEND_IIDX_RXD_POS_EDGE:
        if (gLIN.state != LIN_STATE_SYNC_FIELD_POS_EDGE)
        {
            break;
        }

#if defined(__MSPM0_HAS_UART_EXTD__)
        if ((gLINT.brCurr > gLINT.brPrev) && DL_UART_getErrorStatus(LIN_0_INST, DL_UART_ERROR_OVERRUN))
        {
            DL_UART_Extend_disableLINCounter(LIN_0_INST);
            DL_UART_Extend_enableLINCounter(LIN_0_INST);
        }
        else if (gLINT.brCurr < gLINT.brPrev)
        {
            DL_UART_Extend_receiveData(LIN_0_INST);
        }
#else
    if (gLINT.brCurr < gLINT.brPrev) {
      DL_UART_Extend_receiveData(LIN_0_INST);
    }
#endif

        LIN_switchCounterToBreakMode(LIN_0_INST);
        DL_UART_setLINCounterCompareValue(LIN_0_INST, gLINT.brkCmp);

        DL_UART_Extend_clearInterruptStatus(LIN_0_INST, DL_UART_EXTEND_INTERRUPT_LIN_FALLING_EDGE);
        DL_UART_Extend_enableInterrupt(LIN_0_INST, DL_UART_EXTEND_INTERRUPT_LIN_FALLING_EDGE);
        DL_UART_Extend_disableInterrupt(LIN_0_INST,
        DL_UART_EXTEND_INTERRUPT_RXD_POS_EDGE);

        DL_Timer_setLoadValue(TIMER_0_INST, TIMEOUT);
        DL_Timer_setTimerCount(TIMER_0_INST, TIMEOUT);
        DL_Timer_startCounter(TIMER_0_INST);
        break;

    case DL_UART_EXTEND_IIDX_RX:
        rxByte = DL_UART_Extend_receiveData(LIN_0_INST);
        DL_Timer_setTimerCount(TIMER_0_INST, TIMEOUT);

        if (gLIN.state == LIN_STATE_SYNC_FIELD_POS_EDGE)
        {
            gLIN.state = LIN_STATE_PID_FIELD;
        }
        else if (gLIN.state == LIN_STATE_SYNC_FIELD_NEG_EDGE)
        {
            if (!AUTO_BAUD_ENABLED)
            {
                LIN_switchCounterToBreakMode(LIN_0_INST);
                DL_UART_setLINCounterCompareValue(LIN_0_INST, gLINT.brkCmp);

                DL_UART_Extend_clearInterruptStatus(LIN_0_INST, DL_UART_EXTEND_INTERRUPT_LIN_FALLING_EDGE);
                DL_UART_Extend_enableInterrupt(LIN_0_INST, DL_UART_EXTEND_INTERRUPT_LIN_FALLING_EDGE);
                DL_UART_Extend_disableInterrupt(LIN_0_INST, DL_UART_EXTEND_INTERRUPT_RXD_NEG_EDGE);

                gLIN.state = LIN_STATE_PID_FIELD;
            }
        }
        else if (gLIN.state == LIN_STATE_PID_FIELD)
        {
            LIN_Responder_receivePID(LIN_0_INST, rxByte, &gLIN);
        }
        else if (gLIN.state == LIN_STATE_DATA_FIELD)
        {
            LIN_Responder_receiveMessage(LIN_0_INST, rxByte, &gLIN);
        }
        break;

    case DL_UART_EXTEND_IIDX_TX:
        LIN_Responder_transmitMessage(LIN_0_INST, &gLIN);
        break;

    default:
        break;
    }
}

/**
 * @brief   Timer timeout interrupt handler
 *
 * Handles frame timeout - resets state machine if no data received
 * within the expected time window.
 */
void TIMER_0_INST_IRQHandler(void)
{
    switch (DL_Timer_getPendingInterrupt(TIMER_0_INST))
    {
    case DL_TIMER_IIDX_ZERO:
        /* Timeout occurred - clean exit from any state */
        if (gLIN.state != LIN_STATE_WAIT_FOR_BREAK)
        {
            LIN_Responder_resetState(LIN_0_INST, &gLIN);
        }
        break;

    default:
        break;
    }
}
