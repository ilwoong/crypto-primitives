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

#include "lea.avx2.h"

#include <x86intrin.h>

#include "inline.inc"

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//! avx2 aided basic operations
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
static FORCE_INLINE __m256i rol32x8(__m256i value, size_t rot)
{
    return _mm256_xor_si256(_mm256_srli_epi32(value, 32 - rot), _mm256_slli_epi32(value, rot));
}

static FORCE_INLINE __m256i ror32x8(__m256i value, size_t rot)
{
    return _mm256_xor_si256(_mm256_srli_epi32(value, rot), _mm256_slli_epi32(value, 32 - rot));
}

static FORCE_INLINE __m256i add32x8(__m256i lhs, __m256i rhs)
{
    return _mm256_add_epi32(lhs, rhs);
}

static FORCE_INLINE __m256i sub32x8(__m256i lhs, __m256i rhs)
{
    return _mm256_sub_epi32(lhs, rhs);
}

static FORCE_INLINE __m256i addKey(__m256i value, uint32_t rk) 
{
    return _mm256_xor_si256(value, _mm256_set1_epi32(rk));
}

static FORCE_INLINE __m256i load32x8(const uint32_t* in)
{
    return _mm256_setr_epi32(*in, *(in + 4), *(in + 8), *(in + 12), *(in + 16), *(in + 20), *(in + 24), *(in + 28));
}

static FORCE_INLINE void unload32x8(uint32_t* out, const __m256i value)
{
    out[ 0] = _mm256_extract_epi32(value, 0);
    out[ 4] = _mm256_extract_epi32(value, 1);
    out[ 8] = _mm256_extract_epi32(value, 2);
    out[12] = _mm256_extract_epi32(value, 3);
    out[16] = _mm256_extract_epi32(value, 4);
    out[20] = _mm256_extract_epi32(value, 5);
    out[24] = _mm256_extract_epi32(value, 6);
    out[28] = _mm256_extract_epi32(value, 7);
}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//! common round functions
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
static FORCE_INLINE void enc_round_8blk(__m256i* x0, __m256i* x1, __m256i* x2, __m256i* x3, const uint32_t* rk)
{
    *x3 = ror32x8(add32x8(addKey(*x2, rk[4]), addKey(*x3, rk[5])), 3);
    *x2 = ror32x8(add32x8(addKey(*x1, rk[2]), addKey(*x2, rk[3])), 5);
    *x1 = rol32x8(add32x8(addKey(*x0, rk[0]), addKey(*x1, rk[1])), 9);
}

static FORCE_INLINE void dec_round_8blk(__m256i* x0, __m256i* x1, __m256i* x2, __m256i* x3, const uint32_t* rk)
{
    *x0 = addKey(sub32x8(ror32x8(*x0, 9), addKey(*x3, rk[0])), rk[1]);
    *x1 = addKey(sub32x8(rol32x8(*x1, 5), addKey(*x0, rk[2])), rk[3]);
    *x2 = addKey(sub32x8(rol32x8(*x2, 3), addKey(*x1, rk[4])), rk[5]);
}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//! common encryption functions
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
static FORCE_INLINE void lea_encrypt_8blk(uint8_t* out, const uint8_t* in, const uint8_t* rks, size_t rounds)
{
    const uint32_t* rk = (const uint32_t*) rks;

    __m256i x0 = load32x8((const uint32_t*) in);
    __m256i x1 = load32x8((const uint32_t*) in + 1);
    __m256i x2 = load32x8((const uint32_t*) in + 2);
    __m256i x3 = load32x8((const uint32_t*) in + 3);
    
    for (size_t round = 0; round < rounds; round += 4)
    {
        enc_round_8blk(&x0, &x1, &x2, &x3, rk);
        rk += 6;

        enc_round_8blk(&x1, &x2, &x3, &x0, rk);        
        rk += 6;

        enc_round_8blk(&x2, &x3, &x0, &x1, rk);
        rk += 6;

        enc_round_8blk(&x3, &x0, &x1, &x2, rk);
        rk += 6;
    }

    unload32x8((uint32_t*)out    , x0);
    unload32x8((uint32_t*)out + 1, x1);
    unload32x8((uint32_t*)out + 2, x2);
    unload32x8((uint32_t*)out + 3, x3);
}

