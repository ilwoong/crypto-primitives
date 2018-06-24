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

#include "cham.h"
#include <stdio.h>
#include <string.h>

static void print_array(const uint8_t* array, size_t count)
{
    size_t i = 0;
    for (i = 0; i < count; ++i) {
        printf("%02x", array[i]); 
    }
    printf("\n");
}

void print_result(const char* title, const uint8_t* pt, const uint8_t* encrypted, const uint8_t* ct, const uint8_t* decrypted, size_t blocksize)
{
    printf("%s\n", title);
    if (memcmp(ct, encrypted, blocksize) == 0) {
        printf("\tenc passed\n");
    } else {
        printf("\tenc failed\n");
        printf("\t\t");
        print_array(ct, blocksize);
        printf("\t\t");
        print_array(encrypted, blocksize);
    }
        
    if (memcmp(pt, decrypted, blocksize) == 0) {
        printf("\tdec passed\n");
    } else {
        printf("\tdec failed\n");
        printf("\t\t");
        print_array(pt, blocksize);
        printf("\t\t");
        print_array(decrypted, blocksize);
    }
    
    printf("\n");
}

void test_cham64()
{
    uint8_t key[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
    uint8_t pt[8] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77};
    uint8_t ct[8] = {0x3c, 0x45, 0xbc, 0x63, 0xfa, 0xdc, 0x4e, 0xbf};
    uint8_t encrypted[8] = {0,};
    uint8_t decrypted[8] = {0,};

    uint8_t rks[2 * 16] = {0};

    cham64_keygen(rks, key);
    cham64_encrypt(encrypted, pt, rks);
    cham64_decrypt(decrypted, ct, rks);

    print_result("CHAM-64-64", pt, encrypted, ct, decrypted, 8);
}

void test_cham128()
{
    uint8_t key[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
    uint8_t pt[16] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
    uint8_t ct[16] = {0x34, 0x60, 0x74, 0xc3, 0xc5, 0x00, 0x57, 0xb5, 0x32, 0xec, 0x64, 0x8d, 0xf7, 0x32, 0x93, 0x48};
    uint8_t encrypted[16] = {0,};
    uint8_t decrypted[16] = {0,};

    uint8_t rks[2 * 16] = {0};

    cham128_keygen(rks, key);
    cham128_encrypt(encrypted, pt, rks);
    cham128_decrypt(decrypted, ct, rks);

    print_result("CHAM-128-128", pt, encrypted, ct, decrypted, 16);
}

void test_cham256()
{
    uint8_t key[32] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
    };
    uint8_t pt[16] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
    uint8_t ct[16] = {0xa0, 0xc8, 0x99, 0xa8, 0x5c, 0xd5, 0x29, 0xc9, 0x38, 0x0d, 0x67, 0xab, 0xc8, 0x7a, 0x4f, 0x0c};
    uint8_t encrypted[16] = {0,};
    uint8_t decrypted[16] = {0,};

    uint8_t rks[4 * 16] = {0};

    cham256_keygen(rks, key);
    cham256_encrypt(encrypted, pt, rks);
    cham256_decrypt(decrypted, ct, rks);

    print_result("CHAM-128-256", pt, encrypted, ct, decrypted, 16);
}

int main()
{
    test_cham64();
    test_cham128();
    test_cham256();

    return 0;
}