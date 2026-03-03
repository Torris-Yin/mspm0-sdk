/*
 * Copyright (c) 2023, Texas Instruments Incorporated
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

    .syntax unified
    .thumb
    .section .text
    .align 2

    @ Constants for GPIO registers (absolute addresses)
    .equ GPIOA_BASE,          0x400A0000
    .equ GPIOA_GPRCM_PWREN,   0x400A0800  @ GPIOA_BASE + 0x800 (GPRCM offset) + 0x0 (PWREN offset)
    .equ GPIOA_GPRCM_RSTCTL,  0x400A0804  @ GPIOA_BASE + 0x800 (GPRCM offset) + 0x4 (RSTCTL offset)
    .equ GPIOA_DOUTSET31_0,   0x400A1290  @ GPIOA_BASE + 0x1290 (DOUTSET31_0 offset)
    .equ GPIOA_DOUTCLR31_0,   0x400A12A0  @ GPIOA_BASE + 0x12A0 (DOUTCLR31_0 offset)
    .equ GPIOA_DOESET31_0,    0x400A12D0  @ GPIOA_BASE + 0x12D0 (DOESET31_0 offset)

    @ Constants for IOMUX (Pin Configuration Module)
    .equ IOMUX_BASE,          0x40428000
    .equ IOMUX_PINCM1,        0x40428004  @ IOMUX_BASE + 0x004 (PINCM1 offset for pin 1)

    @ IOMUX Configuration Values
    .equ IOMUX_PINCM_PC_CONNECTED, 0x00000080  @ Enable peripheral connection
    .equ IOMUX_GPIO_FUNCTION,      0x00000001  @ Function value for GPIO

    @ Pin constants
    .equ DL_GPIO_PIN_0,       0x00000001  @ Bit mask for Pin 0

    @ Constants for register values
    .equ GPIO_PWREN_VALUE,    0x26000001  @ KEY_UNLOCK_W | ENABLE_ENABLE
    .equ GPIO_RSTCTL_VALUE,   0xB1000003  @ KEY_UNLOCK_W | RESETSTKYCLR_CLR | RESETASSERT_ASSERT
    .equ POWER_STARTUP_DELAY, 16          @ Delay in CPU cycles
    .equ LED_DELAY_CYCLES,    16000000    @ 500ms delay cycles (32MHz CPU clock / 2 = 16M cycles)

    .global main
    .type main, %function

main:
    @ Initialize GPIO power and configure GPIO PA0 (LED0)
    bl      GPIO_initPA0

loop:
    bl      GPIO_clearPA0            @ Turn on LED0
    ldr     r0, =LED_DELAY_CYCLES    @ 500ms delay
    bl      Delay
    bl      GPIO_setPA0              @ Turn off LED0
    ldr     r0, =LED_DELAY_CYCLES    @ 500ms delay
    bl      Delay
    b       loop                     @ Loop forever

.align 2
.type GPIO_initPA0, %function
GPIO_initPA0:
    push    {r4, r5, lr}            @ Save registers

    @ Reset GPIOA
    ldr     r0, =GPIOA_GPRCM_RSTCTL @ Load GPIOA RSTCTL register address
    ldr     r1, =GPIO_RSTCTL_VALUE  @ Load reset value
    str     r1, [r0]                @ Write to RSTCTL register

    @ Enable power for GPIOA
    ldr     r0, =GPIOA_GPRCM_PWREN  @ Load GPIOA PWREN register address
    ldr     r1, =GPIO_PWREN_VALUE   @ Load power enable value
    str     r1, [r0]                @ Write to PWREN register

    @ Delay for power startup
    ldr     r0, =POWER_STARTUP_DELAY @ Load delay value
    bl      Delay                   @ Call delay function

    @ Configure PA0 (GPIO pin 0) as output for controlling the LED:

    @ Configure pin as GPIO through the I/O multiplexer
    ldr     r0, =IOMUX_PINCM1      @ Load address of pin configuration register
    movs    r1, #IOMUX_GPIO_FUNCTION  @ Function value for GPIO mode (1)
    movs    r2, #IOMUX_PINCM_PC_CONNECTED @ Enable peripheral connection (0x80)
    orrs    r1, r1, r2            @ Combine configuration values
    str     r1, [r0]              @ Write configuration to pin register

    @ Set pin initial output value to high
    ldr     r0, =GPIOA_DOUTSET31_0  @ Load atomic set register address
    ldr     r1, =DL_GPIO_PIN_0      @ Bit mask for pin 0
    str     r1, [r0]                @ Set pin high

    @ Enable the output driver for the pin
    ldr     r0, =GPIOA_DOESET31_0   @ Load output enable set register address
    ldr     r1, =DL_GPIO_PIN_0      @ Bit mask for pin 0
    str     r1, [r0]                @ Enable output driver

    pop     {r4, r5, pc}            @ Restore registers and return

@ Function: GPIO_clearPA0 - Turn ON the LED by clearing GPIO pin
@ PA0 is connected to an active-low LED (clearing the pin turns it ON)
.align 2
.type GPIO_clearPA0, %function
GPIO_clearPA0:
    ldr     r1, =GPIOA_DOUTCLR31_0  @ Load atomic clear register address
    movs    r0, #0x01               @ Bit mask for PA0
    str     r0, [r1]                @ Clear pin 0 (turns ON LED)
    bx      lr                      @ Return

@ Function: GPIO_setPA0 - Turn OFF the LED by setting GPIO pin
@ PA0 is connected to an active-low LED (setting the pin turns it OFF)
.align 2
.type GPIO_setPA0, %function
GPIO_setPA0:
    ldr     r1, =GPIOA_DOUTSET31_0  @ Load atomic set register address
    movs    r0, #0x01               @ Bit mask for PA0
    str     r0, [r1]                @ Set pin 0 (turns OFF LED)
    bx      lr                      @ Return

@ Function: Delay - Create a time delay using CPU cycles
@ Implements a software delay loop for LED blinking timing
.align 2
.type Delay, %function
Delay:
    subs    r0, r0, #2              @ Subtract 2 from cycle count
dloop:
    subs    r0, r0, #4              @ Subtract 4 from cycle count
    nop                             @ No operation (takes 1 cycle)
    bhs     dloop                   @ Branch if still positive
    bx      lr                      @ Return
