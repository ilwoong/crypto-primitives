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

#include "aes.h"
#include <stdio.h>
#include <string.h>
#include <omp.h>

static void print_hex(const char* title, const uint8_t* data, size_t count)
{
    printf("%s: ", title);
    for (size_t i = 0; i < count; ++i) {
        printf("%02x", data[i]);

        if (((i+1) & 0xf) == 0) {
            printf("\n");
        } else if ( ((i+1) & 0x3) == 0) {
            printf(" ");
        }
    }

    if ( (count & 0xf) != 0) {
        printf("\n");
    }
    
}

static void compare_block(const char* title, const uint8_t* pt, const uint8_t* ct, const uint8_t* enc, const uint8_t* dec)
{
    int out = 0;
    if(memcmp(ct, enc, 16)) out=1;
    if(memcmp(pt, dec, 16)) out|=2;

    printf("%s\n", title);
    print_hex("ct", enc, 16);
    print_hex("pt", dec, 16);

    if (out == 0) {
        printf("passed\n");
    }

    if (out & 0x1) {
        printf("encryption failed\n");
    }

    if (out & 0x2) {
        printf("decryption failed\n");
    }
    printf("\n");
}

//return 0 if no error
//1 if encryption failed
//2 if decryption failed
//3 if both failed
static int aes128_self_test(void)
{
    uint8_t mk[] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
    uint8_t pt[] = {0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a, 0x30, 0x8d, 0x31, 0x31, 0x98, 0xa2, 0xe0, 0x37, 0x07, 0x34};
    uint8_t ct[] = {0x39, 0x25, 0x84, 0x1d, 0x02, 0xdc, 0x09, 0xfb, 0xdc, 0x11, 0x85, 0x97, 0x19, 0x6a, 0x0b, 0x32};
    
    uint8_t enc[16] = {0};
    uint8_t dec[16] = {0};

    uint8_t rks[(AES128_ROUNDS + 1) * 16] = {0,};
    aes128_keygen(rks, mk);

    aes128_encrypt(enc, pt, rks);
    aes128_decrypt(dec, ct, rks);
    compare_block("AES-128", pt, ct, enc, dec);
}

static int aes192_self_test() 
{
    uint8_t mk[] = {0x8e, 0x73, 0xb0, 0xf7, 0xda, 0x0e, 0x64, 0x52, 0xc8, 0x10, 0xf3, 0x2b, 0x80, 0x90, 0x79, 0xe5, 0x62, 0xf8, 0xea, 0xd2, 0x52, 0x2c, 0x6b, 0x7b};
    uint8_t pt[] = {0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a};
    uint8_t ct[] = {0xbd, 0x33, 0x4f, 0x1d, 0x6e, 0x45, 0xf2, 0x5f, 0xf7, 0x12, 0xa2, 0x14, 0x57, 0x1f, 0xa5, 0xcc};
    
    uint8_t enc[16] = {0};
    uint8_t dec[16] = {0};

    uint8_t rks[(AES192_ROUNDS + 1) * 16] = {0,};
    aes192_keygen(rks, mk);

    aes192_encrypt(enc, pt, rks);
    aes192_decrypt(dec, ct, rks);
    compare_block("AES-192", pt, ct, enc, dec);
}

static int aes256_self_test() 
{
    uint8_t mk[] = {0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe, 0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81, 0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7, 0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4};
    uint8_t pt[] = {0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a};
    uint8_t ct[] = {0xf3, 0xee, 0xd1, 0xbd, 0xb5, 0xd2, 0xa0, 0x3c, 0x06, 0x4b, 0x5a, 0x7e, 0x3d, 0xb1, 0x81, 0xf8};
    
    uint8_t enc[16] = {0};
    uint8_t dec[16] = {0};

    uint8_t rks[(AES256_ROUNDS + 1) * 16] = {0,};
    aes256_keygen(rks, mk);

    aes256_encrypt(enc, pt, rks);
    aes256_decrypt(dec, ct, rks);
    compare_block("AES-256", pt, ct, enc, dec);
}

static void benchmark(size_t iterations)
{
    uint8_t mk[16] = {0};
    uint8_t pt[16] = {0};
    uint8_t ct[16] = {0};

    uint8_t enc[16] = {0};
    uint8_t dec[16] = {0};

    uint8_t rks[(AES128_ROUNDS + 1) * 16] = {0,};
    aes128_keygen(rks, mk);

    double start = omp_get_wtime();

    for (size_t i = 0; i < iterations; ++i) {
        aes128_encrypt(enc, pt, rks);
    }

    double elapsed = omp_get_wtime() - start;

    printf("Elapsed for %ld encryptions: %lf sec\n", iterations, elapsed);
}

int main()
{
    aes128_self_test();
    aes192_self_test();
    aes256_self_test();

    benchmark(1000);

    return 0;
}