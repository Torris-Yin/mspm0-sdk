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
 * LIN Commander Implementation
 *
 * This file implements the LIN 2.2A commander functionality including:
 *   - Frame header transmission (break, sync, PID)
 *   - Data transmission with enhanced checksum
 *   - Response reception and checksum validation
 *   - Frame timeout handling
 *
 * Supports both blocking and interrupt-driven transmission modes.
 * Compatible with UART Extended and UNICOMM UART peripherals.
 */

#include "lin_config.h"
#include <stdio.h>

/*!< Current message table index for active frame */
uint8_t gMessageTableIndex = 0;

/*!< Receive state machine current state */
LIN_RX_STATE LIN_state = LIN_RX_STATE_DATA;

/*!< Byte counter for data field reception */
uint8_t byteCounter = 0;

/*!< Running checksum accumulator for reception */
LIN_word_t tempChksum;

#if defined(__MSPM0_HAS_UART_EXTD__)

/**
 * @brief   Initiate a LIN frame transmission
 *
 * Sends the complete frame header (break field, sync byte, PID) and then
 * either transmits data or prepares to receive a response based on whether
 * a callback function is registered for the PID.
 *
 * Frame structure:
 *   [BREAK] [SYNC=0x55] [PID] [DATA0..DATAn] [CHECKSUM]
 *
 * @param[in]  uart          Pointer to UART register overlay
 * @param[in]  tableIndex    Index into messageTable for the PID to send
 * @param[in]  TXmsgBuffer   Pointer to transmit data buffer
 * @param[in]  messageTable  Pointer to commander message table
 */
void LIN_Commander_sendPID(UART_Regs *uart, uint8_t tableIndex, uint8_t *TXmsgBuffer, LIN_table_record_t *messageTable)
{
    /* Disable RX interrupt during header transmission */
    DL_UART_Extend_disableInterrupt(uart, DL_UART_EXTEND_INTERRUPT_RX);

    /*
     * Transmit LIN frame header:
     *   1. Break field (dominant for >= 13 Tbit)
     *   2. Sync byte (0x55)
     *   3. Protected Identifier (PID)
     */
    DL_UART_enableLINSendBreak(uart);
    delay_cycles(LIN_BREAK_LENGTH);
    DL_UART_disableLINSendBreak(uart);

    DL_UART_Extend_transmitDataBlocking(uart, LIN_SYNC_BYTE);
    DL_UART_Extend_transmitDataBlocking(uart, messageTable[tableIndex].msgID);

    /* Wait for PID transmission to complete */
    while(DL_UART_isBusy(uart));

    /* Store table index for subsequent data handling */
    gMessageTableIndex = tableIndex;

    /*
     * Determine frame direction based on callback presence:
     *   - Callback exists: Receive response from responder
     *   - No callback: Transmit data to responder
     */
    if (messageTable[tableIndex].callbackFunction != NULL)
    {
        /* Receive mode: Prepare to receive response data */

        /* Flush RX buffer and clear pending interrupts */
        DL_UART_Extend_receiveData(uart);
        DL_UART_Extend_clearInterruptStatus(uart, DL_UART_EXTEND_INTERRUPT_RX);
        DL_UART_Extend_enableInterrupt(uart, DL_UART_EXTEND_INTERRUPT_RX);

        /* Start frame timeout timer */
        DL_Timer_setLoadValue(TIMER_0_INST, TIMEOUT);
        DL_Timer_startCounter(TIMER_0_INST);
    }
    else
    {
        /* Transmit mode: Send data to responder */

#ifdef Transmit_INT
          /* Interrupt-driven: Send first byte, ISR handles rest */
          DL_UART_Extend_transmitData(uart, TXmsgBuffer[0]);
          DL_UART_Extend_clearInterruptStatus(uart, DL_UART_EXTEND_INTERRUPT_TX);
          DL_UART_Extend_enableInterrupt(uart, DL_UART_EXTEND_INTERRUPT_TX);
  #else
        /* Blocking mode: Send entire packet */
        LIN_Commander_transmitMessage(uart, TXmsgBuffer, messageTable);
#endif
    }
}

