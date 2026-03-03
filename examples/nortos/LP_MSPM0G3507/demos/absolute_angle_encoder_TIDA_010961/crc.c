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

#include "crc.h"
#include <stdio.h>
#include "ti_msp_dl_config.h"

typedef enum {
    REF_4BIT  = 4,
    REF_5BIT  = 5,
    REF_6BIT  = 6,
    REF_7BIT  = 7,
    REF_8BIT  = 8,
    REF_16BIT = 16,
    REF_32BIT = 32
} REFLECTED_MODE;

uint32_t ReflectedData(uint32_t data, REFLECTED_MODE mode)
{
    data = ((data & 0xffff0000) >> 16) | ((data & 0x0000ffff) << 16);
    data = ((data & 0xff00ff00) >> 8) | ((data & 0x00ff00ff) << 8);
    data = ((data & 0xf0f0f0f0) >> 4) | ((data & 0x0f0f0f0f) << 4);
    data = ((data & 0xcccccccc) >> 2) | ((data & 0x33333333) << 2);
    data = ((data & 0xaaaaaaaa) >> 1) | ((data & 0x55555555) << 1);

    switch (mode) {
        case REF_32BIT:
            return data;
        case REF_16BIT:
            return (data >> 16) & 0xffff;
        case REF_8BIT:
            return (data >> 24) & 0xff;
        case REF_7BIT:
            return (data >> 25) & 0x7f;
        case REF_6BIT:
            return (data >> 26) & 0x7f;
        case REF_5BIT:
            return (data >> 27) & 0x1f;
        case REF_4BIT:
            return (data >> 28) & 0x0f;
    }
    return 0;
}

uint8_t CheckCrc4(uint8_t poly, uint8_t init, bool refIn, bool refOut,
    uint8_t xorOut, const uint8_t *buffer, uint32_t length)
{
    uint8_t i;
    uint8_t crc;

    if (refIn == true) {
        crc  = init;
        poly = ReflectedData(poly, REF_4BIT);

        while (length--) {
            crc ^= *buffer++;
            for (i = 0; i < 8; i++) {
                if (crc & 0x01) {
                    crc >>= 1;
                    crc ^= poly;
                } else {
                    crc >>= 1;
                }
            }
        }

        return crc ^ xorOut;
    } else {
        crc = init << 4;
        poly <<= 4;

        while (length--) {
            crc ^= *buffer++;
            for (i = 0; i < 8; i++) {
                if (crc & 0x80) {
                    crc <<= 1;
                    crc ^= poly;
                } else {
                    crc <<= 1;
                }
            }
        }

        return (crc >> 4) ^ xorOut;
    }
}

uint8_t CheckCrc5(uint8_t poly, uint8_t init, bool refIn, bool refOut,
    uint8_t xorOut, const uint8_t *buffer, uint32_t length)
{
    uint8_t i;
    uint8_t crc;

    if (refIn == true) {
        crc  = init;
        poly = ReflectedData(poly, REF_5BIT);

        while (length--) {
            crc ^= *buffer++;
            for (i = 0; i < 8; i++) {
                if (crc & 0x01) {
                    crc >>= 1;
                    crc ^= poly;
                } else {
                    crc >>= 1;
                }
            }
        }

        return crc ^ xorOut;
    } else {
        crc = init << 3;
        poly <<= 3;

        while (length--) {
            crc ^= *buffer++;
            for (i = 0; i < 8; i++) {
                if (crc & 0x80) {
                    crc <<= 1;
                    crc ^= poly;
                } else {
                    crc <<= 1;
                }
            }
        }

        return (crc >> 3) ^ xorOut;
    }
}

uint8_t CheckCrc6(uint8_t poly, uint8_t init, bool refIn, bool refOut,
    uint8_t xorOut, const uint8_t *buffer, uint32_t length)
{
    uint8_t i;
    uint8_t crc;

    if (refIn == true) {
        crc  = init;
        poly = ReflectedData(poly, REF_6BIT);

        while (length--) {
            crc ^= *buffer++;
            for (i = 0; i < 8; i++) {
                if (crc & 0x01) {
                    crc >>= 1;
                    crc ^= poly;
                } else {
                    crc >>= 1;
                }
            }
        }

        return crc ^ xorOut;
    } else {
        crc = init << 2;
        poly <<= 2;

        while (length--) {
            crc ^= *buffer++;
            for (i = 0; i < 8; i++) {
                if (crc & 0x80) {
                    crc <<= 1;
                    crc ^= poly;
                } else {
                    crc <<= 1;
                }
            }
        }

        return (crc >> 2) ^ xorOut;
    }
}

