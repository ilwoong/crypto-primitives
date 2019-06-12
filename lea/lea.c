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

#include "lea.h"
#include "inline.inc"

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//! rotation functions
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
static FORCE_INLINE uint32_t rol32(uint32_t value, size_t rot)
{
    return (value << rot) | (value >> (32 - rot));
}

static FORCE_INLINE uint32_t ror32(uint32_t value, size_t rot)
{
    return (value >> rot) | (value << (32 - rot));
}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//! common encryption function
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
static FORCE_INLINE void lea_encrypt(uint8_t* out, const uint8_t* in, const uint8_t* rks, size_t rounds)
{
    const uint32_t* rk = (const uint32_t*) rks;
    const uint32_t* block = (const uint32_t*) in;
    uint32_t* outblk = (uint32_t*) out;

    uint32_t b0 = block[0];
    uint32_t b1 = block[1];
    uint32_t b2 = block[2];
    uint32_t b3 = block[3];

    for (size_t round = 0; round < rounds; round += 4)
    {
        b3 = ror32((b2 ^ rk[4]) + (b3 ^ rk[5]), 3);
        b2 = ror32((b1 ^ rk[2]) + (b2 ^ rk[3]), 5);
        b1 = rol32((b0 ^ rk[0]) + (b1 ^ rk[1]), 9);
        rk += 6;

        b0 = ror32((b3 ^ rk[4]) + (b0 ^ rk[5]), 3);
        b3 = ror32((b2 ^ rk[2]) + (b3 ^ rk[3]), 5);
        b2 = rol32((b1 ^ rk[0]) + (b2 ^ rk[1]), 9);
        rk += 6;

        b1 = ror32((b0 ^ rk[4]) + (b1 ^ rk[5]), 3);
        b0 = ror32((b3 ^ rk[2]) + (b0 ^ rk[3]), 5);
        b3 = rol32((b2 ^ rk[0]) + (b3 ^ rk[1]), 9);
        rk += 6;

        b2 = ror32((b1 ^ rk[4]) + (b2 ^ rk[5]), 3);
        b1 = ror32((b0 ^ rk[2]) + (b1 ^ rk[3]), 5);
        b0 = rol32((b3 ^ rk[0]) + (b0 ^ rk[1]), 9);
        rk += 6;
    }

    outblk[0] = b0;
    outblk[1] = b1;
    outblk[2] = b2;
    outblk[3] = b3;
}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//! common decryption function
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
static FORCE_INLINE void lea_decrypt(uint8_t* out, const uint8_t* in, const uint8_t* rks, size_t rounds)
{
    const uint32_t* rk = (const uint32_t*) rks;
    const uint32_t* block = (const uint32_t*) in;
    uint32_t* outblk = (uint32_t*) out;

    uint32_t b0 = block[0];
    uint32_t b1 = block[1];
    uint32_t b2 = block[2];
    uint32_t b3 = block[3];

    rk += 6 * (rounds - 1);
    for (size_t round = 0; round < rounds; round += 4)
    {
        b0 = (ror32(b0, 9) - (b3 ^ rk[0])) ^ rk[1];
        b1 = (rol32(b1, 5) - (b0 ^ rk[2])) ^ rk[3];
        b2 = (rol32(b2, 3) - (b1 ^ rk[4])) ^ rk[5];
        rk -= 6;

        b3 = (ror32(b3, 9) - (b2 ^ rk[0])) ^ rk[1];
        b0 = (rol32(b0, 5) - (b3 ^ rk[2])) ^ rk[3];
        b1 = (rol32(b1, 3) - (b0 ^ rk[4])) ^ rk[5];
        rk -= 6;

        b2 = (ror32(b2, 9) - (b1 ^ rk[0])) ^ rk[1];
        b3 = (rol32(b3, 5) - (b2 ^ rk[2])) ^ rk[3];
        b0 = (rol32(b0, 3) - (b3 ^ rk[4])) ^ rk[5];
        rk -= 6;

        b1 = (ror32(b1, 9) - (b0 ^ rk[0])) ^ rk[1];
        b2 = (rol32(b2, 5) - (b1 ^ rk[2])) ^ rk[3];
        b3 = (rol32(b3, 3) - (b2 ^ rk[4])) ^ rk[5];
        rk -= 6;
    }

    outblk[0] = b0;
    outblk[1] = b1;
    outblk[2] = b2;
    outblk[3] = b3;
}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//! encryption / decryption for 128-bit key
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
FORCE_INLINE void lea128_encrypt(uint8_t* out, const uint8_t* in, const uint8_t* rks)
{
    lea_encrypt(out, in, rks, LEA128_ROUNDS);
}

FORCE_INLINE void lea128_decrypt(uint8_t* out, const uint8_t* in, const uint8_t* rks)
{
    lea_decrypt(out, in, rks, LEA128_ROUNDS);
}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//! encryption / decryption for 192-bit key
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
FORCE_INLINE void lea192_encrypt(uint8_t* out, const uint8_t* in, const uint8_t* rks)
{
    lea_encrypt(out, in, rks, LEA192_ROUNDS);
}

FORCE_INLINE void lea192_decrypt(uint8_t* out, const uint8_t* in, const uint8_t* rks)
{
    lea_decrypt(out, in, rks, LEA192_ROUNDS);
}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//! encryption / decryption for 256-bit key
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
FORCE_INLINE void lea256_encrypt(uint8_t* out, const uint8_t* in, const uint8_t* rks)
{
    lea_encrypt(out, in, rks, LEA256_ROUNDS);
}

FORCE_INLINE void lea256_decrypt(uint8_t* out, const uint8_t* in, const uint8_t* rks)
{
    lea_decrypt(out, in, rks, LEA256_ROUNDS);
}
