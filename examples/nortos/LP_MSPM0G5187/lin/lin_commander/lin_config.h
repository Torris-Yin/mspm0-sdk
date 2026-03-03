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
 * LIN Commander Configuration Header
 *
 * This header defines the data structures, constants, and function prototypes
 * for implementing a LIN 2.2A commander on MSPM0 microcontrollers.
 * Supports both standard UART Extended and UNICOMM UART peripherals.
 */

#ifndef LIN_CONFIG_H_
#define LIN_CONFIG_H_

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>
#include "ti_msp_dl_config.h"

/*! MACRO to switch between TX INT and polling mode */
#define Transmit_INT

/*!< Maximum data payload size per LIN frame (LIN spec: 1-8 bytes) */
#define LIN_DATA_MAX_BUFFER_SIZE    (8)

/*!< Number of bit cycles used for sync field validation */
#define LIN_RESPONDER_SYNC_CYCLES   (5)

/*!< LIN sync byte value (alternating 1s and 0s: 0b01010101) */
#define LIN_SYNC_BYTE               (0x55)

/*!< Return value when PID is not found in message table */
#define LIN_MESSAGE_NOT_FOUND       (0xFF)

/*!< Number of messages in the commander message table */
#define LIN_COMMANDER_NUM_MSGS      (0x07)

/*
 * Break field duration in CPU cycles.
 * LIN 2.x requires minimum 13 Tbit for break field.
 * Calculated for 19200 baud at 32MHz CPU frequency:
 *   13 bits * (32MHz / 19200) = 21666 cycles
 */
#define LIN_BREAK_LENGTH            (21666)

/*
 * Frame timeout period in LFCLK ticks.
 * Used to detect incomplete frame reception.
 * 32 ticks @ 32kHz LFCLK = 1ms timeout
 */
#define TIMEOUT                     (32)

/*!
 * @enum LIN_RX_STATE
 * @brief Commander receive state machine states
 *
 * Tracks the current field being received in a LIN frame response.
 */
typedef enum
{
    /*!< Waiting for/receiving PID byte */
    LIN_RX_STATE_ID = 0,

    /*!< Receiving data bytes */
    LIN_RX_STATE_DATA,

    /*!< Receiving checksum byte */
    LIN_RX_STATE_CHECKSUM,
} LIN_RX_STATE;

/*!
 * @enum LIN_STATE
 * @brief LIN frame state machine states
 *
 * Used for tracking the overall LIN frame progress.
 */
typedef enum
{
    /*!< Idle state: waiting for break field */
    LIN_STATE_WAIT_FOR_BREAK = 0,

    /*!< Break field in progress */
    LIN_STATE_BREAK_FIELD = 1,

    /*!< Sync field: waiting for falling edge */
    LIN_STATE_SYNC_FIELD_NEG_EDGE = 2,

    /*!< Sync field: waiting for rising edge */
    LIN_STATE_SYNC_FIELD_POS_EDGE = 3,

    /*!< PID field: receiving Protected Identifier */
    LIN_STATE_PID_FIELD = 4,

    /*!< Data field: receiving/transmitting data and checksum */
    LIN_STATE_DATA_FIELD = 5
} LIN_STATE;

/*!
 * @brief Union for byte/word access to 16-bit values
 *
 * Used for checksum calculation where byte-level access is needed
 * to handle carry addition.
 */
typedef union
{
    /*!< Access as 16-bit word */
    uint16_t word;

    /*!< Access as array of 2 bytes */
    uint8_t byte[2];
} LIN_word_t;

/*!
 * @brief Function pointer type for LIN message callbacks
 *
 * Invoked when a specific PID's data is successfully received or transmitted.
 */
typedef void (*LIN_function_ptr_t)(void);

/*!
 * @brief LIN message table record
 *
 * Defines a single entry in the commander's message table.
 * Maps a PID to its data length and optional completion callback.
 */
typedef struct
{
    /*!< Protected Identifier (PID) with parity bits */
    uint8_t msgID;

    /*!< Data field length (1-8 bytes) */
    uint8_t msgSize;

    /*!< Callback invoked on message completion (NULL if not needed) */
    LIN_function_ptr_t callbackFunction;
} LIN_table_record_t;

