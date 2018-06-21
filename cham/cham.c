/**
 * MIT License
 * 
 * Copyright (c) 2018 Ilwoong Jeong
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

#include "cham.h"

static inline uint16_t rol16(uint16_t value, size_t rot)
{
    return (value << rot) | (value >> (16 - rot));
}

static inline uint16_t ror16(uint16_t value, size_t rot)
{
    return (value >> rot) | (value << (16 - rot));
}

static inline uint32_t rol32(uint32_t value, size_t rot)
{
    return (value << rot) | (value >> (32 - rot));
}

static inline uint32_t ror32(uint32_t value, size_t rot)
{
    return (value >> rot) | (value << (32 - rot));
}

/**
 * CHAM 64-bit block, 128-bit key
 */ 
void cham64_keygen(uint8_t* rks, const uint8_t* mk)
{
    const uint16_t* key = (uint16_t*) mk;
    uint16_t* rk = (uint16_t*) rks;

    for (size_t i = 0; i < 8; ++i) {
        rk[i] = key[i] ^ rol16(key[i], 1);
        rk[(i+8)^(0x1)] = rk[i] ^ rol16(key[i], 11);
        rk[i] ^= rol16(key[i], 8);
    }
}

void cham64_encrypt(uint8_t* out, const uint8_t* in, const uint8_t* rks)
{
    const uint16_t* block = (const uint16_t*) in;
    const uint16_t* rk = (const uint16_t*) rks;
    uint16_t* outblk = (uint16_t*) out;

    uint16_t b0 = block[0];
    uint16_t b1 = block[1];
    uint16_t b2 = block[2];
    uint16_t b3 = block[3];

    uint16_t rc = 0;
    for (size_t round = 0; round < 80; round += 16) {
        b0 = rol16((b0 ^ (rc++)) + (rol16(b1, 1) ^ rk[0]), 8);
        b1 = rol16((b1 ^ (rc++)) + (rol16(b2, 8) ^ rk[1]), 1);
        b2 = rol16((b2 ^ (rc++)) + (rol16(b3, 1) ^ rk[2]), 8);
        b3 = rol16((b3 ^ (rc++)) + (rol16(b0, 8) ^ rk[3]), 1);

        b0 = rol16((b0 ^ (rc++)) + (rol16(b1, 1) ^ rk[4]), 8);
        b1 = rol16((b1 ^ (rc++)) + (rol16(b2, 8) ^ rk[5]), 1);
        b2 = rol16((b2 ^ (rc++)) + (rol16(b3, 1) ^ rk[6]), 8);
        b3 = rol16((b3 ^ (rc++)) + (rol16(b0, 8) ^ rk[7]), 1);

        b0 = rol16((b0 ^ (rc++)) + (rol16(b1, 1) ^ rk[8]), 8);
        b1 = rol16((b1 ^ (rc++)) + (rol16(b2, 8) ^ rk[9]), 1);
        b2 = rol16((b2 ^ (rc++)) + (rol16(b3, 1) ^ rk[10]), 8);
        b3 = rol16((b3 ^ (rc++)) + (rol16(b0, 8) ^ rk[11]), 1);

        b0 = rol16((b0 ^ (rc++)) + (rol16(b1, 1) ^ rk[12]), 8);
        b1 = rol16((b1 ^ (rc++)) + (rol16(b2, 8) ^ rk[13]), 1);
        b2 = rol16((b2 ^ (rc++)) + (rol16(b3, 1) ^ rk[14]), 8);
        b3 = rol16((b3 ^ (rc++)) + (rol16(b0, 8) ^ rk[15]), 1);
    }

    outblk[0] = b0;
    outblk[1] = b1;
    outblk[2] = b2;
    outblk[3] = b3;
}

void cham64_decrypt(uint8_t* out, const uint8_t* in, const uint8_t* rks)
{
    const uint16_t* block = (const uint16_t*) in;
    const uint16_t* rk = (const uint16_t*) rks;
    uint16_t* outblk = (uint16_t*) out;

    uint16_t b0 = block[0];
    uint16_t b1 = block[1];
    uint16_t b2 = block[2];
    uint16_t b3 = block[3];

    uint16_t rc = 80;
    for (size_t round = 0; round < 80; round += 16) {        
        b3 = (ror16(b3, 1) - (rol16(b0, 8) ^ rk[15])) ^ (--rc);
        b2 = (ror16(b2, 8) - (rol16(b3, 1) ^ rk[14])) ^ (--rc);
        b1 = (ror16(b1, 1) - (rol16(b2, 8) ^ rk[13])) ^ (--rc);
        b0 = (ror16(b0, 8) - (rol16(b1, 1) ^ rk[12])) ^ (--rc);

        b3 = (ror16(b3, 1) - (rol16(b0, 8) ^ rk[11])) ^ (--rc);
        b2 = (ror16(b2, 8) - (rol16(b3, 1) ^ rk[10])) ^ (--rc);
        b1 = (ror16(b1, 1) - (rol16(b2, 8) ^ rk[9])) ^ (--rc);
        b0 = (ror16(b0, 8) - (rol16(b1, 1) ^ rk[8])) ^ (--rc);

        b3 = (ror16(b3, 1) - (rol16(b0, 8) ^ rk[7])) ^ (--rc);
        b2 = (ror16(b2, 8) - (rol16(b3, 1) ^ rk[6])) ^ (--rc);
        b1 = (ror16(b1, 1) - (rol16(b2, 8) ^ rk[5])) ^ (--rc);
        b0 = (ror16(b0, 8) - (rol16(b1, 1) ^ rk[4])) ^ (--rc);

        b3 = (ror16(b3, 1) - (rol16(b0, 8) ^ rk[3])) ^ (--rc);
        b2 = (ror16(b2, 8) - (rol16(b3, 1) ^ rk[2])) ^ (--rc);
        b1 = (ror16(b1, 1) - (rol16(b2, 8) ^ rk[1])) ^ (--rc);
        b0 = (ror16(b0, 8) - (rol16(b1, 1) ^ rk[0])) ^ (--rc);
    }

    outblk[0] = b0;
    outblk[1] = b1;
    outblk[2] = b2;
    outblk[3] = b3;
}