uint8_t CheckCrc7(uint8_t poly, uint8_t init, bool refIn, bool refOut,
    uint8_t xorOut, const uint8_t *buffer, uint32_t length)
{
    uint8_t i;
    uint8_t crc;

    if (refIn == true) {
        crc  = init;
        poly = ReflectedData(poly, REF_7BIT);

        while (length--) {
            crc ^= *buffer++;
            for (i = 0; i < 8; i++) {
                if (crc & 0x01) {
                    crc >>= 1;
                    crc ^= poly;
                } else {
                    crc >>= 1;
                }
            }
        }

        return crc ^ xorOut;
    } else {
        crc = init << 1;
        poly <<= 1;

        while (length--) {
            crc ^= *buffer++;
            for (i = 0; i < 8; i++) {
                if (crc & 0x80) {
                    crc <<= 1;
                    crc ^= poly;
                } else {
                    crc <<= 1;
                }
            }
        }

        return (crc >> 1) ^ xorOut;
    }
}

uint8_t CheckCrc8(uint8_t poly, uint8_t init, bool refIn, bool refOut,
    uint8_t xorOut, uint8_t *buffer, uint32_t length)
{
    uint32_t i  = 0;
    uint8_t crc = init;

    while (length--) {
        if (refIn == true) {
            crc ^= ReflectedData(*buffer++, REF_8BIT);
        } else {
            crc ^= *buffer++;
        }

        for (i = 0; i < 8; i++) {
            if (crc & 0x80) {
                crc <<= 1;
                crc ^= poly;
            } else {
                crc <<= 1;
            }
        }
    }

    if (refOut == true) {
        crc = ReflectedData(crc, REF_8BIT);
    }

    return crc ^ xorOut;
}

uint16_t CheckCrc16(uint16_t poly, uint16_t init, bool refIn, bool refOut,
    uint16_t xorOut, const uint8_t *buffer, uint32_t length)
{
    uint32_t i   = 0;
    uint16_t crc = init;

    while (length--) {
        if (refIn == true) {
            crc ^= ReflectedData(*buffer++, REF_8BIT) << 8;
        } else {
            crc ^= (*buffer++) << 8;
        }

        for (i = 0; i < 8; i++) {
            if (crc & 0x8000) {
                crc <<= 1;
                crc ^= poly;
            } else {
                crc <<= 1;
            }
        }
    }

    if (refOut == true) {
        crc = ReflectedData(crc, REF_16BIT);
    }

    return crc ^ xorOut;
}

uint32_t CheckCrc32(uint32_t poly, uint32_t init, bool refIn, bool refOut,
    uint32_t xorOut, const uint8_t *buffer, uint32_t length)
{
    uint32_t i   = 0;
    uint32_t crc = init;

    while (length--) {
        if (refIn == true) {
            crc ^= ReflectedData(*buffer++, REF_8BIT) << 24;
        } else {
            crc ^= (*buffer++) << 24;
        }

        for (i = 0; i < 8; i++) {
            if (crc & 0x80000000) {
                crc <<= 1;
                crc ^= poly;
            } else {
                crc <<= 1;
            }
        }
    }

    if (refOut == true) {
        crc = ReflectedData(crc, REF_32BIT);
    }

    return crc ^ xorOut;
}

uint32_t CrcCheck(CRC_Type crcType, const uint8_t *buffer, uint32_t length)
{
    switch (crcType.width) {
        case 4:
            return CheckCrc4(crcType.poly, crcType.init, crcType.refIn,
                crcType.refOut, crcType.xorOut, buffer, length);
        case 5:
            return CheckCrc5(crcType.poly, crcType.init, crcType.refIn,
                crcType.refOut, crcType.xorOut, buffer, length);
        case 6:
            return CheckCrc6(crcType.poly, crcType.init, crcType.refIn,
                crcType.refOut, crcType.xorOut, buffer, length);
        case 7:
            return CheckCrc7(crcType.poly, crcType.init, crcType.refIn,
                crcType.refOut, crcType.xorOut, buffer, length);
        case 8:
            return CheckCrc8(crcType.poly, crcType.init, crcType.refIn,
                crcType.refOut, crcType.xorOut, buffer, length);
        case 16:
            return CheckCrc16(crcType.poly, crcType.init, crcType.refIn,
                crcType.refOut, crcType.xorOut, buffer, length);
        case 32:
            return CheckCrc32(crcType.poly, crcType.init, crcType.refIn,
                crcType.refOut, crcType.xorOut, buffer, length);
    }
    return 0;
}

