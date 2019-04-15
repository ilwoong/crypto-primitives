/**
 * MIT License
 * 
 * Copyright (c) 2019 Ilwoong Jeong, https://github.com/ilwoong
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

#include "ctr.h"
#include <stdlib.h>
#include <string.h>

static void xor(uint8_t* lhs, const uint8_t* rhs, size_t length)
{
    for (size_t i = 0; i < length; ++i) {
        lhs[i] ^= rhs[i];
    }
}

static void increase_counter(uint8_t* ctr, size_t length)
{
    size_t idx = length - 1;
    while ( (++ctr[idx]) == 0 && idx != 0) {
        --idx;
    }
}

void ctr_encrypt(uint8_t* ct, const uint8_t* pt, const uint8_t* rks, const uint8_t* iv, size_t blocksize, size_t length, void(*encrypt)(uint8_t*, const uint8_t*, const uint8_t*))
{
    uint8_t* ctr = calloc(blocksize, sizeof(uint8_t));
    memcpy(ctr, iv, blocksize);

    while (length >= blocksize) {
        encrypt(ct, ctr, rks);
        xor(ct, pt, blocksize);
        increase_counter(ctr, blocksize);

        pt += blocksize;
        ct += blocksize;
        length -= blocksize;
    }

    if (length > 0) {
        encrypt(ct, ctr, rks);
        xor(ct, pt, length);
    }

    free(ctr);
}

void ctr_decrypt(uint8_t* pt, const uint8_t* ct, const uint8_t* rks, const uint8_t* iv, size_t blocksize, size_t length, void(*encrypt)(uint8_t*, const uint8_t*, const uint8_t*))
{
    ctr_encrypt(pt, ct, rks, iv, blocksize, length, encrypt);
}