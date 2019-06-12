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
#include <string.h>

#if _MSC_VER && !__INTEL_COMPILER
    #define FORCE_INLINE __force_inline
#else
    #define FORCE_INLINE __attribute__((always_inline)) inline
#endif

const static uint32_t DELTA[8]= {
    0xc3efe9db, 0x44626b02, 0x79e27c8a, 0x78df30ec, 0x715ea49e, 0xc785da0a, 0xe04ef22a, 0xe5c40957,
};

static FORCE_INLINE uint32_t rol32(uint32_t value, size_t rot)
{
    return (value << rot) | (value >> (32 - rot));
}

static FORCE_INLINE uint32_t ror32(uint32_t value, size_t rot)
{
    return (value >> rot) | (value << (32 - rot));
}

/**
 * LEA 128-bit block, 128-bit key 
 */
void lea128_keygen(uint8_t* out, const uint8_t* mk)
{
    const uint32_t* t = (const uint32_t*) mk;
    uint32_t* rk = (uint32_t*) out;
    
    uint32_t t0 = t[0];
    uint32_t t1 = t[1];
    uint32_t t2 = t[2];
    uint32_t t3 = t[3];

    for(size_t round = 0; round < LEA128_ROUNDS; ++round) {
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

/**
 * LEA 128-bit block, 192-bit key 
 */
void lea192_keygen(uint8_t* out, const uint8_t* mk)
{
    const uint32_t* t = (const uint32_t*) mk;
    uint32_t* rk = (uint32_t*) out;
    
    uint32_t t0 = t[0];
    uint32_t t1 = t[1];
    uint32_t t2 = t[2];
    uint32_t t3 = t[3];
    uint32_t t4 = t[4];
    uint32_t t5 = t[5];

    for(size_t round = 0; round < LEA192_ROUNDS; ++round) {
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

/**
 * LEA 128-bit block, 256-bit key 
 */
void lea256_keygen(uint8_t* out, const uint8_t* mk)
{
    const uint32_t* keys = (const uint32_t*) mk;
    uint32_t* rk = (uint32_t*) out;

    uint32_t t[8] = {0, };
    memcpy(t, keys, 8 * sizeof(uint32_t));
    
    for(size_t round = 0; round < LEA256_ROUNDS; ++round) {
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