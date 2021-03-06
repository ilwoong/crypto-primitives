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

#pragma once

#ifndef __CRYPTO_PRIMITIVES_LEA_H__
#define __CRYPTO_PRIMITIVES_LEA_H__

#include <stdint.h>
#include <stddef.h>

const static size_t LEA128_ROUNDS = 24;
const static size_t LEA192_ROUNDS = 28;
const static size_t LEA256_ROUNDS = 32;

void lea128_keygen(uint8_t* out, const uint8_t* mk);
void lea192_keygen(uint8_t* out, const uint8_t* mk);
void lea256_keygen(uint8_t* out, const uint8_t* mk);

void lea128_encrypt(uint8_t* out, const uint8_t* in, const uint8_t* rks);
void lea128_decrypt(uint8_t* out, const uint8_t* in, const uint8_t* rks);

void lea192_encrypt(uint8_t* out, const uint8_t* in, const uint8_t* rks);
void lea192_decrypt(uint8_t* out, const uint8_t* in, const uint8_t* rks);

void lea256_encrypt(uint8_t* out, const uint8_t* in, const uint8_t* rks);
void lea256_decrypt(uint8_t* out, const uint8_t* in, const uint8_t* rks);

#endif