static FORCE_INLINE void lea_encrypt_16blk(uint8_t* out, const uint8_t* in, const uint8_t* rks, size_t rounds)
{
    const uint32_t* rk = (const uint32_t*) rks;

    __m256i x0 = load32x8((const uint32_t*) in);
    __m256i x1 = load32x8((const uint32_t*) in + 1);
    __m256i x2 = load32x8((const uint32_t*) in + 2);
    __m256i x3 = load32x8((const uint32_t*) in + 3);
    __m256i x4 = load32x8((const uint32_t*) in + 32);
    __m256i x5 = load32x8((const uint32_t*) in + 33);
    __m256i x6 = load32x8((const uint32_t*) in + 34);
    __m256i x7 = load32x8((const uint32_t*) in + 35);
    
    for (size_t round = 0; round < rounds; round += 4)
    {
        enc_round_8blk(&x0, &x1, &x2, &x3, rk);
        enc_round_8blk(&x4, &x5, &x6, &x7, rk);
        rk += 6;

        enc_round_8blk(&x1, &x2, &x3, &x0, rk);
        enc_round_8blk(&x5, &x6, &x7, &x4, rk);
        rk += 6;

        enc_round_8blk(&x2, &x3, &x0, &x1, rk);
        enc_round_8blk(&x6, &x7, &x4, &x5, rk);
        rk += 6;

        enc_round_8blk(&x3, &x0, &x1, &x2, rk);
        enc_round_8blk(&x7, &x4, &x5, &x6, rk);
        rk += 6;
    }

    unload32x8((uint32_t*)out     , x0);
    unload32x8((uint32_t*)out +  1, x1);
    unload32x8((uint32_t*)out +  2, x2);
    unload32x8((uint32_t*)out +  3, x3);
    unload32x8((uint32_t*)out + 32, x4);
    unload32x8((uint32_t*)out + 33, x5);
    unload32x8((uint32_t*)out + 34, x6);
    unload32x8((uint32_t*)out + 35, x7);
}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//! common decryption functions
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
static FORCE_INLINE void lea_decrypt_8blk(uint8_t* out, const uint8_t* in, const uint8_t* rks, size_t rounds)
{
    const uint32_t* rk = (const uint32_t*) rks;

    __m256i x0 = load32x8((const uint32_t*) in);
    __m256i x1 = load32x8((const uint32_t*) in + 1);
    __m256i x2 = load32x8((const uint32_t*) in + 2);
    __m256i x3 = load32x8((const uint32_t*) in + 3);
    
    rk += 6 * (rounds - 1);
    for (size_t round = 0; round < rounds; round += 4)
    {
        dec_round_8blk(&x0, &x1, &x2, &x3, rk);
        rk -= 6;

        dec_round_8blk(&x3, &x0, &x1, &x2, rk);
        rk -= 6;

        dec_round_8blk(&x2, &x3, &x0, &x1, rk);
        rk -= 6;

        dec_round_8blk(&x1, &x2, &x3, &x0, rk);
        rk -= 6;
    }

    unload32x8((uint32_t*)out    , x0);
    unload32x8((uint32_t*)out + 1, x1);
    unload32x8((uint32_t*)out + 2, x2);
    unload32x8((uint32_t*)out + 3, x3);
}

