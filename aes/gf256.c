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

#include "gf256.h"

static uint8_t bitlength(uint16_t value) 
{
    uint8_t len = 8;
    for (int i  = 0; i < 16; ++i) {
        if ((value & 0x8000) == 0x8000) {
            break;
        }

        len -= 1;
        value <<= 1;
    }

    return len;
}

uint8_t gf256_mul(uint8_t lhs, uint8_t rhs)
{
    uint8_t result = 0;
    uint8_t carry = 0;

    for (int i = 0; i < 8; ++i) {
        if (rhs & 1) {
            result ^= lhs;
        }
        carry = lhs & 0x80;
        
        lhs <<= 1;
        if (carry) {
            lhs ^= 0x1b;
        }

        rhs >>= 1;
    }

    return result;
}

uint8_t gf256_inv(uint8_t value)
{
    uint16_t u1 = 0, u3 = 0x11b, v1 = 1, v3 = value;

    while (v3 != 0) {
        uint16_t t1 = u1, t3 = u3;
        int8_t q = bitlength(u3) - bitlength(v3);

        if (q >= 0) {
            t1 ^= v1 << q;
            t3 ^= v3 << q;
        }

        u1 = v1; 
        u3 = v3;
        v1 = t1; 
        v3 = t3;
    }

    if (u1 >= 0x100) {
        u1 ^= 0x11b;
    }

    return u1;
}