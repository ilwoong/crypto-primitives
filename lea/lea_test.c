/**
 * The MIT License
 * 
 * Copyright (c) <2018> <Ilwoong Jeong, ilwoong.jeong@gmail.com>
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

void test_lea128() 
{
    uint8_t key[16] = {0x0f, 0x1e, 0x2d, 0x3c, 0x4b, 0x5a, 0x69, 0x78, 0x87, 0x96, 0xa5, 0xb4, 0xc3, 0xd2, 0xe1, 0xf0};
    uint8_t pt[16] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f};
    uint8_t ct[16] = {0x9f, 0xc8, 0x4e, 0x35, 0x28, 0xc6, 0xc6, 0x18, 0x55, 0x32, 0xc7, 0xa7, 0x04, 0x64, 0x8b, 0xfd};
    uint8_t encrypted[16] = {0,};
    uint8_t decrypted[16] = {0,};

    uint8_t rks[4 * 6 * 24] = {0};

    lea128_keygen(rks, key);
    lea128_encrypt(encrypted, pt, rks);
    lea128_decrypt(decrypted, ct, rks);

    print_result("LEA128", pt, encrypted, ct, decrypted);
}

void test_lea192() 
{
    uint8_t key[24] = {
        0x0f, 0x1e, 0x2d, 0x3c, 0x4b, 0x5a, 0x69, 0x78, 0x87, 0x96, 0xa5, 0xb4, 0xc3, 0xd2, 0xe1, 0xf0,
        0xf0, 0xe1, 0xd2, 0xc3, 0xb4, 0xa5, 0x96, 0x87,
    };
    uint8_t pt[16] = {0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f};
    uint8_t ct[16] = {0x6f, 0xb9, 0x5e, 0x32, 0x5a, 0xad, 0x1b, 0x87, 0x8c, 0xdc, 0xf5, 0x35, 0x76, 0x74, 0xc6, 0xf2};
    uint8_t encrypted[16] = {0,};
    uint8_t decrypted[16] = {0,};

    uint8_t rks[4 * 6 * 28] = {0};

    lea192_keygen(rks, key);
    lea192_encrypt(encrypted, pt, rks);
    lea192_decrypt(decrypted, ct, rks);

    print_result("LEA192", pt, encrypted, ct, decrypted);
}

void test_lea256() 
{
    uint8_t key[32] = {
        0x0f, 0x1e, 0x2d, 0x3c, 0x4b, 0x5a, 0x69, 0x78, 0x87, 0x96, 0xa5, 0xb4, 0xc3, 0xd2, 0xe1, 0xf0,
        0xf0, 0xe1, 0xd2, 0xc3, 0xb4, 0xa5, 0x96, 0x87, 0x78, 0x69, 0x5a, 0x4b, 0x3c, 0x2d, 0x1e, 0x0f,
    };
    uint8_t pt[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f};
    uint8_t ct[16] = {0xd6, 0x51, 0xaf, 0xf6, 0x47, 0xb1, 0x89, 0xc1, 0x3a, 0x89, 0x00, 0xca, 0x27, 0xf9, 0xe1, 0x97};
    uint8_t encrypted[16] = {0,};
    uint8_t decrypted[16] = {0,};

    uint8_t rks[4 * 6 * 32] = {0};

    lea256_keygen(rks, key);
    lea256_encrypt(encrypted, pt, rks);
    lea256_decrypt(decrypted, ct, rks);

    print_result("LEA256", pt, encrypted, ct, decrypted);
}

int main()
{
    test_lea128();
    test_lea192();
    test_lea256();

    return 0;
}