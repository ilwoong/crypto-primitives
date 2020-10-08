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

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

const uint8_t SBOX1[] = {
    0xa9, 0x85, 0xd6, 0xd3, 0x54, 0x1d, 0xac, 0x25, 0x5d, 0x43, 0x18, 0x1e, 0x51, 0xfc, 0xca, 0x63, 
    0x28, 0x44, 0x20, 0x9d, 0xe0, 0xe2, 0xc8, 0x17, 0xa5, 0x8f, 0x03, 0x7b, 0xbb, 0x13, 0xd2, 0xee,
    0x70, 0x8c, 0x3f, 0xa8, 0x32, 0xdd, 0xf6, 0x74, 0xec, 0x95, 0x0b, 0x57, 0x5c, 0x5b, 0xbd, 0x01,
    0x24, 0x1c, 0x73, 0x98, 0x10, 0xcc, 0xf2, 0xd9, 0x2c, 0xe7, 0x72, 0x83, 0x9b, 0xd1, 0x86, 0xc9,
    0x60, 0x50, 0xa3, 0xeb, 0x0d, 0xb6, 0x9e, 0x4f, 0xb7, 0x5a, 0xc6, 0x78, 0xa6, 0x12, 0xaf, 0xd5,
    0x61, 0xc3, 0xb4, 0x41, 0x52, 0x7d, 0x8d, 0x08, 0x1f, 0x99, 0x00, 0x19, 0x04, 0x53, 0xf7, 0xe1,
    0xfd, 0x76, 0x2f, 0x27, 0xb0, 0x8b, 0x0e, 0xab, 0xa2, 0x6e, 0x93, 0x4d, 0x69, 0x7c, 0x09, 0x0a,
    0xbf, 0xef, 0xf3, 0xc5, 0x87, 0x14, 0xfe, 0x64, 0xde, 0x2e, 0x4b, 0x1a, 0x06, 0x21, 0x6b, 0x66,
    0x02, 0xf5, 0x92, 0x8a, 0x0c, 0xb3, 0x7e, 0xd0, 0x7a, 0x47, 0x96, 0xe5, 0x26, 0x80, 0xad, 0xdf,
    0xa1, 0x30, 0x37, 0xae, 0x36, 0x15, 0x22, 0x38, 0xf4, 0xa7, 0x45, 0x4c, 0x81, 0xe9, 0x84, 0x97,
    0x35, 0xcb, 0xce, 0x3c, 0x71, 0x11, 0xc7, 0x89, 0x75, 0xfb, 0xda, 0xf8, 0x94, 0x59, 0x82, 0xc4, 
    0xff, 0x49, 0x39, 0x67, 0xc0, 0xcf, 0xd7, 0xb8, 0x0f, 0x8e, 0x42, 0x23, 0x91, 0x6c, 0xdb, 0xa4,
    0x34, 0xf1, 0x48, 0xc2, 0x6f, 0x3d, 0x2d, 0x40, 0xbe, 0x3e, 0xbc, 0xc1, 0xaa, 0xba, 0x4e, 0x55,
    0x3b, 0xdc, 0x68, 0x7f, 0x9c, 0xd8, 0x4a, 0x56, 0x77, 0xa0, 0xed, 0x46, 0xb5, 0x2b, 0x65, 0xfa,
    0xe3, 0xb9, 0xb1, 0x9f, 0x5e, 0xf9, 0xe6, 0xb2, 0x31, 0xea, 0x6d, 0x5f, 0xe4, 0xf0, 0xcd, 0x88,
    0x16, 0x3a, 0x58, 0xd4, 0x62, 0x29, 0x07, 0x33, 0xe8, 0x1b, 0x05, 0x79, 0x90, 0x6a, 0x2a, 0x9a,
};

