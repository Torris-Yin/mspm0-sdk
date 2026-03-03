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

#include "model/tvmgen_default.h"
#include "ti_msp_dl_config.h"
#include "hello_world_testvector.h"

/**
 * @brief Select the test vector in the hello_world_testvector.h
 *        file to use for inference
 *
 * Change the macro to test different waveform types:
 *   - TESTVECTOR_SAWTOOTH_WAVE (Expected LED: Red)
 *   - TESTVECTOR_SINE_WAVE    (Expected LED: Blue)
 *   - TESTVECTOR_SQUARE_WAVE  (Expected LED: Green)
 */

/** \brief Model output buffer for storing inference results */
float of_map[1][MODEL_OUTPUT_CLASSES] = {0, 0, 0};

/**
 * @brief Copy the selected test vector to the input buffer
 */

int main(void)
{
    SYSCFG_DL_init();

    /* Power up the neural processing unit (NPU) module.
     * Clear and enable the NPU interrupt.
     */
    DL_NPU_reset(NPU);
    DL_NPU_enablePower(NPU);
    while (!(DL_SYSCTL_getStatus() & DL_SYSCTL_STATUS_NPU_READY));
    DL_NPU_clearInterruptStatus(NPU, DL_NPU_INTERRUPT_DONE);
    DL_NPU_enableInterrupt(NPU, DL_NPU_INTERRUPT_DONE);
    NVIC_EnableIRQ(NPU_INT_IRQn);

    struct tvmgen_default_inputs tvm_if_map = {(void*) &if_map[0]};
    struct tvmgen_default_outputs tvm_of_map = {(void*) &of_map[0]};

    /* Run Inference using NPU */
    tvmgen_default_run(&tvm_if_map, &tvm_of_map);

    /* Wait for NPU inference to complete */
    while (!tvmgen_default_finished);

    /* Power down the NPU after inference */
    DL_NPU_disablePower(NPU);

    /* Based on the model output, Turn on the corresponding LED */
    uint8_t maxIndex = SAWTOOTH_WAVE_INDEX;


    if(of_map[0][SINE_WAVE_INDEX] > of_map[0][maxIndex])
    {
        maxIndex = SINE_WAVE_INDEX;
    }

    if(of_map[0][SQUARE_WAVE_INDEX] > of_map[0][maxIndex])
    {
        maxIndex = SQUARE_WAVE_INDEX;
    }


    switch(maxIndex)
    {
        case SAWTOOTH_WAVE_INDEX:
                    DL_GPIO_setPins(GPIO_LED_RED_PORT,GPIO_LED_RED_PIN);
                    break;

        case SINE_WAVE_INDEX:
                    DL_GPIO_setPins(GPIO_LED_BLUE_PORT,GPIO_LED_BLUE_PIN);
                    break;

        case SQUARE_WAVE_INDEX:
                    DL_GPIO_setPins(GPIO_LED_GREEN_PORT,GPIO_LED_GREEN_PIN);
                    break;

        default:
                    break;
    }

    while (1) {
        __WFI();
    }
}
