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

#include "seed.h"

#include <stdio.h>
#include <string.h>

void print(const char* title, const uint8_t* data, size_t count)
{
    printf("%s: ", title);
    for (int i = 0; i < count; ++i) {
        printf("%02x", data[i]);
        if ( (i & 0x3) == 0x3) {
            printf(" ");
        }
    }

    printf("\n");
}

void print_rks(const uint8_t* rks) 
{
    const uint32_t* rk = (const uint32_t*) rks;

    for (int i = 0; i < 16; ++i) {
        printf("%02d: %08x, %08x\n", i, *(rk++), *(rk++));
    }
    printf("\n");
}

int main(int argc, const char** argv)
{
    uint8_t mk[16] = {
        0,
    };

    uint8_t pt[] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    };

    uint8_t ct[] = {
        0x5e, 0xba, 0xc6, 0xe0, 0x05, 0x4e, 0x16, 0x68, 0x19, 0xaf, 0xf1, 0xcc, 0x6d, 0x34, 0x6c, 0xdb,        
    };

    uint8_t rks[8 * 16] = {0};
    uint8_t enc[16] = {0};
    uint8_t dec[16] = {0};

    seed_keygen(rks, mk);
    //print_rks(rks);

    seed_encrypt(enc, pt, rks);
    seed_decrypt(dec, enc, rks);

    int encpassed = memcmp(ct, enc, 16) == 0;
    int decpassed = memcmp(pt, dec, 16) == 0;

    printf("SEED testvector: ");
    if (encpassed && decpassed) {
        printf("psssed\n");
        print("   mk", mk, 16);
        print("   pt", pt, 16);
        print("   ct", ct, 16);
    }

    if (!encpassed) {
        printf("\n");
        printf("   encryption failed \n");
        print("   expected", ct, 16);
        print("        but", enc, 16);
    }

    if (!decpassed) {
        printf("\n");
        printf("   decryption failed \n");
        print("   expected", pt, 16);
        print("        but", dec, 16);
    }
    printf("\n");

    return 0;
}