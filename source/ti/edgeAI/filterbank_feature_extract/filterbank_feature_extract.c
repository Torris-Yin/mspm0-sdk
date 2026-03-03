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

/**
 * @file filterbank_feature_extract.c
 * @brief Implementation of filterbank-based feature extraction for  processing.
 *
 * This file contains the implementation of  feature extraction using
 * filterbank techniques with NPU hardware acceleration. It processes 
 * samples through a series of filterbank operations to extract meaningful
 * features for machine learning applications.
 * The implementation follows a multi-stage processing pipeline:
* 1. Sample packing into frames with configurable overlap
* 2. NPU-accelerated filterbank processing
* 3. Two-pass processing (MSB and LSB) for extended numerical precision
* 4. Max-pooling for feature dimensionality reduction
* 5. Feature normalization, scaling, and formatting
* 6. Window management for temporal context preservation
*
* This implementation is optimized for low-power operation on TI MSP devices
* with integrated Neural Processing Unit (NPU) hardware acceleration.
*/

#include "filterbank_feature_extract.h"
#include "constants.h"
#include "model_AUTOGEN.h"

#include <string.h>
#include <limits.h>

#include "fe_model/fe_model.h"
/* HW includes  */

#include <ti/devices/msp/msp.h>
#include "ti_msp_dl_config.h"
#include <ti/devices/msp/peripherals/hw_npu.h>

/**
 * @brief Shift bits for NPU CTL1 address field.
 */
#define FBFE_CTL1_ADDR_SHIFT_BITS        (18u)

/**
 * @brief Mask for NPU CTL1 address field.
 */
#define FBFE_CTL1_ADDR_MASK              (0xcfffu)

/**
 * @brief Shift bits for NPU CTL1 OFMAP field.
 */
#define FBFE_CTL1_OFMAP_SHIFT_BITS       (16u)

/**
 * @brief Step size for MMR stride operations.
 */
#define FBFE_MMR_STRIDE_STEP             (2u)

/**
 * @brief Shift bits for MSB combination in the 2-bit weights flow.
 */
#define FBFE_MSB_COMBINE_SHIFT_BITS      (8u)

/**
 * @brief Row word count for ARBIAS0.
 */
#define FBFE_ARBIAS0_ROW_WORD64          (128u)

/**
 * @brief Shift bits for word addressing.
 */
#define FBFE_WORD_ADDR_SHIFT_BITS        (2u)

/**
 * @brief Mask for 16-bit word addressing.
 */
#define FBFE_WORD_ADDR_16BIT_MASK        (0xffffu)

/**
 * @brief Mask for sign check in ACCREG sign-extension behavior.
 */
#define FE_SIGNCHK_MASK   (0xFFFDFFFFu)

/**
 * @brief Value for sign check in ACCREG sign-extension behavior.
 */
#define FE_SIGNCHK_VAL    (0xFFFFFFFFu)

/**
 * @brief OR mask for sign extension in ACCREG sign-extension behavior.
 */
#define FE_SIGNEXT_OR     (0xFFFC0000u)

/**
 * @brief CTL0 value for MSB pass in 2-bit weights flow.
 */
#ifndef FE_CTL0_MSB
#define FE_CTL0_MSB (0x12019u)
#endif

/**
 * @brief CTL0 value for LSB pass in 2-bit weights flow.
 */
#ifndef FE_CTL0_LSB
#define FE_CTL0_LSB (0x12011u)
#endif


/* ============================================================
 * Internal datapack buffers (library-owned)
 * ============================================================ */

/**
 * @brief Global buffer for storing frame samples.
 *
 * This buffer holds the raw samples for a complete processing frame.
 */
static int16_t g_frame_buffer[NUM_SAMPLES];

/**
 * @brief Global buffer for NPU input data.
 *
 * This buffer holds the packed byte representation of the data,
 * with proper overlapping between frames for continuous processing.
 */
static int8_t  g_fb_input[FB_INPUT_BYTES];            /* packed input to NPU */

