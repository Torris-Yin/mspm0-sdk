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

/*!****************************************************************************
 *  @file       filterbank_feature_extract.h
 *  @brief      Filterbank Feature Extraction API
 *
 *  This module implements filterbank-based feature extraction functionality
 *  for processing and machine learning applications. It provides
 *  an efficient implementation of filterbank analysis using NPU hardware
 *  acceleration.
 *
 ******************************************************************************
 */

#ifndef FILTERBANK_FEATURE_EXTRACT_H
#define FILTERBANK_FEATURE_EXTRACT_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* ================================
* User configuration
* ================================ */

/**
 * @brief Configuration structure for the Filterbank Feature Extraction.
 *
 * This structure holds all the necessary parameters for configuring the
 * filterbank feature extraction process, including sampling rate, window
   * parameters, and bit manipulation settings.
 */
typedef struct
{
    /**
     * @brief Input sampling rate in Hz.
     *
     * This parameter defines the sampling frequency of the input data.
     */
    uint32_t sampling_rate_hz;

    /**
     * @brief Size of the processing window in milliseconds.
     *
     * This parameter defines the time duration of each processing frame.
     */
    uint16_t window_size_ms;

    /**
     * @brief Context size in milliseconds.
     *
     * This parameter defines the amount of contextual information to
     * maintain between frames.
     */
    uint16_t context_ms;

    /**
     * @brief Mask for extracting least significant bits.
     */
    uint16_t ls_mask;

    /**
     * @brief Mask for extracting most significant bits.
     */
    uint16_t ms_mask;

    /**
     * @brief Shift value for most significant bits.
     */
    uint8_t ms_shift;

} FBFE_Config;


/* ================================
* Filterbank context
* ================================ */

/**
 * @brief Context structure for the Filterbank Feature Extraction.
 *
 * This structure holds the current state of the filterbank feature extraction
 * process, including configuration, buffers, and processing state variables.
 */
typedef struct
{
    /**
     * @brief Configuration parameters for the feature extraction.
     */
    FBFE_Config cfg;

    /**
     * @brief Pointer to the buffer where final extracted features are stored.
     */
    uint8_t *final_features;

    /**
     * @brief Size in bytes of the final features buffer.
     */
    size_t final_features_bytes;

    /**
     * @brief Current sample index in the processing frame.
     */
    uint16_t sample_index;

    /**
     * @brief Current packing index used for sample processing.
     */
    uint8_t pack_index;

    /**
     * @brief Buffer for storing least significant bytes during processing.
     */
    uint8_t ls_bytes[4];

    /**
     * @brief Buffer for storing most significant bytes during processing.
     */
    uint8_t ms_bytes[4];

    /**
     * @brief Buffer for raw frame samples.
     */
    int16_t *frame_buffer;     /* raw packed words per frame */

    /**
     * @brief Buffer for NPU input with overlapped frames.
     */
    int8_t  *fb_input;         /* packed bytes (overlapped) */

    /**
     * @brief Temporary buffer for most significant byte processing.
     */
    int32_t *msb_temp;

    /**
     * @brief Temporary buffer for least significant byte processing.
     */
    int32_t *lsb_temp;

    /**
     * @brief Buffer for storing maximum values from filterbank.
     */
    int32_t *max_values;

    /**
     * @brief Output buffer for single filterbank iteration.
     */
    uint8_t *single_out;

    /**
     * @brief Buffer for maxpool operation outputs.
     */
    uint8_t *maxpool_outputs;

    /**
     * @brief Flag indicating whether the context has been initialized.
     */
    bool inited;
} FBFE_Ctx;


/* ================================
* API
* ================================ */

/**
 * @brief Initialize the filterbank feature extraction context.
 *
 * This function initializes the filterbank feature extraction context with the
 * given configuration and output buffer. It sets up all internal buffers and
 * prepares the context for processing.
 *
 * @param[in,out] ctx Pointer to the context structure to initialize.
   *                    Must be allocated by the caller
 * @param[in] cfg Pointer to configuration structure containing filterbank
   *                parameters. This structure is copied into the context.
 * @param[in] final_features Pointer to buffer where final features will be stored.
 * @param[in] final_features_bytes Size in bytes of the final features buffer.
 *
 * @return true if initialization was successful, false otherwise.
 */
bool FBFE_Init(FBFE_Ctx *ctx,
               const FBFE_Config *cfg,
               uint8_t *final_features,
               size_t final_features_bytes);

/**
 * @brief Push a new sample for processing.
 *
 * This function adds a new sample to the processing buffer. Once enough
 * samples are collected to form a complete frame, they will be ready for
 * processing with FBFE_RunOneFrame.
 *
 * @param[in,out] ctx Pointer to the initialized context structure.
 * @param[in] sample The sample value to pack.
 *
 * @return true if the sample was successfully added, false otherwise.
 */
bool FBFE_PushSample(FBFE_Ctx *ctx, uint16_t sample);

/**
 * @brief Process one frame of data through the filterbank.
 *
 * This function processes a complete frame of data through the filterbank
 * feature extraction pipeline using NPU hardware acceleration. The results are
 * stored in the final features buffer provided during initialization.
 *
 * @param[in,out] ctx Pointer to the initialized context structure.
 *
 * @return true if the frame was successfully processed, false otherwise.
 */
bool FBFE_RunOneFrame(FBFE_Ctx *ctx);

#endif /* FILTERBANK_FEATURE_EXTRACT_H */