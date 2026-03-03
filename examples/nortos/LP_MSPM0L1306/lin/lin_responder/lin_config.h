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
 * MSPM0 LIN (Local Interconnect Network) Header File
 *
 * This header defines the data structures, constants, and types used for
 * implementing a LIN 2.2A compliant responder on MSPM0 microcontrollers.
 * It includes support for automatic baud rate synchronization and
 * frame-level diagnostics.
 *
 * Supports both UART Extended and UNICOMM UART peripherals.
 */

#ifndef LIN_CONFIG_H_
#define LIN_CONFIG_H_

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>
#include "ti_msp_dl_config.h"

/* Number of messages in the responder message table */
#define LIN_RESPONDER_NUM_MSGS      (0x06)

/* Maximum data payload size per LIN frame (LIN spec: 1-8 bytes) */
#define LIN_DATA_MAX_BUFFER_SIZE    (8)

/* Maximum break field duration in bit times (for timeout detection) */
#define LIN_TBIT_BREAK_FIELD_MAX    (20)

/*
 * Number of PWM cycles used for sync field validation.
 * Per LIN 2.2A Section 6.4.2: The sync field consists of 5 falling edges
 * (start bit + 4 data bits of 0x55), providing 4 measurable bit periods.
 */
#define LIN_RESPONDER_SYNC_CYCLES   (4)

/* LIN sync byte value (alternating 1s and 0s for clock synchronization) */
#define LIN_SYNC_BYTE               (0x55)

/*
 * Inter-byte space: delay between PID STOP bit and response data START bit.
 * Calculated as half a bit time to ensure proper spacing.
 */
#define LIN_RESPONSE_LAPSE          (LIN_0_INST_FREQUENCY / (2 * LIN_0_BAUD_RATE))

/* Enable/disable automatic baud rate synchronization feature */
#define AUTO_BAUD_ENABLED           (true)

/* Auto-baud threshold*/
#define LIN_AUTO_BAUD_MAX           (105)
#define LIN_AUTO_BAUD_MIN           (95)

/*
 * Number of consecutive sync failures required before auto-baud
 * recalibration is triggered. Prevents recalibration on transient errors.
 */
#define AUTO_BAUD_THRESHOLD         (3)

/* Delay cycles for UART peripheral to stabilize after baud rate change */
#define AUTO_BAUD_CONFIG_DELAY      (10)

/* Receive timeout period (timer reload value for frame timeout detection) */
#define TIMEOUT                     (48U)

#if defined(__MCU_HAS_UNICOMMUART__)
/* UNICOMM peripheral type and register access */
typedef UNICOMM_Inst_Regs *LIN_Peripheral_t;
#define DL_UART_Extend_enableLINCounterClearOnFallingEdge                     \
                                     DL_UART_enableLINCounterClearOnFallingEdge

#else
/* UART Extended peripheral type and register access */
typedef UART_Regs *LIN_Peripheral_t;
#endif

/*!
 * @brief Function pointer type for LIN message callbacks
 *
 * Called when a specific PID is received/transmitted.
 */
typedef void (*LIN_function_ptr_t)(void);

/*!
 * @enum LIN_STATE
 * @brief LIN responder state machine states
 *
 * The LIN responder progresses through these states while receiving a frame:
 *   BREAK -> SYNC (neg/pos edges) -> PID -> DATA
 */
typedef enum
{
    /*!< Idle state: waiting for break field (dominant bus for >= 13 bit times) */
    LIN_STATE_WAIT_FOR_BREAK = 0,

    /*!< Break detected: measuring break field duration */
    LIN_STATE_BREAK_FIELD = 1,

    /*!< Sync field: waiting for falling edge to measure bit timing */
    LIN_STATE_SYNC_FIELD_NEG_EDGE = 2,

    /*!< Sync field: waiting for rising edge to complete bit time measurement */
    LIN_STATE_SYNC_FIELD_POS_EDGE = 3,

    /*!< PID field: receiving and validating the Protected Identifier byte */
    LIN_STATE_PID_FIELD = 4,

    /*!< Data field: receiving/transmitting data bytes and checksum */
    LIN_STATE_DATA_FIELD = 5
} LIN_STATE;

/*!
 * @enum LIN_ERROR
 * @brief LIN error codes for diagnostics
 */
typedef enum
{
    /*!< No error detected */
    LIN_ERROR_NO_ERROR = 0,

    /*!< Break field too short (< 11 bit times for responder) */
    LIN_ERROR_BREAK_SHORT = 1,

    /*!< Break field too long (exceeds maximum allowed duration) */
    LIN_ERROR_BREAK_LONG = 2,

    /*!< Sync byte mismatch (received value != 0x55) */
    LIN_ERROR_SYNC = 3
} LIN_ERROR;

/*!
 * @brief Union for byte/word access to 16-bit values
 *
 * Used for checksum calculation where byte-level access is needed.
 */
typedef union
{
    /*!< Access as 16-bit word */
    uint16_t word;
    /*!< Access as array of 2 bytes */
    uint8_t byte[2];
} LIN_word_t;

/*!
 * @brief LIN message table record
 *
 * Defines a single entry in the responder's message table.
 * Each record maps a PID to its data length and optional callback.
 */