/**
 * CHAM 128-bit block, 128-bit key
 */ 
void cham128_keygen(uint8_t* rks, const uint8_t* mk)
{
    const uint32_t* key = (uint32_t*) mk;
    uint32_t* rk = (uint32_t*) rks;

    for (size_t i = 0; i < 4; ++i) {
        rk[i] = key[i] ^ rol32(key[i], 1);
        rk[(i+4)^(0x1)] = rk[i] ^ rol32(key[i], 11);
        rk[i] ^= rol32(key[i], 8);
    }
}

void cham128_encrypt(uint8_t* out, const uint8_t* in, const uint8_t* rks)
{
    const uint32_t* block = (const uint32_t*) in;
    const uint32_t* rk = (const uint32_t*) rks;
    uint32_t* outblk = (uint32_t*) out;

    uint32_t b0 = block[0];
    uint32_t b1 = block[1];
    uint32_t b2 = block[2];
    uint32_t b3 = block[3];

    uint32_t rc = 0;
    for (size_t round = 0; round < 80; round += 8) {
        b0 = rol32((b0 ^ (rc++)) + (rol32(b1, 1) ^ rk[0]), 8);
        b1 = rol32((b1 ^ (rc++)) + (rol32(b2, 8) ^ rk[1]), 1);
        b2 = rol32((b2 ^ (rc++)) + (rol32(b3, 1) ^ rk[2]), 8);
        b3 = rol32((b3 ^ (rc++)) + (rol32(b0, 8) ^ rk[3]), 1);

        b0 = rol32((b0 ^ (rc++)) + (rol32(b1, 1) ^ rk[4]), 8);
        b1 = rol32((b1 ^ (rc++)) + (rol32(b2, 8) ^ rk[5]), 1);
        b2 = rol32((b2 ^ (rc++)) + (rol32(b3, 1) ^ rk[6]), 8);
        b3 = rol32((b3 ^ (rc++)) + (rol32(b0, 8) ^ rk[7]), 1);
    }

    outblk[0] = b0;
    outblk[1] = b1;
    outblk[2] = b2;
    outblk[3] = b3;
}

void cham128_decrypt(uint8_t* out, const uint8_t* in, const uint8_t* rks)
{
    const uint32_t* block = (const uint32_t*) in;
    const uint32_t* rk = (const uint32_t*) rks;
    uint32_t* outblk = (uint32_t*) out;

    uint32_t b0 = block[0];
    uint32_t b1 = block[1];
    uint32_t b2 = block[2];
    uint32_t b3 = block[3];

    uint32_t rc = 80;
    for (size_t round = 0; round < 80; round += 8) {        
        b3 = (ror32(b3, 1) - (rol32(b0, 8) ^ rk[7])) ^ (--rc);
        b2 = (ror32(b2, 8) - (rol32(b3, 1) ^ rk[6])) ^ (--rc);
        b1 = (ror32(b1, 1) - (rol32(b2, 8) ^ rk[5])) ^ (--rc);
        b0 = (ror32(b0, 8) - (rol32(b1, 1) ^ rk[4])) ^ (--rc);

        b3 = (ror32(b3, 1) - (rol32(b0, 8) ^ rk[3])) ^ (--rc);
        b2 = (ror32(b2, 8) - (rol32(b3, 1) ^ rk[2])) ^ (--rc);
        b1 = (ror32(b1, 1) - (rol32(b2, 8) ^ rk[1])) ^ (--rc);
        b0 = (ror32(b0, 8) - (rol32(b1, 1) ^ rk[0])) ^ (--rc);
    }

    outblk[0] = b0;
    outblk[1] = b1;
    outblk[2] = b2;
    outblk[3] = b3;
}

/**
 * CHAM 128-bit block, 256-bit key
 */ 
void cham256_keygen(uint8_t* rks, const uint8_t* mk)
{
    const uint32_t* key = (uint32_t*) mk;
    uint32_t* rk = (uint32_t*) rks;

    for (size_t i = 0; i < 8; ++i) {
        rk[i] = key[i] ^ rol32(key[i], 1);
        rk[(i+8)^(0x1)] = rk[i] ^ rol32(key[i], 11);
        rk[i] ^= rol32(key[i], 8);
    }
}

