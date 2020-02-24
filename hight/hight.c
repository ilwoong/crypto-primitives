/**
 * MIT License
 * 
 * Copyright (c) 2019-2020 Ilwoong Jeong (https://github.com/ilwoong)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"), 
 * to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "hight.h"

//=============================================================================
// lookup tables
//=============================================================================
static const uint8_t F0[256] = {
    0x00, 0x86, 0x0d, 0x8b, 0x1a, 0x9c, 0x17, 0x91, 0x34, 0xb2, 0x39, 0xbf, 0x2e, 0xa8, 0x23, 0xa5, 
    0x68, 0xee, 0x65, 0xe3, 0x72, 0xf4, 0x7f, 0xf9, 0x5c, 0xda, 0x51, 0xd7, 0x46, 0xc0, 0x4b, 0xcd,
    0xd0, 0x56, 0xdd, 0x5b, 0xca, 0x4c, 0xc7, 0x41, 0xe4, 0x62, 0xe9, 0x6f, 0xfe, 0x78, 0xf3, 0x75,
    0xb8, 0x3e, 0xb5, 0x33, 0xa2, 0x24, 0xaf, 0x29, 0x8c, 0x0a, 0x81, 0x07, 0x96, 0x10, 0x9b, 0x1d,
    0xa1, 0x27, 0xac, 0x2a, 0xbb, 0x3d, 0xb6, 0x30, 0x95, 0x13, 0x98, 0x1e, 0x8f, 0x09, 0x82, 0x04,
    0xc9, 0x4f, 0xc4, 0x42, 0xd3, 0x55, 0xde, 0x58, 0xfd, 0x7b, 0xf0, 0x76, 0xe7, 0x61, 0xea, 0x6c,
    0x71, 0xf7, 0x7c, 0xfa, 0x6b, 0xed, 0x66, 0xe0, 0x45, 0xc3, 0x48, 0xce, 0x5f, 0xd9, 0x52, 0xd4,
    0x19, 0x9f, 0x14, 0x92, 0x03, 0x85, 0x0e, 0x88, 0x2d, 0xab, 0x20, 0xa6, 0x37, 0xb1, 0x3a, 0xbc,
    0x43, 0xc5, 0x4e, 0xc8, 0x59, 0xdf, 0x54, 0xd2, 0x77, 0xf1, 0x7a, 0xfc, 0x6d, 0xeb, 0x60, 0xe6, 
    0x2b, 0xad, 0x26, 0xa0, 0x31, 0xb7, 0x3c, 0xba, 0x1f, 0x99, 0x12, 0x94, 0x05, 0x83, 0x08, 0x8e,
    0x93, 0x15, 0x9e, 0x18, 0x89, 0x0f, 0x84, 0x02, 0xa7, 0x21, 0xaa, 0x2c, 0xbd, 0x3b, 0xb0, 0x36,
    0xfb, 0x7d, 0xf6, 0x70, 0xe1, 0x67, 0xec, 0x6a, 0xcf, 0x49, 0xc2, 0x44, 0xd5, 0x53, 0xd8, 0x5e,
    0xe2, 0x64, 0xef, 0x69, 0xf8, 0x7e, 0xf5, 0x73, 0xd6, 0x50, 0xdb, 0x5d, 0xcc, 0x4a, 0xc1, 0x47,
    0x8a, 0x0c, 0x87, 0x01, 0x90, 0x16, 0x9d, 0x1b, 0xbe, 0x38, 0xb3, 0x35, 0xa4, 0x22, 0xa9, 0x2f,
    0x32, 0xb4, 0x3f, 0xb9, 0x28, 0xae, 0x25, 0xa3, 0x06, 0x80, 0x0b, 0x8d, 0x1c, 0x9a, 0x11, 0x97,
    0x5a, 0xdc, 0x57, 0xd1, 0x40, 0xc6, 0x4d, 0xcb, 0x6e, 0xe8, 0x63, 0xe5, 0x74, 0xf2, 0x79, 0xff,
};

static const uint8_t F1[256] = {
    0x00, 0x58, 0xb0, 0xe8, 0x61, 0x39, 0xd1, 0x89, 0xc2, 0x9a, 0x72, 0x2a, 0xa3, 0xfb, 0x13, 0x4b, 
    0x85, 0xdd, 0x35, 0x6d, 0xe4, 0xbc, 0x54, 0x0c, 0x47, 0x1f, 0xf7, 0xaf, 0x26, 0x7e, 0x96, 0xce,
    0x0b, 0x53, 0xbb, 0xe3, 0x6a, 0x32, 0xda, 0x82, 0xc9, 0x91, 0x79, 0x21, 0xa8, 0xf0, 0x18, 0x40,
    0x8e, 0xd6, 0x3e, 0x66, 0xef, 0xb7, 0x5f, 0x07, 0x4c, 0x14, 0xfc, 0xa4, 0x2d, 0x75, 0x9d, 0xc5,
    0x16, 0x4e, 0xa6, 0xfe, 0x77, 0x2f, 0xc7, 0x9f, 0xd4, 0x8c, 0x64, 0x3c, 0xb5, 0xed, 0x05, 0x5d,
    0x93, 0xcb, 0x23, 0x7b, 0xf2, 0xaa, 0x42, 0x1a, 0x51, 0x09, 0xe1, 0xb9, 0x30, 0x68, 0x80, 0xd8,
    0x1d, 0x45, 0xad, 0xf5, 0x7c, 0x24, 0xcc, 0x94, 0xdf, 0x87, 0x6f, 0x37, 0xbe, 0xe6, 0x0e, 0x56,
    0x98, 0xc0, 0x28, 0x70, 0xf9, 0xa1, 0x49, 0x11, 0x5a, 0x02, 0xea, 0xb2, 0x3b, 0x63, 0x8b, 0xd3,
    0x2c, 0x74, 0x9c, 0xc4, 0x4d, 0x15, 0xfd, 0xa5, 0xee, 0xb6, 0x5e, 0x06, 0x8f, 0xd7, 0x3f, 0x67,
    0xa9, 0xf1, 0x19, 0x41, 0xc8, 0x90, 0x78, 0x20, 0x6b, 0x33, 0xdb, 0x83, 0x0a, 0x52, 0xba, 0xe2,
    0x27, 0x7f, 0x97, 0xcf, 0x46, 0x1e, 0xf6, 0xae, 0xe5, 0xbd, 0x55, 0x0d, 0x84, 0xdc, 0x34, 0x6c,
    0xa2, 0xfa, 0x12, 0x4a, 0xc3, 0x9b, 0x73, 0x2b, 0x60, 0x38, 0xd0, 0x88, 0x01, 0x59, 0xb1, 0xe9, 
    0x3a, 0x62, 0x8a, 0xd2, 0x5b, 0x03, 0xeb, 0xb3, 0xf8, 0xa0, 0x48, 0x10, 0x99, 0xc1, 0x29, 0x71,
    0xbf, 0xe7, 0x0f, 0x57, 0xde, 0x86, 0x6e, 0x36, 0x7d, 0x25, 0xcd, 0x95, 0x1c, 0x44, 0xac, 0xf4,
    0x31, 0x69, 0x81, 0xd9, 0x50, 0x08, 0xe0, 0xb8, 0xf3, 0xab, 0x43, 0x1b, 0x92, 0xca, 0x22, 0x7a,
    0xb4, 0xec, 0x04, 0x5c, 0xd5, 0x8d, 0x65, 0x3d, 0x76, 0x2e, 0xc6, 0x9e, 0x17, 0x4f, 0xa7, 0xff,
};

static const uint8_t DELTA[128] = {
    0x5a, 0x6d, 0x36, 0x1b, 0x0d, 0x06, 0x03, 0x41, 0x60, 0x30, 0x18, 0x4c, 0x66, 0x33, 0x59, 0x2c,
    0x56, 0x2b, 0x15, 0x4a, 0x65, 0x72, 0x39, 0x1c, 0x4e, 0x67, 0x73, 0x79, 0x3c, 0x5e, 0x6f, 0x37,
    0x5b, 0x2d, 0x16, 0x0b, 0x05, 0x42, 0x21, 0x50, 0x28, 0x54, 0x2a, 0x55, 0x6a, 0x75, 0x7a, 0x7d,
    0x3e, 0x5f, 0x2f, 0x17, 0x4b, 0x25, 0x52, 0x29, 0x14, 0x0a, 0x45, 0x62, 0x31, 0x58, 0x6c, 0x76,
    0x3b, 0x1d, 0x0e, 0x47, 0x63, 0x71, 0x78, 0x7c, 0x7e, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x43, 0x61,
    0x70, 0x38, 0x5c, 0x6e, 0x77, 0x7b, 0x3d, 0x1e, 0x4f, 0x27, 0x53, 0x69, 0x34, 0x1a, 0x4d, 0x26,
    0x13, 0x49, 0x24, 0x12, 0x09, 0x04, 0x02, 0x01, 0x40, 0x20, 0x10, 0x08, 0x44, 0x22, 0x11, 0x48,
    0x64, 0x32, 0x19, 0x0c, 0x46, 0x23, 0x51, 0x68, 0x74, 0x3a, 0x5d, 0x2e, 0x57, 0x6b, 0x35, 0x5a,
};

//=============================================================================
// functions for key schedule
//=============================================================================
static void generate_whitening_keys(uint8_t* wk, const uint8_t* mk)
{
    uint8_t i;

    for (i = 0; i < 4; ++i) {
        wk[i] = mk[i + 12];
    }

    for (i = 4; i < 8; ++i) {
        wk[i] = mk[i - 4];
    }
}

//=============================================================================
// functions for encryption
//=============================================================================
static void initial_transformation(uint8_t* dst, const uint8_t* src, const uint8_t* rk)
{
    dst[0] = src[0] + rk[0];
    dst[1] = src[1];
    dst[2] = src[2] ^ rk[1];
    dst[3] = src[3];
    dst[4] = src[4] + rk[2];
    dst[5] = src[5];
    dst[6] = src[6] ^ rk[3];
    dst[7] = src[7];
}

static void final_transformation(uint8_t* dst, const uint8_t* src, const uint8_t* rk)
{
    dst[0] = src[1] + rk[4];
    dst[1] = src[2];
    dst[2] = src[3] ^ rk[5];
    dst[3] = src[4];
    dst[4] = src[5] + rk[6];
    dst[5] = src[6];
    dst[6] = src[7] ^ rk[7];
    dst[7] = src[0];
}

static void enc_round(uint8_t* block, const uint8_t* rk)
{
    uint8_t tmp6 = block[6];
    uint8_t tmp7 = block[7];

    block[7] = block[6];
    block[6] = block[5] + (F1[block[4]] ^ rk[2]);
    block[5] = block[4];
    block[4] = block[3] ^ (F0[block[2]] + rk[1]);
    block[3] = block[2]; 
    block[2] = block[1] + (F1[block[0]] ^ rk[0]);
    block[1] = block[0]; 
    block[0] = tmp7 ^ (F0[tmp6] + rk[3]);
}

//=============================================================================
// functions for decryption
//=============================================================================
static void inverse_initial_transformation(uint8_t* dst, const uint8_t* src, const uint8_t* rk)
{
    dst[0] = src[0] - rk[0];
    dst[1] = src[1];
    dst[2] = src[2] ^ rk[1];
    dst[3] = src[3];
    dst[4] = src[4] - rk[2];
    dst[5] = src[5];
    dst[6] = src[6] ^ rk[3];
    dst[7] = src[7];
}

static void inverse_final_transformation(uint8_t* dst, const uint8_t* src, const uint8_t* rk)
{
    dst[7] = src[6] ^ rk[7];
    dst[6] = src[5];
    dst[5] = src[4] - rk[6];
    dst[4] = src[3];
    dst[3] = src[2] ^ rk[5];
    dst[2] = src[1];
    dst[1] = src[0] - rk[4];
    dst[0] = src[7];
}

static void dec_round(uint8_t* block, const uint8_t* rk)
{
    uint8_t tmp0 = block[0];

    block[0] = block[1];
	block[1] = block[2] - (F1[block[0]] ^ rk[0]);
	block[2] = block[3];
	block[3] = block[4] ^ (F0[block[2]] + rk[1]);
	block[4] = block[5];
	block[5] = block[6] - (F1[block[4]] ^ rk[2]);
	block[6] = block[7];	
	block[7] = tmp0 ^ (F0[block[6]] + rk[3]);
}

//=============================================================================
// key schedule
//=============================================================================
void hight_keygen(uint8_t* rks, const uint8_t* mk)
{
    uint8_t i, j, index;

    generate_whitening_keys(rks, mk);
    rks += 8;

    for (i = 0; i < 8; ++i) {
        for (j = 0; j < 8; ++j) {
            index = (j - i + 8) & 0x7;
            rks[16 * i + j    ] = mk[index    ] + DELTA[16 * i + j    ];
            rks[16 * i + j + 8] = mk[index + 8] + DELTA[16 * i + j + 8];
        }
    }
}

//=============================================================================
// encryption
//=============================================================================
void hight_encrypt(uint8_t* dst, const uint8_t* src, const uint8_t* rks)
{
    uint8_t round;
    const uint8_t* wk = rks;
    const uint8_t* rk = rks + 8;
    uint8_t block[8] = {0,};

    initial_transformation(block, src, wk);
    
    for (round = 0; round < HIGHT_ROUNDS; ++round) {
        enc_round(block, rk);
        rk += 4;
    }

    final_transformation(dst, block, wk);
}

//=============================================================================
// decryption
//=============================================================================
void hight_decrypt(uint8_t* dst, const uint8_t* src, const uint8_t* rks)
{
    uint8_t round;
    const uint8_t* wk = rks;
    const uint8_t* rk = rks + 132;
    uint8_t block[8] = {0,};

    inverse_final_transformation(block, src, wk);

    for (round = 0; round < HIGHT_ROUNDS; ++round) {
        dec_round(block, rk);
        rk -= 4;
    }

    inverse_initial_transformation(dst, block, wk);
}