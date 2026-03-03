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
#include "linear_actuator_fault_testvector.h"

/** \brief Index for Healthy operation class  */
#define LINEAR_ACTUATOR_NORMAL_INDEX                            (0)
/** \brief Index for Surface defects on screw/balls class */
#define LINEAR_ACTUATOR_SPALLING_INDEX                          (1)
/** \brief Index for Removed lubricant, tightened seals class */
#define LINEAR_ACTUATOR_LACK_LUBRICATION_INDEX                  (2)
/** \brief Index for Undersized balls causing excess play class */
#define LINEAR_ACTUATOR_BACKLASH_INDEX                          (3)


/** \brief Model output buffer for storing inference results */
float of_map[1][4]= {0, 0, 0, 0};

int main(void)
{
    SYSCFG_DL_init();

    struct tvmgen_default_inputs tvm_if_map = {(void*) &if_map[0]};
    struct tvmgen_default_outputs tvm_of_map = {(void*) &of_map[0]};

    /* Run Inference */
    tvmgen_default_run(&tvm_if_map, &tvm_of_map);

    /* Based on the model output, Turn on the corresponding LED */
    uint8_t maxIndex = LINEAR_ACTUATOR_NORMAL_INDEX;

    if(of_map[0][LINEAR_ACTUATOR_SPALLING_INDEX] > of_map[0][maxIndex])
    {
        maxIndex = LINEAR_ACTUATOR_SPALLING_INDEX;
    }

    if(of_map[0][LINEAR_ACTUATOR_LACK_LUBRICATION_INDEX] > of_map[0][maxIndex])
    {
        maxIndex = LINEAR_ACTUATOR_LACK_LUBRICATION_INDEX;
    }

    if(of_map[0][LINEAR_ACTUATOR_BACKLASH_INDEX] > of_map[0][maxIndex])
    {
        maxIndex = LINEAR_ACTUATOR_BACKLASH_INDEX;
    }

    switch(maxIndex)
    {
        case LINEAR_ACTUATOR_NORMAL_INDEX:
                    /* Glow Red LED */
                    DL_GPIO_setPins(GPIO_LED_RED_PORT, GPIO_LED_RED_PIN);
                    break;

        case LINEAR_ACTUATOR_SPALLING_INDEX:
                    /* Glow Green LED */
                    DL_GPIO_setPins(GPIO_LED_GREEN_PORT, GPIO_LED_GREEN_PIN);
                    break;
                    
        case LINEAR_ACTUATOR_LACK_LUBRICATION_INDEX:
                    /* Glow Blue LED */
                    DL_GPIO_setPins(GPIO_LED_BLUE_PORT, GPIO_LED_BLUE_PIN);
                    break;

        case LINEAR_ACTUATOR_BACKLASH_INDEX:
                    /* Glow Yellow LED */
                    DL_GPIO_setPins(GPIO_LED_RED_PORT, GPIO_LED_RED_PIN);
                    DL_GPIO_setPins(GPIO_LED_GREEN_PORT, GPIO_LED_GREEN_PIN);
                    break;

        default:
                    break;
    }

    while (1) {
        __WFI();
    }

}