/**
 * @brief   Process received response data byte
 *
 * State machine that accumulates received bytes into the message buffer,
 * maintains a running checksum, and validates the frame when complete.
 * Invokes the registered callback on successful checksum validation.
 *
 * Uses enhanced checksum (LIN 2.x): includes PID in checksum calculation.
 *
 * @param[in]  uart          Pointer to UART register overlay
 * @param[in]  rxByte        Received data byte
 * @param[out] msgBuffer     Pointer to receive buffer for storing data
 * @param[in]  messageTable  Pointer to commander message table
 */
void LIN_Commander_receiveMessage(UART_Regs *uart, uint8_t rxByte, uint8_t *msgBuffer, LIN_table_record_t *messageTable)
{
    uint8_t checksum;
    uint8_t rxChecksum;
    LIN_function_ptr_t callbackFunction;

    switch (LIN_state)
    {
    /*
     * DATA state: Accumulate data bytes
     */
    case LIN_RX_STATE_DATA:
        /* Store byte in receive buffer */
        msgBuffer[byteCounter] = rxByte;

        /* Accumulate checksum (16-bit to handle carry) */
        tempChksum.word += msgBuffer[byteCounter];

        /* Check if all data bytes received */
        byteCounter++;
        if (byteCounter >= messageTable[gMessageTableIndex].msgSize)
        {
            /* Transition to checksum state */
            LIN_state = LIN_RX_STATE_CHECKSUM;
        }

        /* Restart timeout timer for next byte */
        DL_Timer_startCounter(TIMER_0_INST);
        break;

        /*
         * CHECKSUM state: Validate frame checksum
         */
    case LIN_RX_STATE_CHECKSUM:
        rxChecksum = rxByte;

        /* Disable RX until next frame */
        DL_UART_Extend_disableInterrupt(uart, DL_UART_EXTEND_INTERRUPT_RX);

        /*
         * Enhanced checksum calculation (LIN 2.x):
         *   1. Add PID to running sum
         *   2. Fold carry into lower byte
         *   3. Invert result
         */
        tempChksum.word += messageTable[gMessageTableIndex].msgID;
        tempChksum.word = tempChksum.byte[0] + tempChksum.byte[1];
        checksum = tempChksum.byte[0];
        checksum += tempChksum.byte[1];
        checksum = 0xFF - checksum;

        /* Reset state for next frame */
        byteCounter = 0;
        tempChksum.word = 0;
        LIN_state = LIN_RX_STATE_DATA;

        /* Validate checksum and invoke callback */
        if (rxChecksum == checksum)
        {
            callbackFunction = messageTable[gMessageTableIndex].callbackFunction;
            callbackFunction();
        }
        break;

    default:
        /* Reset to data state on unexpected condition */
        LIN_state = LIN_RX_STATE_DATA;
        break;
    }
}

/**
 * @brief   Transmit data bytes and checksum
 *
 * Two modes of operation:
 *   - Interrupt-driven (Transmit_INT defined): Called from TX ISR,
 *     sends one byte per call, calculates checksum after last data byte
 *   - Blocking mode: Sends all data bytes and checksum in one call
 *
 * Uses enhanced checksum (LIN 2.x): includes PID in checksum calculation.
 *
 * @param[in]  uart          Pointer to UART register overlay
 * @param[in]  msgBuffer     Pointer to transmit data buffer
 * @param[in]  messageTable  Pointer to commander message table
 */