static FORCE_INLINE void lea_decrypt_16blk(uint8_t* out, const uint8_t* in, const uint8_t* rks, size_t rounds)
{
    const uint32_t* rk = (const uint32_t*) rks;

    __m256i x0 = load32x8((const uint32_t*) in);
    __m256i x1 = load32x8((const uint32_t*) in + 1);
    __m256i x2 = load32x8((const uint32_t*) in + 2);
    __m256i x3 = load32x8((const uint32_t*) in + 3);
    __m256i x4 = load32x8((const uint32_t*) in + 32);
    __m256i x5 = load32x8((const uint32_t*) in + 33);
    __m256i x6 = load32x8((const uint32_t*) in + 34);
    __m256i x7 = load32x8((const uint32_t*) in + 35);
    
    rk += 6 * (rounds - 1);
    for (size_t round = 0; round < rounds; round += 4)
    {
        dec_round_8blk(&x0, &x1, &x2, &x3, rk);
        dec_round_8blk(&x4, &x5, &x6, &x7, rk);
        rk -= 6;

        dec_round_8blk(&x3, &x0, &x1, &x2, rk);
        dec_round_8blk(&x7, &x4, &x5, &x6, rk);
        rk -= 6;

        dec_round_8blk(&x2, &x3, &x0, &x1, rk);
        dec_round_8blk(&x6, &x7, &x4, &x5, rk);
        rk -= 6;

        dec_round_8blk(&x1, &x2, &x3, &x0, rk);
        dec_round_8blk(&x5, &x6, &x7, &x4, rk);
        rk -= 6;
    }

    unload32x8((uint32_t*)out    , x0);
    unload32x8((uint32_t*)out + 1, x1);
    unload32x8((uint32_t*)out + 2, x2);
    unload32x8((uint32_t*)out + 3, x3);
    unload32x8((uint32_t*)out + 32, x4);
    unload32x8((uint32_t*)out + 33, x5);
    unload32x8((uint32_t*)out + 34, x6);
    unload32x8((uint32_t*)out + 35, x7);
}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//! encryption / decryption for 128-bit key
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
FORCE_INLINE void lea128_encrypt_8blk(uint8_t* out, const uint8_t* in, const uint8_t* rks)
{
    lea_encrypt_8blk(out, in, rks, LEA128_ROUNDS);
}

FORCE_INLINE void lea128_decrypt_8blk(uint8_t* out, const uint8_t* in, const uint8_t* rks)
{
    lea_decrypt_8blk(out, in, rks, LEA128_ROUNDS);
}

FORCE_INLINE void lea128_encrypt_16blk(uint8_t* out, const uint8_t* in, const uint8_t* rks)
{
    lea_encrypt_16blk(out, in, rks, LEA128_ROUNDS);
}

FORCE_INLINE void lea128_decrypt_16blk(uint8_t* out, const uint8_t* in, const uint8_t* rks)
{
    lea_decrypt_16blk(out, in, rks, LEA128_ROUNDS);
}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//! encryption / decryption for 192-bit key
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
FORCE_INLINE void lea192_encrypt_8blk(uint8_t* out, const uint8_t* in, const uint8_t* rks)
{
    lea_encrypt_8blk(out, in, rks, LEA192_ROUNDS);
}

FORCE_INLINE void lea192_decrypt_8blk(uint8_t* out, const uint8_t* in, const uint8_t* rks)
{
    lea_decrypt_8blk(out, in, rks, LEA192_ROUNDS);
}

FORCE_INLINE void lea192_encrypt_16blk(uint8_t* out, const uint8_t* in, const uint8_t* rks)
{
    lea_encrypt_16blk(out, in, rks, LEA192_ROUNDS);
}

FORCE_INLINE void lea192_decrypt_16blk(uint8_t* out, const uint8_t* in, const uint8_t* rks)
{
    lea_decrypt_16blk(out, in, rks, LEA192_ROUNDS);
}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//! encryption / decryption for 256-bit key
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
FORCE_INLINE void lea256_encrypt_8blk(uint8_t* out, const uint8_t* in, const uint8_t* rks)
{
    lea_encrypt_8blk(out, in, rks, LEA256_ROUNDS);
}

FORCE_INLINE void lea256_decrypt_8blk(uint8_t* out, const uint8_t* in, const uint8_t* rks)
{
    lea_decrypt_8blk(out, in, rks, LEA256_ROUNDS);
}

FORCE_INLINE void lea256_encrypt_16blk(uint8_t* out, const uint8_t* in, const uint8_t* rks)
{
    lea_encrypt_16blk(out, in, rks, LEA256_ROUNDS);
}

FORCE_INLINE void lea256_decrypt_16blk(uint8_t* out, const uint8_t* in, const uint8_t* rks)
{
    lea_decrypt_16blk(out, in, rks, LEA256_ROUNDS);
}
