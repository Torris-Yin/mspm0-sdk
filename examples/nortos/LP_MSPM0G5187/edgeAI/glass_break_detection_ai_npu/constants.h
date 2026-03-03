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

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <stdint.h>
#include "model_AUTOGEN.h"

/* ============================================================
 * User-controlled audio configuration
 * ============================================================ */

/* Sampling rate in Hz (e.g. 8000 or 16000) */
#define SAMPLING_RATE_HZ        (8000u)

/* Feature extraction hop size */
#define WINDOW_SIZE_MS          (60u)

/* Total audio context required by model (ms) */
#define CONTEXT_MS        (3000u)

#define LS_MASK                     (0x00FF)
#define MS_MASK                     (0xFF00)
#define MS_SHIFT                    (8)

/* ============================================================
 * Derived audio geometry
 * ============================================================ */

/* Samples per frame */
#define NUM_SAMPLES \
    ((SAMPLING_RATE_HZ * WINDOW_SIZE_MS) / 1000u)

/* Convolution overlap */
#define EXTRA_SAMPLES \
    (MODEL_CONV_KERNEL_T - MODEL_CONV_STRIDE_T)

/* Number of stride iterations */
#define MAX_FB_STRIDE_ITER \
    (NUM_SAMPLES / MODEL_CONV_STRIDE_T)

/* Output channel block iterations */
#define FB_OUTPUT_ITER \
    (MODEL_CONV_OUT_CHANNELS / MODEL_MAX_OUT_CH_PER_ITER)

/* ============================================================
 * Rolling NN input window
 * ============================================================ */

/* Frames of context */
#define MODEL_CONTEXT_FRAMES \
    (CONTEXT_MS / WINDOW_SIZE_MS)

/* +1 legacy slack frames */
#define FEATURE_WINDOW \
    (MODEL_CONTEXT_FRAMES + 1u)

/* ============================================================
 * Buffer sizes
 * ============================================================ */

/* Packed FB input buffer size (bytes) */
#define FB_INPUT_BYTES \
    ((NUM_SAMPLES + EXTRA_SAMPLES) * 2u)

/* One feature frame length (bytes) */
#define FB_FEATURE_FRAME_BYTES \
    (MODEL_CONV_OUT_CHANNELS)

/* Rolling feature buffer size (bytes) */
#define FEATURES_BYTES \
    (FEATURE_WINDOW * MODEL_CONV_OUT_CHANNELS)

#endif /* CONSTANTS_H */