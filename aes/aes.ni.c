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

#include "aes.h"
#include <wmmintrin.h>

/******************************************************************************
 * AES common functions
 *****************************************************************************/
static __m128i aes_keyexp1(__m128i k0, __m128i k1){
    k1 = _mm_shuffle_epi32(k1, _MM_SHUFFLE(3, 3, 3, 3));

    k0 = _mm_xor_si128(k0, _mm_slli_si128(k0, 4));
    k0 = _mm_xor_si128(k0, _mm_slli_si128(k0, 4));
    k0 = _mm_xor_si128(k0, _mm_slli_si128(k0, 4));

    return _mm_xor_si128(k0, k1);
}

static __m128i aes_keyexp2(__m128i k0, __m128i k1){
    k1 = _mm_shuffle_epi32(k1, _MM_SHUFFLE(2, 2, 2, 2));

    k0 = _mm_xor_si128(k0, _mm_slli_si128(k0, 4));
    k0 = _mm_xor_si128(k0, _mm_slli_si128(k0, 4));
    k0 = _mm_xor_si128(k0, _mm_slli_si128(k0, 4));

    return _mm_xor_si128(k0, k1);
}

static inline void aesni_encrypt(uint8_t *dst, const uint8_t *src, const __m128i *rks, size_t numRounds)
{
    int round = 0;
    __m128i blk = _mm_loadu_si128((__m128i *) src);

    blk = _mm_xor_si128(blk, rks[round]);

    for (round = 1; round < numRounds; ++round) {
        blk = _mm_aesenc_si128(blk, rks[round]);
    }
        
    blk = _mm_aesenclast_si128(blk, rks[round]);

    _mm_storeu_si128((__m128i *) dst, blk);
}

static inline void aesni_decrypt(uint8_t *dst, const uint8_t *src, const __m128i *rks, size_t numRounds)
{
    int round = numRounds;
    __m128i blk = _mm_loadu_si128((__m128i *) src);

    blk = _mm_xor_si128(blk, rks[round]);
    
    for (round = numRounds - 1; round > 0; --round) {
        blk = _mm_aesdec_si128(blk, _mm_aesimc_si128(rks[round]));
    }

    blk = _mm_aesdeclast_si128(blk, rks[round]);

    _mm_storeu_si128((__m128i *) dst, blk);
}

/******************************************************************************
 * AES 128 bit key
 *****************************************************************************/
void aes128_keygen(uint8_t* rk, const uint8_t* mk)
{
    __m128i* rks = (__m128i*) rk;

    rks[0]  = _mm_loadu_si128((const __m128i*) mk);
    rks[1]  = aes_keyexp1(rks[0], _mm_aeskeygenassist_si128(rks[0], 0x01));
    rks[2]  = aes_keyexp1(rks[1], _mm_aeskeygenassist_si128(rks[1], 0x02));
    rks[3]  = aes_keyexp1(rks[2], _mm_aeskeygenassist_si128(rks[2], 0x04));
    rks[4]  = aes_keyexp1(rks[3], _mm_aeskeygenassist_si128(rks[3], 0x08));
    rks[5]  = aes_keyexp1(rks[4], _mm_aeskeygenassist_si128(rks[4], 0x10));
    rks[6]  = aes_keyexp1(rks[5], _mm_aeskeygenassist_si128(rks[5], 0x20));
    rks[7]  = aes_keyexp1(rks[6], _mm_aeskeygenassist_si128(rks[6], 0x40));
    rks[8]  = aes_keyexp1(rks[7], _mm_aeskeygenassist_si128(rks[7], 0x80));
    rks[9]  = aes_keyexp1(rks[8], _mm_aeskeygenassist_si128(rks[8], 0x1b));
    rks[10]  = aes_keyexp1(rks[9], _mm_aeskeygenassist_si128(rks[9], 0x36));
}

void aes128_encrypt(uint8_t *dst, const uint8_t *src, const uint8_t *rks)
{
    aesni_encrypt(dst, src, (__m128i*) rks, AES128_ROUNDS);
}

void aes128_decrypt(uint8_t *dst, const uint8_t *src, const uint8_t *rks)
{
    aesni_decrypt(dst, src, (__m128i*) rks, AES128_ROUNDS);
}

/******************************************************************************
 * AES 192 bit key
 *****************************************************************************/
static void aes192_keyexp(__m128i* pk1, __m128i* pk2, __m128i k2_rcon, uint32_t* out)
{
    __m128i k1 = *pk1;
    __m128i k2 = *pk2;
    k2_rcon = _mm_shuffle_epi32(k2_rcon, _MM_SHUFFLE(1, 1, 1, 1));
    k1 = _mm_xor_si128(k1, _mm_slli_si128(k1, 4));
    k1 = _mm_xor_si128(k1, _mm_slli_si128(k1, 4));
    k1 = _mm_xor_si128(k1, _mm_slli_si128(k1, 4));
    k1 = _mm_xor_si128(k1, k2_rcon);
    
    *pk1 = k1;
    _mm_storeu_si128((__m128i*)out, k1);
    
    k2 = _mm_xor_si128(k2, _mm_slli_si128(k2, 4));
    k2 = _mm_xor_si128(k2, _mm_shuffle_epi32(k1, _MM_SHUFFLE(3, 3, 3, 3)));

    *pk2 = k2;
    out[4] = _mm_cvtsi128_si32(k2);
    out[5] = _mm_cvtsi128_si32(_mm_srli_si128(k2, 4));
}

