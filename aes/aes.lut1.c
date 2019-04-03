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

#include <string.h>
#include "aes.h"
#include "gf256.h"

static const uint8_t SBOX[] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16,
};

static const uint8_t SINV[] = {
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
    0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
    0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
    0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
    0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
    0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
    0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
    0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
    0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
    0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
    0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d,
};

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
    
    ptr[0] = SBOX[ptr[0]];
    ptr[1] = SBOX[ptr[1]];
    ptr[2] = SBOX[ptr[2]];
    ptr[3] = SBOX[ptr[3]];

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
        block[i] = SBOX[block[i]];
    }
}

static void inv_sub_bytes(uint8_t* block) 
{
    for (int i = 0; i < 16; ++i) {
        block[i] = SINV[block[i]];
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

static void aes_encrypt(uint8_t* ct, const uint8_t* pt, const uint8_t* rks, size_t rounds)
{
    uint8_t block[16] = {0};
    memcpy(block, pt, 16);
    transpose(block);

    add_round_keys(block, rks);
    rks += 16;

    for (int i = 0; i < rounds - 1; ++i, rks += 16)
    {
        sub_bytes(block);
        shift_rows(block);
        mix_columns(block);
        add_round_keys(block, rks);
    }

    sub_bytes(block);
    shift_rows(block);
    add_round_keys(block, rks);

    transpose(block);
    memcpy(ct, block, 16);
}

static void aes_decrypt(uint8_t* pt, const uint8_t* ct, const uint8_t* rks, size_t rounds)
{
    uint8_t block[16] = {0};
    memcpy(block, ct, 16);
    transpose(block);

    rks += 16 * rounds;

    add_round_keys(block, rks);
    rks -= 16;

    for (int i = 0; i < rounds - 1; ++i, rks -= 16)
    {
        inv_shift_rows(block);
        inv_sub_bytes(block);
        add_round_keys(block, rks);
        inv_mix_columns(block);
    }

    inv_sub_bytes(block);
    inv_shift_rows(block);
    add_round_keys(block, rks);

    transpose(block);
    memcpy(pt, block, 16);
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

void aes128_encrypt(uint8_t* ct, const uint8_t* pt, const uint8_t* rks)
{
    aes_encrypt(ct, pt, rks, AES128_ROUNDS);
}

void aes128_decrypt(uint8_t* pt, const uint8_t* ct, const uint8_t* rks)
{
    aes_decrypt(pt, ct, rks, AES128_ROUNDS);
}

void aes192_keygen(uint8_t* rks, const uint8_t* mk)
{
    const uint32_t* key = (const uint32_t*) mk;
    uint32_t* rk = (uint32_t*) rks;

    memcpy(rk, key, 24);

    for (int i = 0; i < 8; ++i) {
        rk[6] = rk[0] ^ sub_word(rot32r8(rk[5])) ^ RC[i];
        rk[7] = rk[1] ^ rk[6];
        rk[8] = rk[2] ^ rk[7];
        rk[9] = rk[3] ^ rk[8];
        rk[10] = rk[4] ^ rk[9];
        rk[11] = rk[5] ^ rk[10];

        rk += 6;
    }
}

void aes192_encrypt(uint8_t* ct, const uint8_t* pt, const uint8_t* rks)
{
    aes_encrypt(ct, pt, rks, AES192_ROUNDS);
}

void aes192_decrypt(uint8_t* pt, const uint8_t* ct, const uint8_t* rks)
{
    aes_decrypt(pt, ct, rks, AES192_ROUNDS);
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

void aes256_encrypt(uint8_t* ct, const uint8_t* pt, const uint8_t* rks)
{
    aes_encrypt(ct, pt, rks, AES256_ROUNDS);
}

void aes256_decrypt(uint8_t* pt, const uint8_t* ct, const uint8_t* rks)
{
    aes_decrypt(pt, ct, rks, AES256_ROUNDS);
}