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

#include <stdio.h>
#include <stdint.h>
#include "sbox.h"
#include "gf256.h"

const uint8_t SBOX_COEF[] = {
    0xf1, 0xe3, 0xc7, 0x8f, 0x1f, 0x3e, 0x7c, 0xf8, 
};

const uint8_t SINV_COEF[] = {
    0xa4, 0x49, 0x92, 0x25, 0x4a, 0x94, 0x29, 0x52, 
};

uint8_t affine_sbox(uint8_t input)
{
    uint8_t result = 0;
    input = gf256_inv(input);
    for (int i = 0; i < 8; ++i) {
        uint8_t row = input & SBOX_COEF[i];
        uint8_t bit = 0;
        for (int j = 0; j < 8; ++j) {
            bit ^= (row & 0x1);
            row >>= 1;
        }

        result ^= (bit << i);
    }

    return result ^ 0x63;
}

uint8_t affine_sinv(uint8_t input)
{
    uint8_t result = 0;    
    for (int i = 0; i < 8; ++i) {
        uint8_t row = input & SINV_COEF[i];
        uint8_t bit = 0;
        for (int j = 0; j < 8; ++j) {
            bit ^= (row & 0x1);
            row >>= 1;
        }

        result ^= (bit << i);
    }

    return gf256_inv(result ^ 0x05);
}

static void print_sbox_table(const char* title, uint8_t (*affine)(uint8_t))
{
    printf("%s\n", title);
    for (size_t i = 0; i < 256; ++i) {
        printf("%02x ", affine((uint8_t) i));
        
        if (((i + 1) & 0xf) == 0) {
            printf("\n");
        }
        
    }
    printf("\n");
}

void print_sbox_tables()
{
    print_sbox_table("AES SBOX", affine_sbox);
    print_sbox_table("AES SINV", affine_sinv);
}