void LIN_Commander_transmitMessage(UART_Regs *uart, uint8_t *msgBuffer, LIN_table_record_t *messageTable)
{
    uint8_t checksum;

#ifdef Transmit_INT
      /*
       * Interrupt-driven transmission mode
       * Static variables maintain state between ISR calls
       */
      /*!< Current byte index (starts at 1, byte 0 sent in sendPID) */
      static uint8_t locIndex = 1;
      /*!< Running checksum accumulator */
      static LIN_word_t tempChksum_TX;

      /* Accumulate checksum and transmit current byte */
      tempChksum_TX.word += msgBuffer[locIndex];
      DL_UART_Extend_transmitData(uart, msgBuffer[locIndex++]);

      /* Check if all data bytes sent */
      if (locIndex >= messageTable[gMessageTableIndex].msgSize) {
          /*
           * Calculate enhanced checksum:
           * Include PID and first byte (sent before interrupt mode started)
           */
          tempChksum_TX.word = tempChksum_TX.word + messageTable[gMessageTableIndex].msgID + msgBuffer[0];
          tempChksum_TX.word = tempChksum_TX.byte[0] + tempChksum_TX.byte[1];
          checksum = tempChksum_TX.byte[0];
          checksum += tempChksum_TX.byte[1];
          checksum = 0xFF - checksum;

          /* Transmit checksum (blocking to ensure completion) */
          DL_UART_Extend_transmitDataBlocking(uart, checksum);

          /* Disable TX interrupt */
          DL_UART_Extend_disableInterrupt(uart, DL_UART_EXTEND_INTERRUPT_TX);

          /* Reset state for next transmission */
          locIndex = 1;
          tempChksum_TX.word = 0;

          /* Clear echo data from RX buffer */
          DL_UART_Extend_receiveData(uart);

          /* Wait for transmission to complete */
          while (DL_UART_Extend_isBusy(uart));
      }

  #else
    /*
     * Blocking transmission mode
     * Sends all data bytes and checksum in single function call
     */
    uint8_t locIndex;
    LIN_word_t tempChksum_TX;

    tempChksum_TX.word = 0;

    /* Transmit all data bytes */
    for (locIndex = 0; locIndex < messageTable[gMessageTableIndex].msgSize; locIndex++)
    {
        DL_UART_Extend_transmitDataBlocking(uart, msgBuffer[locIndex]);
        tempChksum_TX.word += msgBuffer[locIndex];
    }

    /* Calculate enhanced checksum (including PID) */
    tempChksum_TX.word += messageTable[gMessageTableIndex].msgID;
    tempChksum_TX.word = tempChksum_TX.byte[0] + tempChksum_TX.byte[1];
    checksum = tempChksum_TX.byte[0];
    checksum += tempChksum_TX.byte[1];
    checksum = 0xFF - checksum;

    /* Transmit checksum */
    DL_UART_Extend_transmitDataBlocking(uart, checksum);

    /* Clear echo data from RX buffer */
    DL_UART_Extend_receiveData(uart);

    /* Wait for transmission to complete */
    while (DL_UART_Extend_isBusy(uart))
        ;
#endif
}

#endif /* __MSPM0_HAS_UART_EXTD__ */

#if defined(__MCU_HAS_UNICOMMUART__)
/**
 * @brief   Initiate a LIN frame transmission (UNICOMM variant)
 *
 * Sends the complete frame header and either transmits data or prepares
 * to receive a response. Includes FIFO flush for UNICOMM peripheral.
 *
 * @param[in]  unicomm       Pointer to UNICOMM register overlay
 * @param[in]  tableIndex    Index into messageTable for the PID to send
 * @param[in]  TXmsgBuffer   Pointer to transmit data buffer
 * @param[in]  messageTable  Pointer to commander message table
 */
