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

#include <stdint.h>
#include <stddef.h>

typedef struct st_lsh256_context {
    size_t bidx;
    size_t length;
    uint8_t block[128];
    uint32_t cv[16];
    uint32_t tcv[16];
    uint32_t msg[16 * (26 + 1)];
} lsh256_context;

typedef struct st_lsh512_context {
    size_t bidx;
    size_t length;
    uint8_t block[256];
    uint64_t cv[16];
    uint64_t tcv[16];
    uint64_t msg[16 * (28 + 1)];
} lsh512_context;

void lsh256_init(lsh256_context* ctx);
void lsh256_update(lsh256_context* ctx, const uint8_t* data, size_t length);
void lsh256_final(lsh256_context* ctx, uint8_t* digest);

void lsh512_init(lsh512_context* ctx);
void lsh512_update(lsh512_context* ctx, const uint8_t* data, size_t length);
void lsh512_final(lsh512_context* ctx, uint8_t* digest);
