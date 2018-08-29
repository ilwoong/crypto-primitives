/**
 * MIT License
 * 
 * Copyright (c) 2018 Ilwoong Jeong, https://github.com/ilwoong
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "lsh.h"
#include <string.h>
#include <immintrin.h>

typedef struct st_lsh256_avx2_context {
    size_t bidx;
    size_t length;
    __attribute__ ((aligned(32))) __m256i block[4];
    __attribute__ ((aligned(32))) __m256i cv[2];
    __attribute__ ((aligned(32))) __m256i tcv[2];
    __attribute__ ((aligned(32))) __m256i msg[2 * (26 + 1)];
} lsh256_avx2_context;

const static size_t NUMSTEP = 26;

const static size_t BLOCKSIZE = 128;

const static __attribute__ ((aligned(32))) uint32_t IV[] = {
    0x46a10f1f, 0xfddce486, 0xb41443a8, 0x198e6b9d, 0x3304388d, 0xb0f5a3c7, 0xb36061c4, 0x7adbd553,
    0x105d5378, 0x2f74de54, 0x5c2f2d95, 0xf2553fbe, 0x8051357a, 0x138668c8, 0x47aa4484, 0xe01afb41
};

const static __attribute__ ((aligned(32))) uint32_t STEP_CONSTANT[] = {
    0x917caf90, 0x6c1b10a2, 0x6f352943, 0xcf778243, 0x2ceb7472, 0x29e96ff2, 0x8a9ba428, 0x2eeb2642, 
    0x0e2c4021, 0x872bb30e, 0xa45e6cb2, 0x46f9c612, 0x185fe69e, 0x1359621b, 0x263fccb2, 0x1a116870, 
    0x3a6c612f, 0xb2dec195, 0x02cb1f56, 0x40bfd858, 0x784684b6, 0x6cbb7d2e, 0x660c7ed8, 0x2b79d88a, 
    0xa6cd9069, 0x91a05747, 0xcdea7558, 0x00983098, 0xbecb3b2e, 0x2838ab9a, 0x728b573e, 0xa55262b5, 
    0x745dfa0f, 0x31f79ed8, 0xb85fce25, 0x98c8c898, 0x8a0669ec, 0x60e445c2, 0xfde295b0, 0xf7b5185a, 
    0xd2580983, 0x29967709, 0x182df3dd, 0x61916130, 0x90705676, 0x452a0822, 0xe07846ad, 0xaccd7351, 
    0x2a618d55, 0xc00d8032, 0x4621d0f5, 0xf2f29191, 0x00c6cd06, 0x6f322a67, 0x58bef48d, 0x7a40c4fd, 
    0x8beee27f, 0xcd8db2f2, 0x67f2c63b, 0xe5842383, 0xc793d306, 0xa15c91d6, 0x17b381e5, 0xbb05c277, 
    0x7ad1620a, 0x5b40a5bf, 0x5ab901a2, 0x69a7a768, 0x5b66d9cd, 0xfdee6877, 0xcb3566fc, 0xc0c83a32, 
    0x4c336c84, 0x9be6651a, 0x13baa3fc, 0x114f0fd1, 0xc240a728, 0xec56e074, 0x009c63c7, 0x89026cf2, 
    0x7f9ff0d0, 0x824b7fb5, 0xce5ea00f, 0x605ee0e2, 0x02e7cfea, 0x43375560, 0x9d002ac7, 0x8b6f5f7b, 
    0x1f90c14f, 0xcdcb3537, 0x2cfeafdd, 0xbf3fc342, 0xeab7b9ec, 0x7a8cb5a3, 0x9d2af264, 0xfacedb06, 
    0xb052106e, 0x99006d04, 0x2bae8d09, 0xff030601, 0xa271a6d6, 0x0742591d, 0xc81d5701, 0xc9a9e200, 
    0x02627f1e, 0x996d719d, 0xda3b9634, 0x02090800, 0x14187d78, 0x499b7624, 0xe57458c9, 0x738be2c9, 
    0x64e19d20, 0x06df0f36, 0x15d1cb0e, 0x0b110802, 0x2c95f58c, 0xe5119a6d, 0x59cd22ae, 0xff6eac3c, 
    0x467ebd84, 0xe5ee453c, 0xe79cd923, 0x1c190a0d, 0xc28b81b8, 0xf6ac0852, 0x26efd107, 0x6e1ae93b, 
    0xc53c41ca, 0xd4338221, 0x8475fd0a, 0x35231729, 0x4e0d3a7a, 0xa2b45b48, 0x16c0d82d, 0x890424a9, 
    0x017e0c8f, 0x07b5a3f5, 0xfa73078e, 0x583a405e, 0x5b47b4c8, 0x570fa3ea, 0xd7990543, 0x8d28ce32, 
    0x7f8a9b90, 0xbd5998fc, 0x6d7a9688, 0x927a9eb6, 0xa2fc7d23, 0x66b38e41, 0x709e491a, 0xb5f700bf, 
    0x0a262c0f, 0x16f295b9, 0xe8111ef5, 0x0d195548, 0x9f79a0c5, 0x1a41cfa7, 0x0ee7638a, 0xacf7c074, 
    0x30523b19, 0x09884ecf, 0xf93014dd, 0x266e9d55, 0x191a6664, 0x5c1176c1, 0xf64aed98, 0xa4b83520, 
    0x828d5449, 0x91d71dd8, 0x2944f2d6, 0x950bf27b, 0x3380ca7d, 0x6d88381d, 0x4138868e, 0x5ced55c4, 
    0x0fe19dcb, 0x68f4f669, 0x6e37c8ff, 0xa0fe6e10, 0xb44b47b0, 0xf5c0558a, 0x79bf14cf, 0x4a431a20, 
    0xf17f68da, 0x5deb5fd1, 0xa600c86d, 0x9f6c7eb0, 0xff92f864, 0xb615e07f, 0x38d3e448, 0x8d5d3a6a, 
    0x70e843cb, 0x494b312e, 0xa6c93613, 0x0beb2f4f, 0x928b5d63, 0xcbf66035, 0x0cb82c80, 0xea97a4f7, 
    0x592c0f3b, 0x947c5f77, 0x6fff49b9, 0xf71a7e5a, 0x1de8c0f5, 0xc2569600, 0xc4e4ac8c, 0x823c9ce1
};
	
const static uint32_t ALPHA_EVEN = 29;
const static uint32_t ALPHA_ODD = 5;
	
const static uint32_t BETA_EVEN = 1;
const static uint32_t BETA_ODD= 17;

const static __attribute__ ((aligned(32))) uint32_t GAMMA[] = {
    0x03020100, 0x06050407, 0x09080b0a, 0x0c0f0e0d, 
    0x10131211, 0x15141716, 0x1a19181b, 0x1f1e1d1c,
};

const static __attribute__ ((aligned(32))) uint32_t MSGEXP_SHUFFLE[] = {
    0x0f0e0d0c, 0x0b0a0908, 0x03020100, 0x07060504,    
    0x1f1e1d1c, 0x13121110, 0x17161514, 0x1b1a1918,
};

static inline __m256i rol32(__m256i value, size_t rot)
{
    return _mm256_slli_epi32(value, rot) | _mm256_srli_epi32(value, (32 - rot));
}

static void expand_message(lsh256_avx2_context* ctx, const uint8_t* in)
{
    __m256i* msg = ctx->msg;    
    msg[0] = _mm256_loadu_si256((__m256i*) in);
    msg[1] = _mm256_loadu_si256((__m256i*) in + 1);
    msg[2] = _mm256_loadu_si256((__m256i*) in + 2);
    msg[3] = _mm256_loadu_si256((__m256i*) in + 3);

    __m256i ctrl = _mm256_loadu_si256((__m256i*) MSGEXP_SHUFFLE);

    for (size_t i = 2; i <= NUMSTEP; ++i) {
        size_t idx = 2 * i;

        msg[idx] = _mm256_add_epi32(
            msg[idx - 2], _mm256_shuffle_epi8(msg[idx - 4], ctrl)
        );
        
        msg[idx + 1] = _mm256_add_epi32(
            msg[idx - 1], _mm256_shuffle_epi8(msg[idx - 3], ctrl)
        );
    }
}

static inline void permute_word(lsh256_avx2_context* ctx)
{
    ctx->tcv[0] = _mm256_shuffle_epi32(ctx->tcv[0], _MM_SHUFFLE(3,1,0,2));
	ctx->tcv[1] = _mm256_shuffle_epi32(ctx->tcv[1], _MM_SHUFFLE(1,2,3,0));
    ctx->cv[0] = _mm256_permute2x128_si256(ctx->tcv[0], ctx->tcv[1], 0x31);
    ctx->cv[1] = _mm256_permute2x128_si256(ctx->tcv[0], ctx->tcv[1], 0x20);
}

static void step(lsh256_avx2_context* ctx, size_t idx, uint32_t alpha, uint32_t beta)
{
    __m256i vl, vr;
    __m256i step_constant, gamma;

    step_constant = _mm256_loadu_si256((const __m256i*) STEP_CONSTANT + idx);
    gamma = _mm256_loadu_si256((const __m256i*) GAMMA);

    vl = ctx->cv[0] ^ ctx->msg[2 * idx    ];
    vr = ctx->cv[1] ^ ctx->msg[2 * idx + 1];

    vl = rol32(_mm256_add_epi32(vl, vr), alpha) ^ step_constant;
    vr = rol32(_mm256_add_epi32(vl, vr), beta);
    
    ctx->tcv[0] = _mm256_add_epi32(vl, vr);
    ctx->tcv[1] = _mm256_shuffle_epi8(vr, gamma);

    permute_word(ctx);
}

static void compress(lsh256_avx2_context* ctx, const uint8_t* data)
{
    expand_message(ctx, data);

    for (size_t i = 0; i < NUMSTEP; i += 2) {
        step(ctx, i, ALPHA_EVEN, BETA_EVEN);
        step(ctx, i + 1, ALPHA_ODD, BETA_ODD);
    }

    for (size_t i = 0; i < 2; ++i) {
        ctx->cv[i] ^= ctx->msg[2 * NUMSTEP + i];
    }
}

void lsh256_init(lsh256_context* ctx)
{
    ctx->bidx = 0;
    ctx->length = 0;
    memset(ctx->block, 0, BLOCKSIZE);

    memcpy(ctx->cv, IV, 16 * sizeof(uint32_t));
    memset(ctx->tcv, 0, 16 * sizeof(uint32_t));
    memset(ctx->msg, 0, 16 * (NUMSTEP + 1) * sizeof(uint32_t));
}

void lsh256_update(lsh256_context* ctx, const uint8_t* data, size_t length)
{
    ctx->length += length;

    if (ctx->bidx > 0) {
        size_t gap = BLOCKSIZE - ctx->bidx;

        if (length >= gap) {
            memcpy(ctx->block + ctx->bidx, data, gap);
            compress((lsh256_avx2_context*)ctx, ctx->block);
            ctx->bidx = 0;
            data += gap;
            length -= gap;

        } else {
            memcpy(ctx->block + ctx->bidx, data, length);
            ctx->bidx += length;
            data += length;
            length = 0;
        }
    }
    
    while (length >= BLOCKSIZE) {
        compress((lsh256_avx2_context*)ctx, data);
        data += BLOCKSIZE;
        length -= BLOCKSIZE;
    }

    if (length > 0) {
        memcpy(ctx->block + ctx->bidx, data, length);
        ctx->bidx += length;
    }
}

void lsh256_final(lsh256_context* ctx, uint8_t* digest)
{
    uint32_t* result = (uint32_t*) digest;

    ctx->block[(ctx->bidx)++] = (uint8_t) 0x80;
    memset(ctx->block + ctx->bidx, 0, BLOCKSIZE - ctx->bidx);
    compress((lsh256_avx2_context*)ctx, ctx->block);

    for (size_t i = 0; i < 8; ++i) {
        result[i] = ctx->cv[i] ^ ctx->cv[i + 8];
    }

    lsh256_init(ctx);
}