/* ============================================================
 * Internal FE scratch buffers (library-owned)
 * Sizes depend on model
 * ============================================================ */

/**
 * @brief Temporary buffer for storing MSB pass results.
 *
 * Used during the NPU processing to store most significant byte results
 * before combining with LSB results.
 */
static int32_t g_msbTemp[MODEL_MAX_OUT_CH_PER_ITER];

/**
 * @brief Temporary buffer for storing LSB pass results.
 *
 * Used during the NPU processing to store least significant byte results
 * before combining with MSB results.
 */
static int32_t g_lsbTemp[MODEL_MAX_OUT_CH_PER_ITER];

/**
 * @brief Buffer for storing maximum values across all strides.
 *
 * This buffer holds the maximum activation values for each output channel
 * across all stride iterations.
 */
static int32_t g_maxValues[MODEL_CONV_OUT_CHANNELS];

/**
 * @brief Buffer for storing single iteration outputs.
 *
 * This buffer holds the output from a single iteration of the NPU processing.
 */
static uint8_t g_singleOut[MODEL_CONV_OUT_CHANNELS];

/**
 * @brief Buffer for storing maxpool operation results.
 *
 * This buffer holds the final maxpooled feature values after processing
 * and scaling.
 */
static uint8_t g_maxpool[MODEL_CONV_OUT_CHANNELS];

/**
 * @brief Start the NPU layer execution.
 *
 * This function triggers the execution of a layer in the NPU by setting
 * the START bit in the CTL5 register.
 *
 * @return None
 */
void TINIE_start(void)
{
    // start layer execution, set START bit 1 in CTL5
    volatile uint32_t* mmrs = (uint32_t*) NPU_getCtlReg();
    mmrs[0] = 0x1;
}

/**
 * @brief Shift the neural network input window to incorporate a new frame.
 *
 * This function updates the feature window by shifting existing features and
 * adding the new frame. It maintains a sliding window of features for temporal
 * context in the neural network input.
 *
 * @param[in,out] ctx Pointer to the filterbank context.
 * @param[in] new_frame Pointer to the new frame data to be incorporated.
 *
 * @return None
 */
static void FBFE_ShiftNNInputWindow(FBFE_Ctx *ctx,
                                   const uint8_t *new_frame)
{
    const uint32_t F = MODEL_CONV_OUT_CHANNELS;
    const uint32_t W = FEATURE_WINDOW;

    memcpy((uint8_t*)&ctx->final_features[F],
           (uint8_t*)&ctx->final_features[2u * F],
           (size_t)(W - 3u) * F);

    memcpy((uint8_t*)&ctx->final_features[(W - 2u) * F],
           (uint8_t*)new_frame,
           F);
}
/* ============================================================
 * Init
 * ============================================================ */

/**
 * @brief Initialize the filterbank feature extraction context.
 *
 * This function initializes the filterbank feature extraction context with
 * the provided configuration and output buffer. It performs parameter validation,
 * sets up the context structure, binds internal buffers, and ensures a
 * deterministic initial state.
 *
 * @param[in,out] ctx Pointer to the context structure to initialize.
 * @param[in] cfg Pointer to the configuration structure.
 * @param[in] final_features Pointer to the buffer where final features will be stored.
 * @param[in] final_features_bytes Size in bytes of the final features buffer.
 *
 * @return true if initialization was successful, false otherwise.
 */