typedef struct
{
    /*!< Protected Identifier (PID) */
    uint8_t msgID;
    /*!< Data field length (1-8 bytes) */
    uint8_t msgSize;
    /*!< Callback invoked on message Rx/Tx */
    LIN_function_ptr_t callbackFunction;
} LIN_table_record_t;

/*!
 * @brief Sync field bit timing measurement
 *
 * Stores timer capture values for one bit period during sync field.
 */
typedef struct
{
    /*!< Timer value at falling edge */
    uint16_t negEdge;
    /*!< Timer value at rising edge */
    uint16_t posEdge;
} LIN_sync_bits_t;

/*!
 * @brief LIN timing and auto-baud rate context
 *
 * Contains all timing-related measurements and auto-baud state.
 */
typedef struct
{
    /* Break field timing */
    /*!< Measured break field width (timer counts) */
    uint32_t brkW;
    /*!< Break field compare value for validation */
    uint16_t brkCmp;

    /* Sync field measurements */
    /*!< Flag: sync start bit detected */
    bool syncStart;
    /*!< Bit timing for each sync cycle */
    LIN_sync_bits_t syncBt[LIN_RESPONDER_SYNC_CYCLES];
    /*!< Count of valid sync cycles */
    uint8_t syncOkCnt;
    /*!< Count of failed sync cycles */
    uint8_t syncErrCnt;

    /* Bit time calculations */
    /*!< Sum of measured bit times */
    uint16_t btSum;
    /*!< Average bit time (btSum / num_cycles) */
    uint16_t btAvg;
    /*!< Current bit time width */
    uint16_t btWidth;
    /*!< Minimum acceptable bit width (lower threshold) */
    uint16_t btWMin;
    /*!< Maximum acceptable bit width (upper threshold) */
    uint16_t btWMax;

    /* Baud rate tracking */
    /*!< Measured baud rate from sync field */
    uint16_t brMeas;
    /*!< Previous baud rate (before recalibration) */
    uint16_t brPrev;
    /*!< Current configured baud rate */
    uint16_t brCurr;

    /* Auto-baud state */
    /*!< Flag: auto-baud calibration active */
    bool autoBaud;

    /* Response timing */
    /*!< Delay from PID to response (clock cycles) */
    uint16_t respDelay;
} Lin_timing_t;

/*!
 * @brief LIN transmit/receive context
 *
 * Main structure containing the LIN state machine, buffers, and configuration.
 */
typedef struct
{
    /* State machine */
    /*!< Current LIN state */
    volatile LIN_STATE state;

    /* Message table */
    /*!< Pointer to responder message table */
    LIN_table_record_t *respMsgTbl;
    /*!< Current message index in table */
    uint8_t msgTblIdx;

    /* Receive context */
    /*!< Current receive buffer index */
    volatile uint8_t rxIdx;
    /*!< Receive data buffer */
    uint8_t rxBuf[LIN_DATA_MAX_BUFFER_SIZE];

    /* Transmit context */
    /*!< Current transmit buffer index */
    volatile uint8_t txIdx;
    /*!< Transmit data buffer */
    uint8_t txBuf[LIN_DATA_MAX_BUFFER_SIZE];

    /* Checksum */
    /*!< Running checksum calculation */
    LIN_word_t chkSum;

    /* Timing reference */
    /*!< Pointer to timing context */
    volatile Lin_timing_t *timing;
} Lin_TxRxCtx_t;

/*!
 * @brief LIN diagnostic information
 *
 * Stores diagnostic data from the last received frame for debugging.
 */
typedef struct
{
    /*!< Received sync byte (should be 0x55) */
    uint8_t syncByte;
    /*!< Received Protected Identifier */
    uint8_t PID;
    /*!< Error code from last frame */
    LIN_ERROR error;
} LIN_diag_t;

/*
 * User must define the message table in application file (lin_responder.c)
 */
extern LIN_table_record_t responderMessageTable[LIN_RESPONDER_NUM_MSGS];

/*!< Global LIN transmit/receive context */
extern Lin_TxRxCtx_t gLIN;

/*!< Global LIN diagnostic data */
extern volatile LIN_diag_t gLINDiag;

/*!< Global LIN timing context */
extern volatile Lin_timing_t gLINT;

/*!< Transmit complete callback (must be defined in application) */
extern void LIN_processMessage_Tx(void);

/**
 * @brief   Process received Protected Identifier (PID)
 *
 * @param[in] peripheral  Pointer to LIN peripheral instance
 * @param[in] rxByte      Received PID byte
 * @param[in] lin         Pointer to LIN context structure
 */
void LIN_Responder_receivePID(LIN_Peripheral_t peripheral, uint8_t rxByte, Lin_TxRxCtx_t *lin);

/**
 * @brief   Receive data bytes and validate checksum
 *
 * @param[in] peripheral  Pointer to LIN peripheral instance
 * @param[in] rxByte      Received data byte
 * @param[in] lin         Pointer to LIN context structure
 */
void LIN_Responder_receiveMessage(LIN_Peripheral_t peripheral, uint8_t rxByte, Lin_TxRxCtx_t *lin);

/**
 * @brief   Transmit data bytes and checksum
 *
 * @param[in] peripheral  Pointer to LIN peripheral instance
 * @param[in] lin         Pointer to LIN context structure
 */
void LIN_Responder_transmitMessage(LIN_Peripheral_t peripheral, Lin_TxRxCtx_t *lin);

#endif /* LIN_CONFIG_H_ */
