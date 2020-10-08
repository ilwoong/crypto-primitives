/**
 * MIT License
 * 
 * Copyright (c) 2019-2020 Ilwoong Jeong (https://github.com/ilwoong)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"), 
 * to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "hight.h"

#include <stdio.h>
#include <string.h>
#include <omp.h>

static void print_array(const uint8_t* array, size_t count)
{
    size_t i = 0;
    for (i = 0; i < count; ++i) {
        printf("%02x", array[i]); 
    }
    printf("\n");
}

static void print_result(const char* title, const uint8_t* pt, const uint8_t* encrypted, const uint8_t* ct, const uint8_t* decrypted)
{
    printf("%s\n", title);
    if (memcmp(ct, encrypted, 8) == 0) {
        printf("\tenc passed\n");
    } else {
        printf("\tenc failed\n");
        printf("\t\t");
        print_array(ct, 8);
        printf("\t\t");
        print_array(encrypted, 8);
    }
        
    if (memcmp(pt, decrypted, 8) == 0) {
        printf("\tdec passed\n");
    } else {
        printf("\tdec failed\n");
        printf("\t\t");
        print_array(pt, 8);
        printf("\t\t");
        print_array(decrypted, 8);
    }
    
    printf("\n");
}

void benchmark(size_t iterations) {
    size_t i = 0;
    uint8_t mk[] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
    uint8_t pt[8] = {0, };
    uint8_t enc[8] = {0, };
    uint8_t rks[136] = {0, };

    hight_keygen(rks, mk);
    double elapsed = omp_get_wtime();    
    for (i = 0; i < iterations; ++i) {
        hight_encrypt(enc, pt, rks);
    }
    elapsed = omp_get_wtime() - elapsed;

    printf("Elapsed time for %ld iterations: %lf\n", iterations, elapsed);
}

int main(int argc, const char** argv)
{
    uint8_t pt[] = {0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00};
    uint8_t ct[] = {0xd8, 0xe6, 0x43, 0xe5, 0x72, 0x9f, 0xce, 0x23};
    uint8_t mk[] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
    uint8_t rks[136] = {0};
    uint8_t enc[8] = {0};
    uint8_t dec[8] = {0};

    hight_keygen(rks, mk);
    hight_encrypt(enc, pt, rks);
    hight_decrypt(dec, ct, rks);

    print_result("HIGHT", pt, enc, ct, dec);
    benchmark(1000000);

    return 0;
}