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
#include "model_AUTOGEN.h"
#include "constants.h"
#include "filterbank_feature_extract.h"
#include "glass_break_detection_testvector.h"  
#include "main_model_glass_break/tvmgen_default.h"

/** \brief Index for glass break event */
#define GLASS_BREAK_EVENT_INDEX                                (0)
/** \brief Index for normal event */
#define NORMAL_EVENT_INDEX                                     (1)
/** \brief Index for event similar to glass break*/
#define EVENT_SIMILAR_TO_GLASS_BREAK                           (2)

float output[1][3] = {0, 0, 0};

/* Rolling NN input buffer (user-owned, size derived in constants.h) */
uint8_t final_features[FEATURES_BYTES];

/* FBFE context */
static FBFE_Ctx fe;

int main(void)
{
    SYSCFG_DL_init();
    DL_NPU_reset(NPU);
    DL_NPU_enablePower(NPU);
    while (!(DL_SYSCTL_getStatus() & DL_SYSCTL_STATUS_NPU_READY));

    DL_NPU_clearInterruptStatus(NPU, DL_NPU_INTERRUPT_DONE);
    DL_NPU_enableInterrupt(NPU, DL_NPU_INTERRUPT_DONE);
    NVIC_EnableIRQ(NPU_INT_IRQn);


    FBFE_Config cfg =
    {
        .sampling_rate_hz = SAMPLING_RATE_HZ,
        .window_size_ms   = WINDOW_SIZE_MS,
        .context_ms = CONTEXT_MS,

        .ls_mask  = LS_MASK,
        .ms_mask  = MS_MASK,
        .ms_shift = MS_SHIFT,
    };

    FBFE_Init(&fe, &cfg, final_features, sizeof(final_features));

    for (uint32_t i = 0; i < NUM_SAMPLES *(MODEL_CONTEXT_FRAMES-1); i++)
    {
        uint16_t sample = test_input_model_top_f32[i];

        FBFE_PushSample(&fe, sample);

        if ((i + 1u) % NUM_SAMPLES == 0u)
        {
            FBFE_RunOneFrame(&fe);
        }
    }

    static struct tvmgen_default_inputs  inputs  =
    {
        (void *)&final_features[0]
    };

    static struct tvmgen_default_outputs outputs =
    {
        (void *)&output[0]
    };

    tvmgen_default_finished = 0;
    tvmgen_default_run(&inputs, &outputs);
    while (!tvmgen_default_finished) { __WFE(); }
/* Based on the model output, Turn on the corresponding LED */
    uint8_t maxIndex = GLASS_BREAK_EVENT_INDEX;

    if(output[0][NORMAL_EVENT_INDEX] > output[0][maxIndex])
    {
        maxIndex = NORMAL_EVENT_INDEX;
    }

    if(output[0][EVENT_SIMILAR_TO_GLASS_BREAK] > output[0][maxIndex])
    {
        maxIndex = EVENT_SIMILAR_TO_GLASS_BREAK;
    }

    switch(maxIndex)
    {
        case GLASS_BREAK_EVENT_INDEX:
                    /* Glow Red LED */
                    DL_GPIO_setPins(GPIO_LED_RED_PORT, GPIO_LED_RED_PIN);
                    break;

        case NORMAL_EVENT_INDEX:
                    /* Glow Green LED */
                    DL_GPIO_setPins(GPIO_LED_GREEN_PORT, GPIO_LED_GREEN_PIN);
                    break;

        case EVENT_SIMILAR_TO_GLASS_BREAK:
                    /* Glow Blue LED */
                    DL_GPIO_setPins(GPIO_LED_BLUE_PORT, GPIO_LED_BLUE_PIN);
                    break;

        default:
                    break;
    }

    while (1) { __NOP(); }
}