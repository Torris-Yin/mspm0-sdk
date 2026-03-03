/*
 * Copyright (c) 2021, Texas Instruments Incorporated
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
 * LIN Responder Application
 *
 * This application implements a LIN (Local Interconnect Network) responder node
 * on the MSPM0 microcontroller. The responder listens for messages from a LIN
 * commander and responds according to the configured message table.
 */

#include <lin_config.h>
#include <stdio.h>
#include <string.h>
#include "ti_msp_dl_config.h"

/* Forward declaration of the receive message handler */
void LIN_processMessage_Rx(void);

/*
 * LIN Responder Message Table
 *
 * Defines the PIDs (Protected Identifiers) this responder handles.
 * Each entry contains:
 *   - PID: The protected identifier (includes parity bits)
 *   - Data length: Number of bytes in the message (1-8)
 *   - Callback: Optional function pointer called when message is received
 */
LIN_table_record_t responderMessageTable[LIN_RESPONDER_NUM_MSGS] = {
        { 0x08, 5, },
        { 0x49, 5, },
        { 0x0D, 5, },
        { 0x39, 8, LIN_processMessage_Rx },
        { 0xBA, 8, LIN_processMessage_Rx },
        { 0xFB, 8, LIN_processMessage_Rx }
};

/*
 * Main function
 *
 * Initializes the system and enters low-power mode, waking only on interrupts.
 */
int main(void)
{
    /* Initialize all peripherals configured in SysConfig */
    SYSCFG_DL_init();

    /* Enable the LIN transceiver by setting the enable pin high */
    DL_GPIO_setPins(GPIO_LIN_ENABLE_PORT, GPIO_LIN_ENABLE_USER_LIN_ENABLE_PIN);

    /* Clear any pending interrupts and enable LIN UART interrupt */
    NVIC_ClearPendingIRQ(LIN_0_INST_INT_IRQN);
    NVIC_EnableIRQ(LIN_0_INST_INT_IRQN);

    /* Clear any pending interrupts and enable timer interrupt (for LIN timing) */
    NVIC_ClearPendingIRQ(TIMER_0_INST_INT_IRQN);
    NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);

    /* Enable automatic sleep when exiting ISR for low power operation */
    DL_SYSCTL_enableSleepOnExit();

    /* Main loop: sleep until interrupt occurs */
    while (1)
    {
        /* Wait For Interrupt - enters low power mode */
        __WFI();
    }
}

/*
 * LIN Receive Message Handler
 *
 * Called when a LIN message is received.
 * Copies received data to the transmit buffer (echo behavior) and
 * toggles LED1 to provide visual indication of reception.
 */
void LIN_processMessage_Rx(void)
{
    /* Copy received data to transmit buffer for echo/loopback */
    memcpy(&gLIN.txBuf[0], &gLIN.rxBuf[0], LIN_DATA_MAX_BUFFER_SIZE);

    /* Toggle LED1 to indicate a packet has been received */
    DL_GPIO_togglePins(GPIO_LEDS_PORT, GPIO_LEDS_USER_LED_1_PIN);
}

/*
 * LIN Transmit Message Handler
 *
 * Called after a LIN message has been transmitted.
 * Toggles LED2 to provide visual indication of transmission.
 */
void LIN_processMessage_Tx(void)
{
    /* Toggle LED2 to indicate a packet has been transmitted */
    DL_GPIO_togglePins(GPIO_LEDS_PORT, GPIO_LEDS_USER_LED_2_PIN);
}
