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

#include "aria.h"
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

void print_result(const char* title, const uint8_t* pt, const uint8_t* encrypted, const uint8_t* ct, const uint8_t* decrypted)
{
    printf("%s\n", title);
    if (memcmp(ct, encrypted, 16) == 0) {
        printf("\tenc passed\n");
    } else {
        printf("\tenc failed\n");
        printf("\t\t");
        print_array(ct, 16);
        printf("\t\t");
        print_array(encrypted, 16);
    }
        
    if (memcmp(pt, decrypted, 16) == 0) {
        printf("\tdec passed\n");
    } else {
        printf("\tdec failed\n");
        printf("\t\t");
        print_array(pt, 16);
        printf("\t\t");
        print_array(decrypted, 16);
    }
    
    printf("\n");
}

void test_aria128() 
{
    uint8_t key[16] = {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
    };

    uint8_t pt[16] = {
        0x11, 0x11, 0x11, 0x11, 0xaa, 0xaa, 0xaa, 0xaa, 0x11, 0x11, 0x11, 0x11, 0xbb, 0xbb, 0xbb, 0xbb
    };

    uint8_t ct[16] = {
        0xc6, 0xec, 0xd0, 0x8e, 0x22, 0xc3, 0x0a, 0xbd, 0xb2, 0x15, 0xcf, 0x74, 0xe2, 0x07, 0x5e, 0x6e
    };

    uint8_t encrypted[16] = {0,};
    uint8_t decrypted[16] = {0,};

    uint8_t rks[13 * 16] = {0};

    aria128_expand_key_enc(rks, key);
    aria128_encrypt(encrypted, pt, rks);

    aria128_expand_key_dec(rks, key);
    aria128_decrypt(decrypted, ct, rks);

    print_result("aria128", pt, encrypted, ct, decrypted);
}

void test_aria192() 
{
    uint8_t key[24] = {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
    };

    uint8_t pt[16] = {
        0x11, 0x11, 0x11, 0x11, 0xaa, 0xaa, 0xaa, 0xaa, 0x11, 0x11, 0x11, 0x11, 0xbb, 0xbb, 0xbb, 0xbb
    };

    uint8_t ct[16] = {
        0x8d, 0x14, 0x70, 0x62, 0x5f, 0x59, 0xeb, 0xac, 0xb0, 0xe5, 0x5b, 0x53, 0x4b, 0x3e, 0x46, 0x2b
    };

    uint8_t encrypted[16] = {0,};
    uint8_t decrypted[16] = {0,};

    uint8_t rks[4 * 6 * 28] = {0};

    aria192_expand_key_enc(rks, key);
    aria192_encrypt(encrypted, pt, rks);

    aria192_expand_key_dec(rks, key);
    aria192_decrypt(decrypted, ct, rks);

    print_result("aria192", pt, encrypted, ct, decrypted);
}

void test_aria256() 
{
    uint8_t key[32] = {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
    };

    uint8_t pt[16] = {
        0x11, 0x11, 0x11, 0x11, 0xaa, 0xaa, 0xaa, 0xaa, 0x11, 0x11, 0x11, 0x11, 0xbb, 0xbb, 0xbb, 0xbb
    };

    uint8_t ct[16] = {
        0x58, 0xa8, 0x75, 0xe6, 0x04, 0x4a, 0xd7, 0xff, 0xfa, 0x4f, 0x58, 0x42, 0x0f, 0x7f, 0x44, 0x2d,

    };
    
    uint8_t encrypted[16] = {0,};
    uint8_t decrypted[16] = {0,};

    uint8_t rks[4 * 6 * 32] = {0};

    aria256_expand_key_enc(rks, key);
    aria256_encrypt(encrypted, pt, rks);

    aria256_expand_key_dec(rks, key);
    aria256_decrypt(decrypted, ct, rks);

    print_result("aria256", pt, encrypted, ct, decrypted);
}

int main()
{
    test_aria128();
    test_aria192();
    test_aria256();

    return 0;
}