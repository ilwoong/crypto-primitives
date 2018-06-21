/**
 * The MIT License
 * 
 * Copyright (c) <2018> <Ilwoong Jeong, ilwoong.jeong@gmail.com>
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
#include <string.h>

const static size_t LEA128_ROUNDS = 24;
const static size_t LEA192_ROUNDS = 28;
const static size_t LEA256_ROUNDS = 32;

const static uint32_t DELTA[8]= {
    0xc3efe9db, 0x44626b02, 0x79e27c8a, 0x78df30ec,
    0x715ea49e, 0xc785da0a, 0xe04ef22a, 0xe5c40957,
};

static inline uint32_t rol32(uint32_t value, size_t rot)
{
    return (value << rot) | (value >> (32 - rot));
}

static inline uint32_t ror32(uint32_t value, size_t rot)
{
    return (value >> rot) | (value << (32 - rot));
}

static inline void lea_encrypt(uint8_t* out, const uint8_t* in, const uint8_t* rks, size_t rounds)
{
    uint32_t* block = (uint32_t*) in;
    uint32_t* outblk = (uint32_t*) out;
    uint32_t* rk = (uint32_t*) rks;
    size_t round = 0;

    uint32_t b0 = block[0];
    uint32_t b1 = block[1];
    uint32_t b2 = block[2];
    uint32_t b3 = block[3];

    for (round = 0; round < rounds; round += 4)
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

static inline void lea_decrypt(uint8_t* out, const uint8_t* in, const uint8_t* rks, size_t rounds)
{
    uint32_t* block = (uint32_t*) in;
    uint32_t* outblk = (uint32_t*) out;
    uint32_t* rk = (uint32_t*) rks;
    size_t round = 0;

    uint32_t b0 = block[0];
    uint32_t b1 = block[1];
    uint32_t b2 = block[2];
    uint32_t b3 = block[3];

    rk += 6 * (rounds - 1);
    for (round = 0; round < rounds; round += 4)
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

/**
 * LEA 128-bit block, 128-bit key 
 */
void lea128_keygen(uint8_t* out, const uint8_t* mk)
{
    uint32_t* rk = (uint32_t*) out;
    uint32_t* t = (uint32_t*) mk;    
    size_t round = 0;
    
    uint32_t t0 = t[0];
    uint32_t t1 = t[1];
    uint32_t t2 = t[2];
    uint32_t t3 = t[3];

    for(round = 0; round < LEA128_ROUNDS; ++round) {
        uint32_t delta = DELTA[round & 3];
        
        t0 = rol32(t0 + rol32(delta, round), 1);
        t1 = rol32(t1 + rol32(delta, round + 1), 3);
        t2 = rol32(t2 + rol32(delta, round + 2), 6);
        t3 = rol32(t3 + rol32(delta, round + 3), 11);

        rk[0] = t0;
        rk[1] = t1;
        rk[2] = t2;
        rk[3] = t1;
        rk[4] = t3;
        rk[5] = t1;
        rk += 6;
    }
}

void lea128_encrypt(uint8_t* out, const uint8_t* in, const uint8_t* rks)
{
    lea_encrypt(out, in, rks, LEA128_ROUNDS);
}

void lea128_decrypt(uint8_t* out, const uint8_t* in, const uint8_t* rks)
{
    lea_decrypt(out, in, rks, LEA128_ROUNDS);
}

/**
 * LEA 128-bit block, 192-bit key 
 */
void lea192_keygen(uint8_t* out, const uint8_t* mk)
{
    uint32_t* rk = (uint32_t*) out;
    uint32_t* t = (uint32_t*) mk;    
    size_t round = 0;
    
    uint32_t t0 = t[0];
    uint32_t t1 = t[1];
    uint32_t t2 = t[2];
    uint32_t t3 = t[3];
    uint32_t t4 = t[4];
    uint32_t t5 = t[5];

    for(round = 0; round < LEA192_ROUNDS; ++round) {
        uint32_t delta = DELTA[round % 6];
        
        t0 = rol32(t0 + rol32(delta, round), 1);
        t1 = rol32(t1 + rol32(delta, round + 1), 3);
        t2 = rol32(t2 + rol32(delta, round + 2), 6);
        t3 = rol32(t3 + rol32(delta, round + 3), 11);
        t4 = rol32(t4 + rol32(delta, round + 4), 13);
        t5 = rol32(t5 + rol32(delta, round + 5), 17);

        rk[0] = t0;
        rk[1] = t1;
        rk[2] = t2;
        rk[3] = t3;
        rk[4] = t4;
        rk[5] = t5;
        rk += 6;
    }
}

void lea192_encrypt(uint8_t* out, const uint8_t* in, const uint8_t* rks)
{
    lea_encrypt(out, in, rks, LEA192_ROUNDS);
}

void lea192_decrypt(uint8_t* out, const uint8_t* in, const uint8_t* rks)
{
    lea_decrypt(out, in, rks, LEA192_ROUNDS);
}

/**
 * LEA 128-bit block, 256-bit key 
 */
void lea256_keygen(uint8_t* out, const uint8_t* mk)
{
    uint32_t* rk = (uint32_t*) out;
    uint32_t* keys = (uint32_t*) mk;
    size_t round = 0;

    uint32_t t[8] = {0, };
    memcpy(t, keys, 8 * sizeof(uint32_t));
    
    for(round = 0; round < LEA256_ROUNDS; ++round) {
        uint32_t delta = DELTA[round & 0x7];
        
        t[(6 * round) & 0x7] = rol32(t[(6 * round) & 0x7] + rol32(delta, round), 1);
        t[(6 * round + 1) & 0x7] = rol32(t[(6 * round + 1) & 0x7] + rol32(delta, round + 1), 3);
        t[(6 * round + 2) & 0x7] = rol32(t[(6 * round + 2) & 0x7] + rol32(delta, round + 2), 6);
        t[(6 * round + 3) & 0x7] = rol32(t[(6 * round + 3) & 0x7] + rol32(delta, round + 3), 11);
        t[(6 * round + 4) & 0x7] = rol32(t[(6 * round + 4) & 0x7] + rol32(delta, round + 4), 13);
        t[(6 * round + 5) & 0x7] = rol32(t[(6 * round + 5) & 0x7] + rol32(delta, round + 5), 17);

        rk[0] = t[(6 * round) & 0x7];
        rk[1] = t[(6 * round + 1) & 0x7];
        rk[2] = t[(6 * round + 2) & 0x7];
        rk[3] = t[(6 * round + 3) & 0x7];
        rk[4] = t[(6 * round + 4) & 0x7];
        rk[5] = t[(6 * round + 5) & 0x7];
        rk += 6;
    }
}

void lea256_encrypt(uint8_t* out, const uint8_t* in, const uint8_t* rks)
{
    lea_encrypt(out, in, rks, LEA256_ROUNDS);
}

void lea256_decrypt(uint8_t* out, const uint8_t* in, const uint8_t* rks)
{
    lea_decrypt(out, in, rks, LEA256_ROUNDS);
}