// CRC8 table X^8 + 1
static const unsigned int crc8Table[256] = {
    0x00,
    0x01,
    0x02,
    0x03,
    0x04,
    0x05,
    0x06,
    0x07,
    0x08,
    0x09,
    0x0A,
    0x0B,
    0x0C,
    0x0D,
    0x0E,
    0x0F,
    0x10,
    0x11,
    0x12,
    0x13,
    0x14,
    0x15,
    0x16,
    0x17,
    0x18,
    0x19,
    0x1A,
    0x1B,
    0x1C,
    0x1D,
    0x1E,
    0x1F,
    0x20,
    0x21,
    0x22,
    0x23,
    0x24,
    0x25,
    0x26,
    0x27,
    0x28,
    0x29,
    0x2A,
    0x2B,
    0x2C,
    0x2D,
    0x2E,
    0x2F,
    0x30,
    0x31,
    0x32,
    0x33,
    0x34,
    0x35,
    0x36,
    0x37,
    0x38,
    0x39,
    0x3A,
    0x3B,
    0x3C,
    0x3D,
    0x3E,
    0x3F,
    0x40,
    0x41,
    0x42,
    0x43,
    0x44,
    0x45,
    0x46,
    0x47,
    0x48,
    0x49,
    0x4A,
    0x4B,
    0x4C,
    0x4D,
    0x4E,
    0x4F,
    0x50,
    0x51,
    0x52,
    0x53,
    0x54,
    0x55,
    0x56,
    0x57,
    0x58,
    0x59,
    0x5A,
    0x5B,
    0x5C,
    0x5D,
    0x5E,
    0x5F,
    0x60,
    0x61,
    0x62,
    0x63,
    0x64,
    0x65,
    0x66,
    0x67,
    0x68,
    0x69,
    0x6A,
    0x6B,
    0x6C,
    0x6D,
    0x6E,
    0x6F,
    0x70,
    0x71,
    0x72,
    0x73,
    0x74,
    0x75,
    0x76,
    0x77,
    0x78,
    0x79,
    0x7A,
    0x7B,
    0x7C,
    0x7D,
    0x7E,
    0x7F,
    0x80,
    0x81,
    0x82,
    0x83,
    0x84,
    0x85,
    0x86,
    0x87,
    0x88,
    0x89,
    0x8A,
    0x8B,
    0x8C,
    0x8D,
    0x8E,
    0x8F,
    0x90,
    0x91,
    0x92,
    0x93,
    0x94,
    0x95,
    0x96,
    0x97,
    0x98,
    0x99,
    0x9A,
    0x9B,
    0x9C,
    0x9D,
    0x9E,
    0x9F,
    0xA0,
    0xA1,
    0xA2,
    0xA3,
    0xA4,
    0xA5,
    0xA6,
    0xA7,
    0xA8,
    0xA9,
    0xAA,
    0xAB,
    0xAC,
    0xAD,
    0xAE,
    0xAF,
    0xB0,
    0xB1,
    0xB2,
    0xB3,
    0xB4,
    0xB5,
    0xB6,
    0xB7,
    0xB8,
    0xB9,
    0xBA,
    0xBB,
    0xBC,
    0xBD,
    0xBE,
    0xBF,
    0xC0,
    0xC1,
    0xC2,
    0xC3,
    0xC4,
    0xC5,
    0xC6,
    0xC7,
    0xC8,
    0xC9,
    0xCA,
    0xCB,
    0xCC,
    0xCD,
    0xCE,
    0xCF,
    0xD0,
    0xD1,
    0xD2,
    0xD3,
    0xD4,
    0xD5,
    0xD6,
    0xD7,
    0xD8,
    0xD9,
    0xDA,
    0xDB,
    0xDC,
    0xDD,
    0xDE,
    0xDF,
    0xE0,
    0xE1,
    0xE2,
    0xE3,
    0xE4,
    0xE5,
    0xE6,
    0xE7,
    0xE8,
    0xE9,
    0xEA,
    0xEB,
    0xEC,
    0xED,
    0xEE,
    0xEF,
    0xF0,
    0xF1,
    0xF2,
    0xF3,
    0xF4,
    0xF5,
    0xF6,
    0xF7,
    0xF8,
    0xF9,
    0xFA,
    0xFB,
    0xFC,
    0xFD,
    0xFE,
    0xFF,
};

uint8_t CalcCRC(unsigned char *data, unsigned int len)
{
    unsigned char crc8 = 0;

    while (len--) {
        crc8 = crc8 ^ (*data++);
        crc8 = crc8Table[crc8];
    }

    return crc8;
}
