/**
 * The MIT License
 *
 * Copyright (c) 2019-2020 Ilwoong Jeong (https://github.com/ilwoong)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "hight.h"

//=============================================================================
// auxiliary functions
//=============================================================================
static uint8_t F0(uint8_t x)
{
    uint8_t t1 = (x << 1) ^ (x >> 7);
	uint8_t t2 = (x << 2) ^ (x >> 6);
	uint8_t t3 = (x << 7) ^ (x >> 1);

    return t1 ^ t2 ^ t3;
}

static uint8_t F1(uint8_t x)
{
    uint8_t t1 = (x << 3) ^ (x >> 5);
	uint8_t t2 = (x << 4) ^ (x >> 4);
	uint8_t t3 = (x << 6) ^ (x >> 2);

    return t1 ^ t2 ^ t3;
}

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

static void generate_constatnts(uint8_t* delta)
{
    uint8_t i, j;
    uint8_t s[134] = {0, 1, 0, 1, 1, 0, 1};

    delta[0] = 0b1011010;
    for (i = 1; i < 128; ++i) {
        s[i + 6] = s[i + 2] ^ s[i - 1];

        delta[i] = s[i + 6];
        for (j = 1; j < 7; ++j) {
            delta[i] <<= 1;
            delta[i] ^= s[i + 6 - j];
        }
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
    block[6] = block[5] + (F1(block[4]) ^ rk[2]);
    block[5] = block[4];
    block[4] = block[3] ^ (F0(block[2]) + rk[1]);
    block[3] = block[2]; 
    block[2] = block[1] + (F1(block[0]) ^ rk[0]);
    block[1] = block[0]; 
    block[0] = tmp7 ^ (F0(tmp6) + rk[3]);
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
	block[1] = block[2] - (F1(block[0]) ^ rk[0]);
	block[2] = block[3];
	block[3] = block[4] ^ (F0(block[2]) + rk[1]);
	block[4] = block[5];
	block[5] = block[6] - (F1(block[4]) ^ rk[2]);
	block[6] = block[7];	
	block[7] = tmp0 ^ (F0(block[6]) + rk[3]);
}

//=============================================================================
// key schedule
//=============================================================================
void hight_keygen(uint8_t* rks, const uint8_t* mk)
{
    uint8_t delta[128] = { 0, };
    uint8_t i, j, index;

    generate_whitening_keys(rks, mk);
    rks += 8;

    generate_constatnts(delta);
    for (i = 0; i < 8; ++i) {
        for (j = 0; j < 8; ++j) {
            index = (j - i + 8) & 0x7;
            rks[16 * i + j    ] = mk[index    ] + delta[16 * i + j    ];
            rks[16 * i + j + 8] = mk[index + 8] + delta[16 * i + j + 8];
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