void LIN_Commander_sendPID(UNICOMM_Inst_Regs *unicomm, uint8_t tableIndex, uint8_t *TXmsgBuffer, LIN_table_record_t *messageTable)
{
    /* Disable RX interrupt during header transmission */
    DL_UART_Extend_disableInterrupt(unicomm, DL_UART_EXTEND_INTERRUPT_RX);

    /*
     * Transmit LIN frame header:
     *   1. Break field (dominant for >= 13 Tbit)
     *   2. Sync byte (0x55)
     *   3. Protected Identifier (PID)
     */
    DL_UART_enableLINSendBreak(unicomm);
    delay_cycles(LIN_BREAK_LENGTH);
    DL_UART_disableLINSendBreak(unicomm);

    DL_UART_Extend_transmitDataBlocking(unicomm, LIN_SYNC_BYTE);
    DL_UART_Extend_transmitDataBlocking(unicomm, messageTable[tableIndex].msgID);

    /* Wait for PID transmission to complete */
    while (DL_UART_isBusy(unicomm));

    /* Flush RX FIFO to clear echo data */
    while (!DL_UART_isRXFIFOEmpty(unicomm))
    {
        DL_UART_Extend_receiveData(unicomm);
    }

    /* Store table index for subsequent data handling */
    gMessageTableIndex = tableIndex;

    /*
     * Determine frame direction based on callback presence
     */
    if (messageTable[tableIndex].callbackFunction != NULL)
    {
        /* Receive mode: Prepare to receive response data */

        /* Clear pending RX interrupts */
        DL_UART_Extend_receiveData(unicomm);
        DL_UART_Extend_clearInterruptStatus(unicomm, DL_UART_EXTEND_INTERRUPT_RX);
        DL_UART_Extend_enableInterrupt(unicomm, DL_UART_EXTEND_INTERRUPT_RX);

        /* Start frame timeout timer */
        DL_Timer_setLoadValue(TIMER_0_INST, TIMEOUT);
        DL_Timer_startCounter(TIMER_0_INST);
    }
    else
    {
        /* Transmit mode: Send data to responder */

#ifdef Transmit_INT
          /* Interrupt-driven: Send first byte, ISR handles rest */
          DL_UART_Extend_transmitData(unicomm, TXmsgBuffer[0]);
          DL_UART_Extend_clearInterruptStatus(unicomm, DL_UART_EXTEND_INTERRUPT_TX);
          DL_UART_Extend_enableInterrupt(unicomm, DL_UART_EXTEND_INTERRUPT_TX);
  #else
        /* Blocking mode: Send entire packet */
        LIN_Commander_transmitMessage(unicomm, TXmsgBuffer, messageTable);
#endif
    }
}

/**
 * @brief   Process received response data byte (UNICOMM variant)
 *
 * @param[in]  unicomm       Pointer to UNICOMM register overlay
 * @param[in]  rxByte        Received data byte
 * @param[out] msgBuffer     Pointer to receive buffer for storing data
 * @param[in]  messageTable  Pointer to commander message table
 */
void LIN_Commander_receiveMessage(UNICOMM_Inst_Regs *unicomm, uint8_t rxByte, uint8_t *msgBuffer, LIN_table_record_t *messageTable)
{
    uint8_t checksum;
    uint8_t rxChecksum;
    LIN_function_ptr_t callbackFunction;

    switch (LIN_state)
    {
    /*
     * DATA state: Accumulate data bytes
     */
    case LIN_RX_STATE_DATA:
        /* Store byte in receive buffer */
        msgBuffer[byteCounter] = rxByte;

        /* Accumulate checksum */
        tempChksum.word += msgBuffer[byteCounter];

        /* Check if all data bytes received */
        byteCounter++;
        if (byteCounter >= messageTable[gMessageTableIndex].msgSize)
        {
            LIN_state = LIN_RX_STATE_CHECKSUM;
        }

        /* Restart timeout timer */
        DL_Timer_startCounter(TIMER_0_INST);
        break;

        /*
         * CHECKSUM state: Validate frame checksum
         */
    case LIN_RX_STATE_CHECKSUM:
        rxChecksum = rxByte;

        /* Disable RX until next frame */
        DL_UART_Extend_disableInterrupt(unicomm, DL_UART_EXTEND_INTERRUPT_RX);

        /* Calculate enhanced checksum */
        tempChksum.word += messageTable[gMessageTableIndex].msgID;
        tempChksum.word = tempChksum.byte[0] + tempChksum.byte[1];
        checksum = tempChksum.byte[0];
        checksum += tempChksum.byte[1];
        checksum = 0xFF - checksum;

        /* Reset state for next frame */
        byteCounter = 0;
        tempChksum.word = 0;
        LIN_state = LIN_RX_STATE_DATA;

        /* Validate checksum and invoke callback */
        if (rxChecksum == checksum)
        {
            callbackFunction = messageTable[gMessageTableIndex].callbackFunction;
            callbackFunction();
        }
        break;

    default:
        LIN_state = LIN_RX_STATE_DATA;
        break;
    }
}

