/**
 * MIT License
 * 
 * Copyright (c) 2019 Ilwoong Jeong, https://github.com/ilwoong
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
// auxiliary functions
//=============================================================================
static uint8_t F0(uint8_t x)
{
    uint8_t t1 = ((x << 1) & 0xff) ^ ((x >> 7) & 0xff);
	uint8_t t2 = ((x << 2) & 0xff) ^ ((x >> 6) & 0xff);
	uint8_t t3 = ((x << 7) & 0xff) ^ ((x >> 1) & 0xff);

    return t1 ^ t2 ^ t3;
}

static uint8_t F1(uint8_t x)
{
    uint8_t t1 = ((x << 3) & 0xff) ^ ((x >> 5) & 0xff);
	uint8_t t2 = ((x << 4) & 0xff) ^ ((x >> 4) & 0xff);
	uint8_t t3 = ((x << 6) & 0xff) ^ ((x >> 2) & 0xff);

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
static void initial_transformation(uint8_t* out, const uint8_t* in, const uint8_t* rk)
{
    out[0] = (in[0] + rk[0]) & 0xff;
    out[1] = (in[1]);
    out[2] = (in[2] ^ rk[1]);
    out[3] = (in[3]);
    out[4] = (in[4] + rk[2]) & 0xff;
    out[5] = (in[5]);
    out[6] = (in[6] ^ rk[3]);
    out[7] = (in[7]);
}

static void final_transformation(uint8_t* out, const uint8_t* in, const uint8_t* rk)
{
    out[0] = (in[1] + rk[4]) & 0xff;
    out[1] = (in[2]);
    out[2] = (in[3] ^ rk[5]) & 0xff;
    out[3] = (in[4]);
    out[4] = (in[5] + rk[6]) & 0xff;
    out[5] = (in[6]);
    out[6] = (in[7] ^ rk[7]) & 0xff;
    out[7] = (in[0]);
}

static void enc_round(uint8_t* out, const uint8_t* rk)
{
    uint8_t tmp6 = out[6];
    uint8_t tmp7 = out[7];

    out[7] = (out[6]);
    out[6] = (out[5] + (F1(out[4]) ^ rk[2])) & 0xff;
    out[5] = (out[4]);
    out[4] = (out[3]) ^ ((F0(out[2]) + rk[1]) & 0xff);
    out[3] = (out[2]); 
    out[2] = (out[1] + (F1(out[0]) ^ rk[0])) & 0xff;
    out[1] = (out[0]); 
    out[0] = (tmp7) ^ ((F0(tmp6) + rk[3]) & 0xff);
}

//=============================================================================
// functions for decryption
//=============================================================================
static void inverse_initial_transformation(uint8_t* out, const uint8_t* in, const uint8_t* rk)
{
    out[0] = (in[0] - rk[0]) & 0xff;
    out[1] = in[1];
    out[2] = (in[2] ^ rk[1]);
    out[3] = in[3];
    out[4] = (in[4] - rk[2]) & 0xff;
    out[5] = in[5];
    out[6] = (in[6] ^ rk[3]);
    out[7] = in[7];
}

static void inverse_final_transformation(uint8_t* out, const uint8_t* in, const uint8_t* rk)
{
    out[7] = (in[6] ^ rk[7]);
    out[6] = (in[5]);
    out[5] = (in[4] - rk[6]) & 0xff;
    out[4] = (in[3]);
    out[3] = (in[2] ^ rk[5]);
    out[2] = (in[1]);
    out[1] = (in[0] - rk[4]) & 0xff;
    out[0] = (in[7]);
}

static void dec_round(uint8_t* out, const uint8_t* rk)
{
    uint8_t tmp0 = out[0];

    out[0] = out[1];
	out[1] = (out[2] - (F1(out[0]) ^ rk[0])) & 0xFF;
	out[2] = out[3];
	out[3] = out[4] ^ ((F0(out[2]) + rk[1]) & 0xFF);	
	out[4] = out[5];
	out[5] = (out[6] - (F1(out[4]) ^ rk[2])) & 0xFF;
	out[6] = out[7];	
	out[7] = tmp0 ^ ((F0(out[6]) + rk[3]) & 0xFF);
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
            rks[16 * i + j    ] = (mk[index    ] + delta[16 * i + j    ]) & 0xff;
            rks[16 * i + j + 8] = (mk[index + 8] + delta[16 * i + j + 8]) & 0xff;
        }
    }
}

//=============================================================================
// encryption
//=============================================================================
void hight_encrypt(uint8_t* out, const uint8_t* in, const uint8_t* rks)
{
    uint8_t round;
    const uint8_t* wk = rks;
    const uint8_t* rk = rks + 8;
    uint8_t block[8] = {0,};

    initial_transformation(block, in, wk);
    
    for (round = 0; round < HIGHT_ROUNDS; ++round) {
        enc_round(block, rk);
        rk += 4;
    }

    final_transformation(out, block, wk);
}

//=============================================================================
// decryption
//=============================================================================
void hight_decrypt(uint8_t* out, const uint8_t* in, const uint8_t* rks)
{
    uint8_t round;
    const uint8_t* wk = rks;
    const uint8_t* rk = rks + 132;
    uint8_t block[8] = {0,};

    inverse_final_transformation(block, in, wk);

    for (round = 0; round < HIGHT_ROUNDS; ++round)
    {
        dec_round(block, rk);
        rk -= 4;
    }

    inverse_initial_transformation(out, block, wk);
}