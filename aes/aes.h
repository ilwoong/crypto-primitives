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

#ifndef __CRYPTO_PRIMITIVES_AES_H__
#define __CRYPTO_PRIMITIVES_AES_H__

#include <stdint.h>
#include <stddef.h>

#define AES128_ROUNDS 10
#define AES192_ROUNDS 12
#define AES256_ROUNDS 14

void aes128_keygen(uint8_t* rks, const uint8_t* mk);
void aes128_encrypt(uint8_t* dst, const uint8_t* src, const uint8_t* rks);
void aes128_decrypt(uint8_t* dst, const uint8_t* src, const uint8_t* rks);

void aes192_keygen(uint8_t* rks, const uint8_t* mk);
void aes192_encrypt(uint8_t* dst, const uint8_t* src, const uint8_t* rks);
void aes192_decrypt(uint8_t* dst, const uint8_t* src, const uint8_t* rks);

void aes256_keygen(uint8_t* rks, const uint8_t* mk);
void aes256_encrypt(uint8_t* dst, const uint8_t* src, const uint8_t* rks);
void aes256_decrypt(uint8_t* dst, const uint8_t* src, const uint8_t* rks);

#endif