bool FBFE_Init(FBFE_Ctx *ctx,
               const FBFE_Config *cfg,
               uint8_t *final_features,
               size_t final_features_bytes)
{
    /* Parameter validation */
    if (!ctx || !cfg || !final_features) return false;

    /* Verify buffer size is sufficient */
    const size_t need = (size_t)FEATURES_BYTES;
    if (final_features_bytes < need) return false;

    /* Clear context structure */
    memset(ctx, 0, sizeof(*ctx));

    /* Store configuration and buffer pointers */
    ctx->cfg = *cfg;
    ctx->final_features = final_features;
    ctx->final_features_bytes = final_features_bytes;

    /* Bind internal global buffers to context */
    ctx->frame_buffer    = g_frame_buffer;
    ctx->fb_input        = g_fb_input;
    ctx->msb_temp        = g_msbTemp;
    ctx->lsb_temp        = g_lsbTemp;
    ctx->max_values      = g_maxValues;
    ctx->single_out      = g_singleOut;
    ctx->maxpool_outputs = g_maxpool;

    /* Initialize all buffers to zero for deterministic startup */
    memset(ctx->final_features, 0, final_features_bytes);
    memset(ctx->fb_input, 0, sizeof(g_fb_input));
    memset(ctx->frame_buffer, 0, sizeof(g_frame_buffer));
    memset(ctx->max_values, 0, sizeof(g_maxValues));
    memset(ctx->maxpool_outputs, 0, sizeof(g_maxpool));
    memset(ctx->single_out, 0, sizeof(g_singleOut));

    /* Initialize processing indices */
    ctx->sample_index = 0u;
    ctx->pack_index   = 0u;

    /* Mark context as initialized */
    ctx->inited = true;
    return true;
}
/**
 * @brief Push a new audio sample for processing.
 *
 * This function adds a new sample to the processing pipeline. It handles
 * the sample packing, byte splitting, and frame buffer management. When
 * enough samples have been collected to form a complete frame, the function
 * prepares the data for NPU processing by properly arranging it in the input buffer.
 * The sample packing process follows these steps:
 * 1. Split the 16-bit sample into most and least significant bytes using configured masks
 * 2. Store these bytes in temporary buffers
 * 3. When enough samples are collected (pack_size=4), pack them into the frame buffer
 * 4. When a complete frame is collected, update the input buffer with overlap handling
 *
 * The byte packing scheme optimizes memory usage and aligns data for efficient
 * NPU processing by organizing samples in a specific pattern.
 *
 * @param[in,out] ctx Pointer to the initialized context structure.
 * @param[in] sample The sample value to process.
 *
 * @return true if the sample was successfully added, false if the context is invalid.
 */
bool FBFE_PushSample(FBFE_Ctx *ctx, uint16_t sample)
{
    /* Parameter validation */
    if (!ctx || !ctx->inited)
        return false;

    /* Define constants for clarity */
    const uint16_t N = NUM_SAMPLES;
    const uint8_t E = EXTRA_SAMPLES;
    const uint16_t num_bytes = (uint16_t)(N * 2u);
    const uint16_t pack_size = 4;
    int16_t temp = (int16_t)sample;

    /* Split sample into most significant and least significant bytes based on configured masks */
    ctx->ls_bytes[ctx->pack_index] = (uint8_t)(temp & ctx->cfg.ls_mask);
    ctx->ms_bytes[ctx->pack_index] = (int8_t)((temp & ctx->cfg.ms_mask) >> ctx->cfg.ms_shift);

    /* When we've collected enough samples (pack_size), process them as a group */
    if (ctx->pack_index == pack_size - 1)
    {
        uint16_t i = ctx->sample_index;

        /* Pack the bytes into the frame buffer in the correct format */
        ctx->frame_buffer[i - 3u] = (uint16_t)(((ctx->ms_bytes[2] & 0xFFu) << 8) | (ctx->ms_bytes[3] & 0xFFu));
        ctx->frame_buffer[i - 2u] = (uint16_t)(((ctx->ms_bytes[0] & 0xFFu) << 8) |(ctx->ms_bytes[1] & 0xFFu));
        ctx->frame_buffer[i - 1u] = (uint16_t)(((ctx->ls_bytes[2] & 0xFFu) << 8) | (ctx->ls_bytes[3] & 0xFFu));
        ctx->frame_buffer[i]      = (uint16_t)(((ctx->ls_bytes[0] & 0xFFu) << 8) |(ctx->ls_bytes[1] & 0xFFu));
    }

    /* When we've reached the end of a frame, prepare the input buffer for NPU processing */
    if (ctx->sample_index == (uint16_t)(N - 1u))
    {
        /* Shift the extra samples (overlap) at the beginning */
        memcpy(&ctx->fb_input[0], &ctx->fb_input[num_bytes], (size_t)(E * 2u));

        /* Copy the new frame samples after the overlap region */
        memcpy(&ctx->fb_input[E * 2u], ctx->frame_buffer, (size_t)num_bytes);
    }

    /* Update indices for the next sample */
    ctx->pack_index = (uint8_t)((ctx->pack_index + 1u) % pack_size); /* mod 4 */
    ctx->sample_index = (uint8_t)((ctx->sample_index + 1u) % N);

    return true;
}


