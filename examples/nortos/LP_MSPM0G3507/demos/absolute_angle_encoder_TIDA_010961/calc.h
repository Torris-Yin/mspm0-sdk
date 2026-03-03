/*
 * Copyright (c) 2026, Texas Instruments Incorporated
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

#ifndef CALC_H_
#define CALC_H_

#include <ti/iqmath/include/IQmathLib.h>
#include "ti_msp_dl_config.h"

typedef struct {
    uint16_t Adc[2];
} RawData;

//Define handle
typedef RawData *RAWDATA_HANDLE;

#define RAWDATA_DEFAULTS \
    {                    \
        {                \
            0, 0         \
        }                \
    }

typedef struct {
    _iq Sin;
    _iq Cos;
} SinCosVolts;

//Define handle
typedef SinCosVolts *SINCOSVOLTS_HANDLE;

#define SINCOSVOLTS_DEFAULTS \
    {                        \
        _IQ(0.0), _IQ(0.0)   \
    }

typedef struct {
    RawData RawData1;
    RawData RawData2;
    SinCosVolts SinCos1;
    SinCosVolts SinCos2;
} SinCosIn;

//Define handle:
/////////////////////////////////////////////////////////////////////////////////////////////
typedef SinCosIn *SINCOSIN_HANDLE;

#endif /* CALC_H_ */