const uint8_t SBOX2[] = {
    0x38, 0xe8, 0x2d, 0xa6, 0xcf, 0xde, 0xb3, 0xb8, 0xaf, 0x60, 0x55, 0xc7, 0x44, 0x6f, 0x6b, 0x5b, 
    0xc3, 0x62, 0x33, 0xb5, 0x29, 0xa0, 0xe2, 0xa7, 0xd3, 0x91, 0x11, 0x06, 0x1c, 0xbc, 0x36, 0x4b,
    0xef, 0x88, 0x6c, 0xa8, 0x17, 0xc4, 0x16, 0xf4, 0xc2, 0x45, 0xe1, 0xd6, 0x3f, 0x3d, 0x8e, 0x98,
    0x28, 0x4e, 0xf6, 0x3e, 0xa5, 0xf9, 0x0d, 0xdf, 0xd8, 0x2b, 0x66, 0x7a, 0x27, 0x2f, 0xf1, 0x72,
    0x42, 0xd4, 0x41, 0xc0, 0x73, 0x67, 0xac, 0x8b, 0xf7, 0xad, 0x80, 0x1f, 0xca, 0x2c, 0xaa, 0x34,
    0xd2, 0x0b, 0xee, 0xe9, 0x5d, 0x94, 0x18, 0xf8, 0x57, 0xae, 0x08, 0xc5, 0x13, 0xcd, 0x86, 0xb9,
    0xff, 0x7d, 0xc1, 0x31, 0xf5, 0x8a, 0x6a, 0xb1, 0xd1, 0x20, 0xd7, 0x02, 0x22, 0x04, 0x68, 0x71,
    0x07, 0xdb, 0x9d, 0x99, 0x61, 0xbe, 0xe6, 0x59, 0xdd, 0x51, 0x90, 0xdc, 0x9a, 0xa3, 0xab, 0xd0,
    0x81, 0x0f, 0x47, 0x1a, 0xe3, 0xec, 0x8d, 0xbf, 0x96, 0x7b, 0x5c, 0xa2, 0xa1, 0x63, 0x23, 0x4d,
    0xc8, 0x9e, 0x9c, 0x3a, 0x0c, 0x2e, 0xba, 0x6e, 0x9f, 0x5a, 0xf2, 0x92, 0xf3, 0x49, 0x78, 0xcc,
    0x15, 0xfb, 0x70, 0x75, 0x7f, 0x35, 0x10, 0x03, 0x64, 0x6d, 0xc6, 0x74, 0xd5, 0xb4, 0xea, 0x09,
    0x76, 0x19, 0xfe, 0x40, 0x12, 0xe0, 0xbd, 0x05, 0xfa, 0x01, 0xf0, 0x2a, 0x5e, 0xa9, 0x56, 0x43,
    0x85, 0x14, 0x89, 0x9b, 0xb0, 0xe5, 0x48, 0x79, 0x97, 0xfc, 0x1e, 0x82, 0x21, 0x8c, 0x1b, 0x5f,
    0x77, 0x54, 0xb2, 0x1d, 0x25, 0x4f, 0x00, 0x46, 0xed, 0x58, 0x52, 0xeb, 0x7e, 0xda, 0xc9, 0xfd,
    0x30, 0x95, 0x65, 0x3c, 0xb6, 0xe4, 0xbb, 0x7c, 0x0e, 0x50, 0x39, 0x26, 0x32, 0x84, 0x69, 0x93,
    0x37, 0xe7, 0x24, 0xa4, 0xcb, 0x53, 0x0a, 0x87, 0xd9, 0x4c, 0x83, 0x8f, 0xce, 0x3b, 0x4a, 0xb7,
};

const uint8_t M[] = { 0xfc, 0xf3, 0xcf, 0x3f };

void printss(int idx, const uint8_t* sbox, const uint8_t* m) {
    printf("static const uint32_t SS%d[] = {\n    ", idx);
    for (uint16_t x = 0; x < 256; ++x) {
        uint8_t tmp = sbox[x];
        uint32_t value = 0;
        
        value |= (tmp & m[3]);
        value |= (tmp & m[2]) << 8;
        value |= (tmp & m[1]) << 16;
        value |= (tmp & m[0]) << 24;

        printf("0x%08x, ", value);

        if ((x & 0x7) == 0x7) {
            printf("\n");
            if (x != 255) {
                printf("    ");
            }
        }
    }    

    printf("};\n\n");
}

int main(int argc, const char** argv)
{
    uint8_t m3[] = {M[2], M[1], M[0], M[3]};
    uint8_t m2[] = {M[1], M[0], M[3], M[2]};
    uint8_t m1[] = {M[0], M[3], M[2], M[1]};
    uint8_t m0[] = {M[3], M[2], M[1], M[0]};
    
    printf("// lookup tables for G function of SEED algorithm\n\n");
    printss(0, SBOX1, m0);
    printss(1, SBOX2, m1);
    printss(2, SBOX1, m2);
    printss(3, SBOX2, m3);
    
    return 0;
}