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

#include "ecb.h"

void ecb_encrypt(uint8_t* ct, const uint8_t* pt, const uint8_t* rks, size_t blocksize, size_t length, void(*encrypt)(uint8_t*, const uint8_t*, const uint8_t*))
{
    while (length >= blocksize) {
        encrypt(ct, pt, rks);
        
        pt += blocksize;
        ct += blocksize;
        length -= blocksize;
    }
}

void ecb_decrypt(uint8_t* pt, const uint8_t* ct, const uint8_t* rks, size_t blocksize, size_t length, void(*decrypt)(uint8_t*, const uint8_t*, const uint8_t*))
{
    while (length >= blocksize) {
        decrypt(pt, ct, rks);
        
        ct += blocksize;
        pt += blocksize;
        length -= blocksize;
    }
}