void cham256_encrypt(uint8_t* out, const uint8_t* in, const uint8_t* rks)
{
    const uint32_t* block = (const uint32_t*) in;
    const uint32_t* rk = (const uint32_t*) rks;
    uint32_t* outblk = (uint32_t*) out;

    uint32_t b0 = block[0];
    uint32_t b1 = block[1];
    uint32_t b2 = block[2];
    uint32_t b3 = block[3];

    uint32_t rc = 0;
    for (size_t round = 0; round < 96; round += 16) {
        b0 = rol32((b0 ^ (rc++)) + (rol32(b1, 1) ^ rk[0]), 8);
        b1 = rol32((b1 ^ (rc++)) + (rol32(b2, 8) ^ rk[1]), 1);
        b2 = rol32((b2 ^ (rc++)) + (rol32(b3, 1) ^ rk[2]), 8);
        b3 = rol32((b3 ^ (rc++)) + (rol32(b0, 8) ^ rk[3]), 1);

        b0 = rol32((b0 ^ (rc++)) + (rol32(b1, 1) ^ rk[4]), 8);
        b1 = rol32((b1 ^ (rc++)) + (rol32(b2, 8) ^ rk[5]), 1);
        b2 = rol32((b2 ^ (rc++)) + (rol32(b3, 1) ^ rk[6]), 8);
        b3 = rol32((b3 ^ (rc++)) + (rol32(b0, 8) ^ rk[7]), 1);

        b0 = rol32((b0 ^ (rc++)) + (rol32(b1, 1) ^ rk[8]), 8);
        b1 = rol32((b1 ^ (rc++)) + (rol32(b2, 8) ^ rk[9]), 1);
        b2 = rol32((b2 ^ (rc++)) + (rol32(b3, 1) ^ rk[10]), 8);
        b3 = rol32((b3 ^ (rc++)) + (rol32(b0, 8) ^ rk[11]), 1);

        b0 = rol32((b0 ^ (rc++)) + (rol32(b1, 1) ^ rk[12]), 8);
        b1 = rol32((b1 ^ (rc++)) + (rol32(b2, 8) ^ rk[13]), 1);
        b2 = rol32((b2 ^ (rc++)) + (rol32(b3, 1) ^ rk[14]), 8);
        b3 = rol32((b3 ^ (rc++)) + (rol32(b0, 8) ^ rk[15]), 1);
    }

    outblk[0] = b0;
    outblk[1] = b1;
    outblk[2] = b2;
    outblk[3] = b3;
}

void cham256_decrypt(uint8_t* out, const uint8_t* in, const uint8_t* rks)
{
    const uint32_t* block = (const uint32_t*) in;
    const uint32_t* rk = (const uint32_t*) rks;
    uint32_t* outblk = (uint32_t*) out;

    uint32_t b0 = block[0];
    uint32_t b1 = block[1];
    uint32_t b2 = block[2];
    uint32_t b3 = block[3];

    uint16_t rc = 96;
    for (size_t round = 0; round < 96; round += 16) {        
        b3 = (ror32(b3, 1) - (rol32(b0, 8) ^ rk[15])) ^ (--rc);
        b2 = (ror32(b2, 8) - (rol32(b3, 1) ^ rk[14])) ^ (--rc);
        b1 = (ror32(b1, 1) - (rol32(b2, 8) ^ rk[13])) ^ (--rc);
        b0 = (ror32(b0, 8) - (rol32(b1, 1) ^ rk[12])) ^ (--rc);

        b3 = (ror32(b3, 1) - (rol32(b0, 8) ^ rk[11])) ^ (--rc);
        b2 = (ror32(b2, 8) - (rol32(b3, 1) ^ rk[10])) ^ (--rc);
        b1 = (ror32(b1, 1) - (rol32(b2, 8) ^ rk[9])) ^ (--rc);
        b0 = (ror32(b0, 8) - (rol32(b1, 1) ^ rk[8])) ^ (--rc);

        b3 = (ror32(b3, 1) - (rol32(b0, 8) ^ rk[7])) ^ (--rc);
        b2 = (ror32(b2, 8) - (rol32(b3, 1) ^ rk[6])) ^ (--rc);
        b1 = (ror32(b1, 1) - (rol32(b2, 8) ^ rk[5])) ^ (--rc);
        b0 = (ror32(b0, 8) - (rol32(b1, 1) ^ rk[4])) ^ (--rc);

        b3 = (ror32(b3, 1) - (rol32(b0, 8) ^ rk[3])) ^ (--rc);
        b2 = (ror32(b2, 8) - (rol32(b3, 1) ^ rk[2])) ^ (--rc);
        b1 = (ror32(b1, 1) - (rol32(b2, 8) ^ rk[1])) ^ (--rc);
        b0 = (ror32(b0, 8) - (rol32(b1, 1) ^ rk[0])) ^ (--rc);
    }

    outblk[0] = b0;
    outblk[1] = b1;
    outblk[2] = b2;
    outblk[3] = b3;
}