/**
 * @brief   Transmit data bytes and checksum (UNICOMM variant)
 *
 * @param[in]  unicomm       Pointer to UNICOMM register overlay
 * @param[in]  msgBuffer     Pointer to transmit data buffer
 * @param[in]  messageTable  Pointer to commander message table
 */
void LIN_Commander_transmitMessage(UNICOMM_Inst_Regs *unicomm, uint8_t *msgBuffer, LIN_table_record_t *messageTable)
{
    uint8_t checksum;

#ifdef Transmit_INT
      /*
       * Interrupt-driven transmission mode
       */
      /*!< Current byte index */
      static uint8_t locIndex = 1;
      /*!< Running checksum accumulator */
      static LIN_word_t tempChksum_TX;

      /* Accumulate checksum and transmit current byte */
      tempChksum_TX.word += msgBuffer[locIndex];
      DL_UART_Extend_transmitData(unicomm, msgBuffer[locIndex++]);

      /* Check if all data bytes sent */
      if (locIndex >= messageTable[gMessageTableIndex].msgSize) {
          /* Calculate enhanced checksum */
          tempChksum_TX.word = tempChksum_TX.word + messageTable[gMessageTableIndex].msgID + msgBuffer[0];
          tempChksum_TX.word = tempChksum_TX.byte[0] + tempChksum_TX.byte[1];
          checksum = tempChksum_TX.byte[0];
          checksum += tempChksum_TX.byte[1];
          checksum = 0xFF - checksum;

          /* Transmit checksum */
          DL_UART_Extend_transmitDataBlocking(unicomm, checksum);

          /* Disable TX interrupt */
          DL_UART_Extend_disableInterrupt(unicomm, DL_UART_EXTEND_INTERRUPT_TX);

          /* Flush RX FIFO to clear echo data */
          while (!DL_UART_isRXFIFOEmpty(unicomm)) {
              DL_UART_Extend_receiveData(unicomm);
          }

          /* Reset state for next transmission */
          locIndex = 1;
          tempChksum_TX.word = 0;

          /* Wait for transmission to complete */
          while (DL_UART_Extend_isBusy(unicomm));
      }

#else
    /*
     * Blocking transmission mode
     */
    uint8_t locIndex;
    LIN_word_t tempChksum_TX;

    tempChksum_TX.word = 0;

    /* Transmit all data bytes */
    for (locIndex = 0; locIndex < messageTable[gMessageTableIndex].msgSize; locIndex++)
    {
        DL_UART_Extend_transmitDataBlocking(unicomm, msgBuffer[locIndex]);
        tempChksum_TX.word += msgBuffer[locIndex];
    }

    /* Calculate enhanced checksum */
    tempChksum_TX.word += messageTable[gMessageTableIndex].msgID;
    tempChksum_TX.word = tempChksum_TX.byte[0] + tempChksum_TX.byte[1];
    checksum = tempChksum_TX.byte[0];
    checksum += tempChksum_TX.byte[1];
    checksum = 0xFF - checksum;

    /* Transmit checksum */
    DL_UART_Extend_transmitDataBlocking(unicomm, checksum);

    /* Flush RX FIFO to clear echo data */
    while (!DL_UART_isRXFIFOEmpty(unicomm))
    {
        DL_UART_Extend_receiveData(unicomm);
    }

    /* Wait for transmission to complete */
    while (DL_UART_Extend_isBusy(unicomm));
#endif
}

#endif /* __MCU_HAS_UNICOMMUART__ */

/**
 * @brief   Frame timeout interrupt handler
 *
 * Handles timeout conditions when a complete response is not received
 * within the expected time window. Resets the receive state machine
 * to prepare for the next frame.
 */
void TIMER_0_INST_IRQHandler(void)
{
    switch (DL_Timer_getPendingInterrupt(TIMER_0_INST))
    {
    case DL_TIMER_IIDX_ZERO:
        /* Timeout occurred - reset receive state machine */
        LIN_state = LIN_RX_STATE_DATA;
        byteCounter = 0;
        tempChksum.word = 0;
        break;
    default:
        break;
    }
}