/**
 * @brief Process one frame of data through the filterbank.
 *
 * This function executes the filterbank feature extraction process on a complete
 * frame of  data using the NPU hardware accelerator. It performs the
 * following steps:
 * 1. Initializes NPU registers
 * 2. Processes data in blocks, corresponding to channel groups
 * 3. For each block, performs multiple stride iterations
 * 4. For each stride, executes both MSB and LSB passes
 * 5. Combines MSB and LSB results and tracks maximum values
 * 6. Applies scaling, offset, and quantization to the output features
 * 7. Updates the sliding feature window with the new results
 *
 * @param[in,out] ctx Pointer to the initialized context structure.
 *
 * @return true if the frame was successfully processed, false if the context is invalid.
 */
bool FBFE_RunOneFrame(FBFE_Ctx *ctx)
{
    if (!ctx || !ctx->inited ) return false;
    
   
    int8_t *fb_input= ctx->fb_input;

    volatile NPU_Regs *npu = (volatile NPU_Regs *)NPU_BASE;
    uint32_t mmr0Val = (((uint32_t)(uintptr_t)fb_input) >> FBFE_WORD_ADDR_SHIFT_BITS) & FBFE_WORD_ADDR_16BIT_MASK;
    volatile uint32_t *dreg0 = &npu->DREG0;

    for (uint16_t block = 0; block < FB_OUTPUT_ITER; block++)
    {
        const uint16_t base_ch = (uint16_t)(block * MODEL_MAX_OUT_CH_PER_ITER);

        /* init max_values for this block range */
        for (uint16_t i = 0; i <MODEL_MAX_OUT_CH_PER_ITER; i++)
        {
            ctx->max_values[base_ch + i] = INT_MIN;
        }

        /* Write MMR words */
        for (size_t i = 0; i < FBANK_MMR_LEN; i++)
        {
            dreg0[i] = FBANK_MMR[i];
        }

        /* CTL1 ifmap/ofmap placement */
        uint32_t ctl_val =
            ((((uint32_t)(uintptr_t)fb_input) >> FBFE_CTL1_ADDR_SHIFT_BITS) & FBFE_CTL1_ADDR_MASK) |
            (((((uint32_t)(uintptr_t)ctx->single_out) >>  FBFE_CTL1_ADDR_SHIFT_BITS) & FBFE_CTL1_ADDR_MASK) << FBFE_CTL1_OFMAP_SHIFT_BITS);
        npu->DREG4 = ctl_val;

        /* AROUT0 */
        npu->DREG8[4] = (((uint32_t)(uintptr_t)ctx->single_out) >> FBFE_WORD_ADDR_SHIFT_BITS) & FBFE_WORD_ADDR_16BIT_MASK;
        /* MMR0 base */
        npu->DREG8[16] = mmr0Val;

        /* ARWT1 */
        
         npu->DREG8[9] = ((uint32_t)(MODEL_CONV_OUT_CHANNELS * (uint32_t)block)) & FBFE_WORD_ADDR_16BIT_MASK;
        /* ARBias0 */
         npu->DREG8[12] = FBFE_ARBIAS0_ROW_WORD64 & FBFE_WORD_ADDR_16BIT_MASK;
        /* Load INS only once */
        if (block == 0u)
        {
            for (size_t i = 0; i < FB_INS_LEN; i++)
            {
                 npu->DREG20[i] = FB_INS[i];
            }
        }

        /* Load params for this block + dummy words */
        const uint32_t params_base = (uint32_t)block * (uint32_t)MODEL_PARAMS_WORDS_PER_ITER ;

        for (uint32_t i = 0; i < (uint32_t)MODEL_MAX_PARAMS_WORDS + (uint32_t)MODEL_DUMMY_TMA_INIT_WORDS; i++)
        {
            if (i < (uint32_t)MODEL_PARAMS_WORDS_PER_ITER)
            {
                const uint32_t idx = params_base + i;

               npu->DREG21[i] = FB_PARAMS[idx];
            }
            else
            {
                  npu->DREG21[i] = 0u;
            }
        }

        /* Stride iterations: MSB pass then LSB pass */
        for (uint16_t stride = 0; stride < MAX_FB_STRIDE_ITER; stride++)
        {
            /* -------- MSB pass -------- */

               npu->DREG7     = 0u;                         /* PC */
            npu->DREG8[16] = mmr0Val + (uint32_t)(FBFE_MMR_STRIDE_STEP * stride); /* MMR0 */
            npu->DREG5     = FE_CTL0_MSB;                /* CTL0 */

            TINIE_start();
            __WFI();

            for (uint16_t i = 0; i <MODEL_MAX_OUT_CH_PER_ITER; i++)
            {
               
                int32_t temp_msb = (int32_t)npu->DREG19[i];
                if (((uint32_t)temp_msb | FE_SIGNCHK_MASK) == FE_SIGNCHK_VAL)
                {
                    temp_msb = (int32_t)((uint32_t)temp_msb | FE_SIGNEXT_OR);
                }
                ctx->msb_temp[i] = temp_msb;
            }

            /* -------- LSB pass -------- */
          
            npu->DREG7     = 0u;                               /* PC */
            npu->DREG8[16] = mmr0Val + (uint32_t)(FBFE_MMR_STRIDE_STEP * stride + 1u); /* MMR0 */
            npu->DREG5     = FE_CTL0_LSB;                



            TINIE_start();
            __WFI();

            for (uint16_t i = 0; i <MODEL_MAX_OUT_CH_PER_ITER; i++)
            {
                int32_t temp_lsb = (int32_t)npu->DREG19[i];
                if (((uint32_t)temp_lsb | FE_SIGNCHK_MASK) == FE_SIGNCHK_VAL)
                {
                    temp_lsb = (int32_t)((uint32_t)temp_lsb | FE_SIGNEXT_OR);
                }
                ctx->lsb_temp[i] = temp_lsb;

                /* Legacy combine (2-bit flow) */
                const int32_t combined = ctx->lsb_temp[i] + (ctx->msb_temp[i] << FBFE_MSB_COMBINE_SHIFT_BITS);

                const uint16_t idx = (uint16_t)(base_ch + i);
                if (combined > ctx->max_values[idx])
                {
                    ctx->max_values[idx] = combined;
                }
            }

            /* Postprocess ONLY on the last stride */
            if (stride == (uint16_t)(MAX_FB_STRIDE_ITER - 1u))
            {
                for (uint16_t i = 0; i <MODEL_MAX_OUT_CH_PER_ITER; i++)
                {
                    const uint16_t idx = (uint16_t)(base_ch + i);
                    int32_t result = ctx->max_values[idx];

                    result = (int32_t)(((result + FB_OFFSET[idx]) * FB_SCALE[idx]) >> FB_SHIFT[idx]);

                    if (result < 0) result = 0;
                    else if (result > 255) result = 255;
                 
                    ctx->maxpool_outputs[idx] = (uint8_t)result;
                
                }
            }
        }
    }
 
    FBFE_ShiftNNInputWindow(ctx, ctx->maxpool_outputs);
    return true;
}