static void aes192_keyexp_final(__m128i* pk1, __m128i* pk2, __m128i k2_rcon, uint32_t* out)
{
    __m128i k1 = *pk1;
    __m128i k2 = *pk2;
    k2_rcon = _mm_shuffle_epi32(k2_rcon, _MM_SHUFFLE(1, 1, 1, 1));
    
    k1 = _mm_xor_si128(k1, _mm_slli_si128(k1, 4));
    k1 = _mm_xor_si128(k1, _mm_slli_si128(k1, 4));
    k1 = _mm_xor_si128(k1, _mm_slli_si128(k1, 4));
    k1 = _mm_xor_si128(k1, k2_rcon);
    
    *pk1 = k1;
    _mm_storeu_si128((__m128i*)out, k1);
}

void aes192_keygen(uint8_t* rk, const uint8_t* mk)
{
    __m128i k1, k2;
    uint32_t* rks = (uint32_t*) rk;
    int i = 0;

    for (i = 0; i < 24; ++i) {
        rk[i] = mk[i];
    }

    k1 = _mm_loadu_si128((const __m128i*) mk);
    k2 = _mm_loadu_si128((const __m128i*) (mk + 16));

    aes192_keyexp(&k1, &k2, _mm_aeskeygenassist_si128(k2, 0x01), rks += 6);
    aes192_keyexp(&k1, &k2, _mm_aeskeygenassist_si128(k2, 0x02), rks += 6);
    aes192_keyexp(&k1, &k2, _mm_aeskeygenassist_si128(k2, 0x04), rks += 6);
    aes192_keyexp(&k1, &k2, _mm_aeskeygenassist_si128(k2, 0x08), rks += 6);
    aes192_keyexp(&k1, &k2, _mm_aeskeygenassist_si128(k2, 0x10), rks += 6);
    aes192_keyexp(&k1, &k2, _mm_aeskeygenassist_si128(k2, 0x20), rks += 6);
    aes192_keyexp(&k1, &k2, _mm_aeskeygenassist_si128(k2, 0x40), rks += 6);
    aes192_keyexp_final(&k1, &k2, _mm_aeskeygenassist_si128(k2, 0x80), rks += 6);

}

void aes192_encrypt(uint8_t* dst, const uint8_t* src, const uint8_t* rks)
{
    aesni_encrypt(dst, src, (__m128i*) rks, AES192_ROUNDS);
}

void aes192_decrypt(uint8_t* dst, const uint8_t* src, const uint8_t* rks)
{
    aesni_decrypt(dst, src, (__m128i*) rks, AES192_ROUNDS);
}

/******************************************************************************
 * AES 256 bit key
 *****************************************************************************/
void aes256_keygen(uint8_t* rk, const uint8_t* mk)
{
    __m128i* rks = (__m128i*) rk;

    rks[0] = _mm_loadu_si128((const __m128i*) mk);
    rks[1] = _mm_loadu_si128((const __m128i*) (mk+16));
    
    rks[2] = aes_keyexp1(rks[0], _mm_aeskeygenassist_si128(rks[1], 0x1));
    rks[3] = aes_keyexp2(rks[1], _mm_aeskeygenassist_si128(rks[2], 0x0));

    rks[4] = aes_keyexp1(rks[2], _mm_aeskeygenassist_si128(rks[3], 0x2));
    rks[5] = aes_keyexp2(rks[3], _mm_aeskeygenassist_si128(rks[4], 0x0));

    rks[6] = aes_keyexp1(rks[4], _mm_aeskeygenassist_si128(rks[5], 0x4));
    rks[7] = aes_keyexp2(rks[5], _mm_aeskeygenassist_si128(rks[6], 0x0));

    rks[8] = aes_keyexp1(rks[6], _mm_aeskeygenassist_si128(rks[7], 0x8));
    rks[9] = aes_keyexp2(rks[7], _mm_aeskeygenassist_si128(rks[8], 0x0));

    rks[10] = aes_keyexp1(rks[8], _mm_aeskeygenassist_si128(rks[9], 0x10));
    rks[11] = aes_keyexp2(rks[9], _mm_aeskeygenassist_si128(rks[10], 0x0));

    rks[12] = aes_keyexp1(rks[10], _mm_aeskeygenassist_si128(rks[11], 0x20));
    rks[13] = aes_keyexp2(rks[11], _mm_aeskeygenassist_si128(rks[12], 0x0));
    
    rks[14] = aes_keyexp1(rks[12], _mm_aeskeygenassist_si128(rks[13], 0x40));
}

void aes256_encrypt(uint8_t* dst, const uint8_t* src, const uint8_t* rks)
{
    aesni_encrypt(dst, src, (__m128i*) rks, AES256_ROUNDS);
}

void aes256_decrypt(uint8_t* dst, const uint8_t* src, const uint8_t* rks)
{
    aesni_decrypt(dst, src, (__m128i*) rks, AES256_ROUNDS);
}
