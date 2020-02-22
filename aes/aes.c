/**
 * The MIT License
 *
 * Copyright (c) 2020 Ilwoong Jeong (https://github.com/ilwoong)
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

#include <string.h>
#include "aes.h"
#include "sbox.h"
#include "gf256.h"

static const uint32_t RC[] = {
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36,
};

static inline uint32_t rot32r8(uint32_t value)
{
    return (value >> 8) ^ (value << 24);
}

static uint32_t sub_word(uint32_t value)
{
    uint8_t* ptr = (uint8_t*) &value;
    
    ptr[0] = affine_sbox(ptr[0]);
    ptr[1] = affine_sbox(ptr[1]);
    ptr[2] = affine_sbox(ptr[2]);
    ptr[3] = affine_sbox(ptr[3]);

    return value;
}

static void add_round_keys(uint8_t* block, const uint8_t* rks) {
   block[0] ^= rks[0];
   block[1] ^= rks[4];
   block[2] ^= rks[8];
   block[3] ^= rks[12];

   block[4] ^= rks[1];
   block[5] ^= rks[5];
   block[6] ^= rks[9];
   block[7] ^= rks[13];

   block[8] ^= rks[2];
   block[9] ^= rks[6];
   block[10] ^= rks[10];
   block[11] ^= rks[14];

   block[12] ^= rks[3];
   block[13] ^= rks[7];
   block[14] ^= rks[11];
   block[15] ^= rks[15];
}

static void swap(uint8_t* block, size_t pos1, size_t pos2)
{
    uint8_t tmp = block[pos1];
    block[pos1] = block[pos2];
    block[pos2] = tmp;
}

static void transpose(uint8_t* block)
{
    swap(block, 1, 4);
    swap(block, 2, 8);
    swap(block, 3, 12);
    swap(block, 6, 9);
    swap(block, 7, 13);
    swap(block, 11, 14);
}

static void sub_bytes(uint8_t* block)
{
    for (int i = 0; i < 16; ++i) {
        block[i] = affine_sbox(block[i]);
    }
}

static void inv_sub_bytes(uint8_t* block) 
{
    for (int i = 0; i < 16; ++i) {
        block[i] = affine_sinv(block[i]);
    }
}

static void shift_rows(uint8_t* block)
{
    uint8_t tmp = block[4];
    block[4] = block[5];
    block[5] = block[6];
    block[6] = block[7];
    block[7] = tmp;

    swap(block, 8, 10);
    swap(block, 9, 11);

    tmp = block[15];
    block[15] = block[14];
    block[14] = block[13];
    block[13] = block[12];
    block[12] = tmp;
}

static void inv_shift_rows(uint8_t* block)
{
    uint8_t tmp = block[7];
    block[7] = block[6];
    block[6] = block[5];
    block[5] = block[4];
    block[4] = tmp;

    swap(block, 8, 10);
    swap(block, 9, 11);

    tmp = block[12];
    block[12] = block[13];
    block[13] = block[14];
    block[14] = block[15];
    block[15] = tmp;
}

static void mix_columns(uint8_t* in)
{
    uint8_t block[16] = {0};

    for (int i = 0; i < 4; ++i) {
        block[i     ] = gf256_mul(in[i], 2) ^ gf256_mul(in[i + 4], 3) ^ gf256_mul(in[i + 8], 1) ^ gf256_mul(in[i + 12], 1);
        block[i +  4] = gf256_mul(in[i], 1) ^ gf256_mul(in[i + 4], 2) ^ gf256_mul(in[i + 8], 3) ^ gf256_mul(in[i + 12], 1);
        block[i +  8] = gf256_mul(in[i], 1) ^ gf256_mul(in[i + 4], 1) ^ gf256_mul(in[i + 8], 2) ^ gf256_mul(in[i + 12], 3);
        block[i + 12] = gf256_mul(in[i], 3) ^ gf256_mul(in[i + 4], 1) ^ gf256_mul(in[i + 8], 1) ^ gf256_mul(in[i + 12], 2);
    }

    memcpy(in, block, 16);
}

static void inv_mix_columns(uint8_t* in)
{
    uint8_t block[16] = {0};

    for (int i = 0; i < 4; ++i) {
        block[i     ] = gf256_mul(in[i], 0xe) ^ gf256_mul(in[i + 4], 0xb) ^ gf256_mul(in[i + 8], 0xd) ^ gf256_mul(in[i + 12], 0x9);
        block[i +  4] = gf256_mul(in[i], 0x9) ^ gf256_mul(in[i + 4], 0xe) ^ gf256_mul(in[i + 8], 0xb) ^ gf256_mul(in[i + 12], 0xd);
        block[i +  8] = gf256_mul(in[i], 0xd) ^ gf256_mul(in[i + 4], 0x9) ^ gf256_mul(in[i + 8], 0xe) ^ gf256_mul(in[i + 12], 0xb);
        block[i + 12] = gf256_mul(in[i], 0xb) ^ gf256_mul(in[i + 4], 0xd) ^ gf256_mul(in[i + 8], 0x9) ^ gf256_mul(in[i + 12], 0xe);
    }

    memcpy(in, block, 16);
}

static inline void encrypt_round(uint8_t* block, const uint8_t* rk)
{
    sub_bytes(block);
    shift_rows(block);
    mix_columns(block);
    add_round_keys(block, rk);
}

static inline void encrypt_last_round(uint8_t* block, const uint8_t* rk)
{
    sub_bytes(block);
    shift_rows(block);
    add_round_keys(block, rk);
}

static void aes_encrypt(uint8_t* dst, const uint8_t* src, const uint8_t* rks, size_t rounds)
{
    uint8_t block[16] = {0};
    memcpy(block, src, 16);
    transpose(block);

    add_round_keys(block, rks);
    rks += 16;

    for (int i = 0; i < rounds - 1; ++i, rks += 16)
    {
        encrypt_round(block, rks);
    }

    encrypt_last_round(block, rks);

    transpose(block);
    memcpy(dst, block, 16);
}

static inline void decrypt_round(uint8_t* block, const uint8_t* rk)
{
    inv_shift_rows(block);
    inv_sub_bytes(block);
    add_round_keys(block, rk);
    inv_mix_columns(block);
}

static inline void decrypt_last_round(uint8_t* block, const uint8_t* rk)
{
    inv_sub_bytes(block);
    inv_shift_rows(block);
    add_round_keys(block, rk);
}

static void aes_decrypt(uint8_t* dst, const uint8_t* src, const uint8_t* rks, size_t rounds)
{
    uint8_t block[16] = {0};
    memcpy(block, src, 16);
    transpose(block);

    rks += 16 * rounds;

    add_round_keys(block, rks);
    rks -= 16;

    for (int i = 0; i < rounds - 1; ++i, rks -= 16)
    {
        decrypt_round(block, rks);
    }

    decrypt_last_round(block, rks);

    transpose(block);
    memcpy(dst, block, 16);
}

void aes128_keygen(uint8_t* rks, const uint8_t* mk)
{
    const uint32_t* key = (const uint32_t*) mk;
    uint32_t* rk = (uint32_t*) rks;

    memcpy(rk, key, 16);

    for (int i = 0; i < 10; ++i) {
        rk[4] = rk[0] ^ sub_word(rot32r8(rk[3])) ^ RC[i];
        rk[5] = rk[1] ^ rk[4];
        rk[6] = rk[2] ^ rk[5];
        rk[7] = rk[3] ^ rk[6];
        rk += 4;
    }
}

void aes128_encrypt(uint8_t* dst, const uint8_t* src, const uint8_t* rks)
{
    aes_encrypt(dst, src, rks, AES128_ROUNDS);
}

void aes128_decrypt(uint8_t* dst, const uint8_t* src, const uint8_t* rks)
{
    aes_decrypt(dst, src, rks, AES128_ROUNDS);
}

void aes192_keygen(uint8_t* rks, const uint8_t* mk)
{
    const uint32_t* key = (const uint32_t*) mk;
    uint32_t* rk = (uint32_t*) rks;

    memcpy(rk, key, 24);

    for (int i = 0; i < 7; ++i) {
        rk[6] = rk[0] ^ sub_word(rot32r8(rk[5])) ^ RC[i];
        rk[7] = rk[1] ^ rk[6];
        rk[8] = rk[2] ^ rk[7];
        rk[9] = rk[3] ^ rk[8];
        rk[10] = rk[4] ^ rk[9];
        rk[11] = rk[5] ^ rk[10];

        rk += 6;
    }

    rk[6] = rk[0] ^ sub_word(rot32r8(rk[5])) ^ RC[7];
    rk[7] = rk[1] ^ rk[6];
    rk[8] = rk[2] ^ rk[7];
    rk[9] = rk[3] ^ rk[8];
}

void aes192_encrypt(uint8_t* dst, const uint8_t* src, const uint8_t* rks)
{
    aes_encrypt(dst, src, rks, AES192_ROUNDS);
}

void aes192_decrypt(uint8_t* dst, const uint8_t* src, const uint8_t* rks)
{
    aes_decrypt(dst, src, rks, AES192_ROUNDS);
}

void aes256_keygen(uint8_t* rks, const uint8_t* mk)
{
    const uint32_t* key = (const uint32_t*) mk;
    uint32_t* rk = (uint32_t*) rks;

    memcpy(rk, key, 32);

    for (int i = 0; i < 7; ++i) {
        rk[8] = rk[0] ^ sub_word(rot32r8(rk[7])) ^ RC[i];
        rk[9] = rk[1] ^ rk[8];
        rk[10] = rk[2] ^ rk[9];
        rk[11] = rk[3] ^ rk[10];

        if (i == 6) {
            break;
        }

        rk[12] = rk[4] ^ sub_word(rk[11]);
        rk[13] = rk[5] ^ rk[12];
        rk[14] = rk[6] ^ rk[13];
        rk[15] = rk[7] ^ rk[14];
        
        rk += 8;
    }
}

void aes256_encrypt(uint8_t* dst, const uint8_t* src, const uint8_t* rks)
{
    aes_encrypt(dst, src, rks, AES256_ROUNDS);
}

void aes256_decrypt(uint8_t* dst, const uint8_t* src, const uint8_t* rks)
{
    aes_decrypt(dst, src, rks, AES256_ROUNDS);
}