/*!
 * @brief Sync field bit timing measurement
 *
 * Stores timer capture values for measuring bit periods during sync field.
 */
typedef struct
{
    /*!< Timer value captured at falling edge */
    uint16_t negEdge;

    /*!< Timer value captured at rising edge */
    uint16_t posEdge;
} LIN_Sync_Bits;

/*
 * UART Extended peripheral API (MSPM0 devices with UART_EXTD)
 */
#if defined(__MSPM0_HAS_UART_EXTD__)

/**
 * @brief   Initiate a LIN frame transmission
 *
 * Sends the break field, sync byte, and PID. If the message table entry
 * has no callback (transmit message), also sends the data and checksum.
 * If callback exists (receive message), waits for responder data.
 *
 * @param[in]  uart          Pointer to UART register overlay
 * @param[in]  tableIndex    Index into messageTable for the PID to send
 * @param[in]  TXmsgBuffer   Pointer to transmit data buffer
 * @param[in]  messageTable  Pointer to commander message table
 */
void LIN_Commander_sendPID(UART_Regs *uart, uint8_t tableIndex, uint8_t *TXmsgBuffer, LIN_table_record_t *messageTable);

/**
 * @brief   Process received data byte
 *
 * Accumulates received bytes into the message buffer and validates
 * the checksum when complete. Invokes the callback on successful reception.
 *
 * @param[in]  uart          Pointer to UART register overlay
 * @param[in]  rxByte        Received data byte
 * @param[out] msgBuffer     Pointer to receive buffer for storing data
 * @param[in]  messageTable  Pointer to commander message table
 */
void LIN_Commander_receiveMessage(UART_Regs *uart, uint8_t rxByte, uint8_t *msgBuffer, LIN_table_record_t *messageTable);

/**
 * @brief   Transmit next data byte (interrupt-driven)
 *
 * Called from TX interrupt to send subsequent data bytes and checksum.
 * Used when interrupt-driven transmission is enabled.
 *
 * @param[in]  uart          Pointer to UART register overlay
 * @param[in]  msgBuffer     Pointer to transmit data buffer
 * @param[in]  messageTable  Pointer to commander message table
 */
void LIN_Commander_transmitMessage(UART_Regs *uart, uint8_t *msgBuffer, LIN_table_record_t *messageTable);

#endif /* __MSPM0_HAS_UART_EXTD__ */

/*
 * UNICOMM UART peripheral API (devices with unified communication module)
 */
#if defined(__MCU_HAS_UNICOMMUART__)
/**
 * @brief   Initiate a LIN frame transmission (UNICOMM)
 *
 * @param[in]  unicomm       Pointer to UNICOMM register overlay
 * @param[in]  tableIndex    Index into messageTable for the PID to send
 * @param[in]  TXmsgBuffer   Pointer to transmit data buffer
 * @param[in]  messageTable  Pointer to commander message table
 */
void LIN_Commander_sendPID(UNICOMM_Inst_Regs *unicomm, uint8_t tableIndex, uint8_t *TXmsgBuffer, LIN_table_record_t *messageTable);

/**
 * @brief   Process received data byte (UNICOMM)
 *
 * @param[in]  unicomm       Pointer to UNICOMM register overlay
 * @param[in]  rxByte        Received data byte
 * @param[out] msgBuffer     Pointer to receive buffer for storing data
 * @param[in]  messageTable  Pointer to commander message table
 */
void LIN_Commander_receiveMessage(UNICOMM_Inst_Regs *unicomm, uint8_t rxByte, uint8_t *msgBuffer, LIN_table_record_t *messageTable);

/**
 * @brief   Transmit next data byte (UNICOMM, interrupt-driven)
 *
 * @param[in]  unicomm       Pointer to UNICOMM register overlay
 * @param[in]  msgBuffer     Pointer to transmit data buffer
 * @param[in]  messageTable  Pointer to commander message table
 */
void LIN_Commander_transmitMessage(UNICOMM_Inst_Regs *unicomm, uint8_t *msgBuffer, LIN_table_record_t *messageTable);

#endif /* __MCU_HAS_UNICOMMUART__ */

#endif /* LIN_CONFIG_H_ */
