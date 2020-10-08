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

#include "mds.h"
#include "sbox.h"
#include "gf256.h"

static uint32_t mds(uint8_t input, const uint8_t coef[4])
{
    return (gf256_mul(input, coef[0]) << 24) ^ (gf256_mul(input, coef[1]) << 16) ^ (gf256_mul(input, coef[2]) << 8) ^ gf256_mul(input, coef[3]);
}

static void print_mds_table(const char* title, const uint8_t coef[4])
{
    printf("%s\n", title);
    for (size_t i = 0; i < 256; ++i) {
        printf("0x%08x, ", mds((uint8_t) i, coef));

        if (((i + 1) & 0x7) == 0) {
            printf("\n");
        }
    }
    printf("\n");
}

static void print_forward_table(const char* title, const uint8_t coef[4])
{
    printf("%s\n", title);
    for (size_t i = 0; i < 256; ++i) {
        printf("0x%08x, ", mds(affine_sbox((uint8_t) i), coef));

        if (((i + 1) & 0x7) == 0) {
            printf("\n");
        }
    }
    printf("\n");
}

static uint32_t inv_sub_word(uint32_t value)
{
    uint8_t* ptr = (uint8_t*) &value;
    
    ptr[0] = affine_sinv(ptr[0]);
    ptr[1] = affine_sinv(ptr[1]);
    ptr[2] = affine_sinv(ptr[2]);
    ptr[3] = affine_sinv(ptr[3]);

    return value;
}

static void print_inverse_table(const char* title, const uint8_t coef[4])
{
    printf("%s\n", title);
    for (size_t i = 0; i < 256; ++i) {
        printf("0x%08x, ", mds(affine_sinv((uint8_t) i), coef));

        if (((i + 1) & 0x7) == 0) {
            printf("\n");
        }
    }
    printf("\n");
}

static void print_mds() 
{
    const uint8_t coef0[] = {0x2, 0x1, 0x1, 0x3};
    const uint8_t coef1[] = {0x3, 0x2, 0x1, 0x1};
    const uint8_t coef2[] = {0x1, 0x3, 0x2, 0x1};
    const uint8_t coef3[] = {0x1, 0x1, 0x3, 0x2};

    const uint8_t coef4[] = {0xe, 0x9, 0xd, 0xb};
    const uint8_t coef5[] = {0xb, 0xe, 0x9, 0xd};
    const uint8_t coef6[] = {0xd, 0xb, 0xe, 0x9};
    const uint8_t coef7[] = {0x9, 0xd, 0xb, 0xe};

    print_mds_table("MC0", coef0);
    print_mds_table("MC1", coef1);
    print_mds_table("MC2", coef2);
    print_mds_table("MC3", coef3);

    print_mds_table("IMC0", coef4);
    print_mds_table("IMC1", coef5);
    print_mds_table("IMC2", coef6);
    print_mds_table("IMC3", coef7);
}

static void print_sbox_mds() 
{
    const uint8_t coef0[] = {0x2, 0x1, 0x1, 0x3};
    const uint8_t coef1[] = {0x3, 0x2, 0x1, 0x1};
    const uint8_t coef2[] = {0x1, 0x3, 0x2, 0x1};
    const uint8_t coef3[] = {0x1, 0x1, 0x3, 0x2};

    const uint8_t coef4[] = {0xe, 0x9, 0xd, 0xb};
    const uint8_t coef5[] = {0xb, 0xe, 0x9, 0xd};
    const uint8_t coef6[] = {0xd, 0xb, 0xe, 0x9};
    const uint8_t coef7[] = {0x9, 0xd, 0xb, 0xe};

    print_forward_table("SMC0", coef0);
    print_forward_table("SMC1", coef1);
    print_forward_table("SMC2", coef2);
    print_forward_table("SMC3", coef3);

    print_inverse_table("ISMC0", coef4);
    print_inverse_table("ISMC1", coef5);
    print_inverse_table("ISMC2", coef6);
    print_inverse_table("ISMC3", coef7);
}

void print_mds_tables()
{
    print_mds();
    print_sbox_mds();
}