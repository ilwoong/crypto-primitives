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

#include "aria.h"
#include "aria_sbox.h"
#include <string.h>

const static size_t BLOCKSIZE = 16;
const static size_t ARIA128_ROUNDS = 12;
const static size_t ARIA192_ROUNDS = 14;
const static size_t ARIA256_ROUNDS = 16;

static const uint8_t CK[3][16] = {
    {0x51, 0x7c, 0xc1, 0xb7, 0x27, 0x22, 0x0a, 0x94, 0xfe, 0x13, 0xab, 0xe8, 0xfa, 0x9a, 0x6e, 0xe0}, 
    {0x6d, 0xb1, 0x4a, 0xcc, 0x9e, 0x21, 0xc8, 0x20, 0xff, 0x28, 0xb1, 0xd5, 0xef, 0x5d, 0xe2, 0xb0}, 
    {0xdb, 0x92, 0x37, 0x1d, 0x21, 0x26, 0xe9, 0x70, 0x03, 0x24, 0x97, 0x75, 0x04, 0xe8, 0xc9, 0x0e},
};

static inline void bitwise_xor(uint8_t* out, const uint8_t* in, size_t count)
{
    for (size_t i = 0; i < count; ++i) {
        out[i] ^= in[i];
    }
}

static inline void add_round_key(uint8_t* block, const uint8_t* rk)
{
    for (int i = 0; i < 16; ++i)
    {
        block[i] ^= rk[i];
    }
}

static inline void substitute_odd(uint8_t* block)
{
    for (int i = 0; i < 16; ++i) {
        block[i] = ARIA_SBOX[i & 3][block[i]];
    }
}

static inline void substitute_even(uint8_t* block)
{
    for (int i = 0; i < 16; ++i) {
        block[i] = ARIA_SBOX[(i + 2) & 3][block[i]];
    }
}

static void diffuse(uint8_t* in)
{
    uint8_t t[4] = {0};
    uint8_t tmp[16] = {0, };

    t[0] = in[3] ^ in[4] ^ in[9] ^ in[14];
    tmp[ 0] = in[ 6] ^ in[ 8] ^ in[13] ^ t[0];
    tmp[ 5] = in[ 1] ^ in[10] ^ in[15] ^ t[0];
    tmp[11] = in[ 2] ^ in[ 7] ^ in[12] ^ t[0];
    tmp[14] = in[ 0] ^ in[ 5] ^ in[11] ^ t[0];

    t[1] = in[2] ^ in[5] ^ in[8] ^ in[15];
    tmp[ 1] = in[ 7] ^ in[ 9] ^ in[12] ^ t[1];
    tmp[ 4] = in[ 0] ^ in[11] ^ in[14] ^ t[1];
    tmp[10] = in[ 3] ^ in[ 6] ^ in[13] ^ t[1];
    tmp[15] = in[ 1] ^ in[ 4] ^ in[10] ^ t[1];

    t[2] = in[1] ^ in[6] ^ in[11] ^ in[12];
    tmp[ 2] = in[ 4] ^ in[10] ^ in[15] ^ t[2];
    tmp[ 7] = in[ 3] ^ in[ 8] ^ in[13] ^ t[2];
    tmp[ 9] = in[ 0] ^ in[ 5] ^ in[14] ^ t[2];
    tmp[12] = in[ 2] ^ in[ 7] ^ in[ 9] ^ t[2];

    t[3] = in[0] ^ in[7] ^ in[10] ^ in[13];
    tmp[ 3] = in[ 5] ^ in[11] ^ in[14] ^ t[3];
    tmp[ 6] = in[ 2] ^ in[ 9] ^ in[12] ^ t[3];
    tmp[ 8] = in[ 1] ^ in[ 4] ^ in[15] ^ t[3];
    tmp[13] = in[ 3] ^ in[ 6] ^ in[ 8] ^ t[3];

    memcpy(in, tmp, BLOCKSIZE);
}

static inline void round_odd(uint8_t* block, const uint8_t* rk)
{
    add_round_key(block, rk);
    substitute_odd(block);
    diffuse(block);
}

static inline void round_even(uint8_t* block, const uint8_t* rk) 
{
    add_round_key(block, rk);
    substitute_even(block);
    diffuse(block);
}

static inline void round_final(uint8_t* block, const uint8_t* rk)
{
    add_round_key(block, rk);
    rk += 16;

    substitute_even(block);
    add_round_key(block, rk);
}

static void xor_with_rotation(uint8_t* out, const uint8_t* lhs, const uint8_t* rhs, size_t rot)
{
    uint8_t rotated[16] = {0};

    size_t q = rot >> 3;
    rot &= 0x7;

    for (int i = 0; i < 16; ++i) {
        rotated[(q + i) & 0xf] ^= (rhs[i] >> rot);
        rotated[(q + i + 1) & 0xf] ^= (rhs[i] << (8 - rot));
    }

    for (int i = 0; i < 16; ++i) {
        out[i] = lhs[i] ^ rotated[i];
    }
}

static void init_key_material(uint8_t* w, const uint8_t* idx, const uint8_t* mk, size_t keysize)
{
    uint8_t kl[16] = {0, };
    uint8_t kr[16] = {0, };

    uint8_t* w0 = w;
    uint8_t* w1 = w0 + 16;
    uint8_t* w2 = w1 + 16;
    uint8_t* w3 = w2 + 16;

    memcpy(kl, mk, BLOCKSIZE);
    if (keysize > 16) {
        memcpy(kr, mk + 16, keysize - 16);
    }

    memcpy(w0, kl, BLOCKSIZE);

    memcpy(w1, w0, BLOCKSIZE);
    round_odd(w1, CK[idx[0]]);
    bitwise_xor(w1, kr, BLOCKSIZE);

    memcpy(w2, w1, BLOCKSIZE);
    round_even(w2, CK[idx[1]]);
    bitwise_xor(w2, w0, BLOCKSIZE);

    memcpy(w3, w2, BLOCKSIZE);
    round_odd(w3, CK[idx[2]]);
    bitwise_xor(w3, w1, BLOCKSIZE);
}

static void gen_4round_keys(uint8_t* out, uint8_t* w, size_t rot)
{
    uint8_t* w0 = w;
    uint8_t* w1 = w0 + 16;
    uint8_t* w2 = w1 + 16;
    uint8_t* w3 = w2 + 16;

    xor_with_rotation(out, w0, w1, rot);
    out += 16;

    xor_with_rotation(out, w1, w2, rot);
    out += 16;

    xor_with_rotation(out, w2, w3, rot);
    out += 16;

    xor_with_rotation(out, w3, w0, rot);
    out += 16;
}

static void aria_expand_key_dec(uint8_t* dec_rks, const uint8_t* enc_rks, size_t rounds)
{
    enc_rks += rounds * 16;

    memcpy(dec_rks, enc_rks, BLOCKSIZE);
    dec_rks += 16;
    enc_rks -= 16;

    for (int i = 1; i < rounds; ++i) {
        memcpy(dec_rks, enc_rks, BLOCKSIZE);
        diffuse(dec_rks);
        
        dec_rks += 16;
        enc_rks -= 16;
    }

    memcpy(dec_rks, enc_rks, BLOCKSIZE);
}

static inline void aria_encrypt(uint8_t* dst, const uint8_t* src, const uint8_t* rks, size_t rounds)
{
    const uint8_t* rk = rks;
    uint8_t block[16] = {0, };

    memcpy(block, src, BLOCKSIZE);

    for (int round = 0; round < rounds - 2; round += 2)
    {
        round_odd(block, rk);
        rk += 16;

        round_even(block, rk);
        rk += 16;
    }

    round_odd(block, rk);
    rk += 16;

    round_final(block, rk);

    memcpy(dst, block, BLOCKSIZE);
}

static inline void aria_decrypt(uint8_t* dst, const uint8_t* src, const uint8_t* rks, size_t rounds)
{
    aria_encrypt(dst, src, rks, rounds);
}

/**
 * ARIA key expansion for encryption with 128-bit key
 * 
 * @param[out] out ciphertext
 * @param[in] in plaintext
 * @param[in] rks round keys
 */ 
void aria128_expand_key_enc(uint8_t* rks, const uint8_t* mk)
{
    uint8_t idx[3] = {0, 1, 2};
    
    uint8_t w[16 * 4] = {0, };
    uint8_t* w0 = w;
    uint8_t* w1 = w0 + 16;
    uint8_t* w2 = w1 + 16;
    uint8_t* w3 = w2 + 16;

    init_key_material(w, idx, mk, 16);

    gen_4round_keys(rks, w, 19);
    rks += 4 * 16 ;

    gen_4round_keys(rks, w, 31);
    rks += 4 * 16 ;

    gen_4round_keys(rks, w, 128-61);
    rks += 4 * 16 ;
    
    // 13th
    xor_with_rotation(rks, w0, w1, 128-31); 
}

/**
 * ARIA key expansion for decryption with 128-bit key
 * 
 * @param[out] out ciphertext
 * @param[in] in plaintext
 * @param[in] rks round keys
 */ 
void aria128_expand_key_dec(uint8_t* rks, const uint8_t* mk)
{
    uint8_t tmp[13 * 16] = {0};
    aria128_expand_key_enc(tmp, mk);
    aria_expand_key_dec(rks, tmp, ARIA128_ROUNDS);
}

/**
 * ARIA encryption with 128-bit key
 * 
 * @param[out] out ciphertext
 * @param[in] in plaintext
 * @param[in] rks round keys
 */ 
void aria128_encrypt(uint8_t* dst, const uint8_t* src, const uint8_t* rks)
{
    aria_encrypt(dst, src, rks, ARIA128_ROUNDS);
}

/**
 * ARIA decryption with 128-bit key
 * 
 * @param[out] out plaintext
 * @param[in] in ciphertext
 * @param[in] rks round keys
 */ 
void aria128_decrypt(uint8_t* dst, const uint8_t* src, const uint8_t* rks)
{
    aria_decrypt(dst, src, rks, ARIA128_ROUNDS);
}

/**
 * ARIA key expansion for encryption with 192-bit key
 * 
 * @param[out] out ciphertext
 * @param[in] in plaintext
 * @param[in] rks round keys
 */ 
void aria192_expand_key_enc(uint8_t* rks, const uint8_t* mk)
{
    uint8_t idx[3] = {1, 2, 0};

    uint8_t w[16 * 4] = {0, };
    uint8_t* w0 = w;
    uint8_t* w1 = w0 + 16;
    uint8_t* w2 = w1 + 16;
    uint8_t* w3 = w2 + 16;

    init_key_material(w, idx, mk, 24);

    gen_4round_keys(rks, w, 19);
    rks += 4 * 16 ;

    gen_4round_keys(rks, w, 31);
    rks += 4 * 16 ;

    gen_4round_keys(rks, w, 128 - 61);
    rks += 4 * 16 ;
    
    // 13th
    xor_with_rotation(rks, w0, w1, 128 - 31);
    rks += 16;

    xor_with_rotation(rks, w1, w2, 128 - 31);
    rks += 16;

    xor_with_rotation(rks, w2, w3, 128 - 31);
    rks += 16;
}

/**
 * ARIA key expansion for decryption with 192-bit key
 * 
 * @param[out] out ciphertext
 * @param[in] in plaintext
 * @param[in] rks round keys
 */ 
void aria192_expand_key_dec(uint8_t* rks, const uint8_t* mk)
{
    uint8_t tmp[15 * 16] = {0};
    aria192_expand_key_enc(tmp, mk);
    aria_expand_key_dec(rks, tmp, ARIA192_ROUNDS);
}

/**
 * ARIA encryption with 192-bit key
 * 
 * @param[out] out ciphertext
 * @param[in] in plaintext
 * @param[in] rks round keys
 */ 
void aria192_encrypt(uint8_t* dst, const uint8_t* src, const uint8_t* rks)
{
    aria_encrypt(dst, src, rks, ARIA192_ROUNDS);
}

/**
 * ARIA decryption with 192-bit key
 * 
 * @param[out] out plaintext
 * @param[in] in ciphertext
 * @param[in] rks round keys
 */ 
void aria192_decrypt(uint8_t* dst, const uint8_t* src, const uint8_t* rks)
{
    aria_decrypt(dst, src, rks, ARIA192_ROUNDS);
}

/**
 * ARIA key expansion for encryption with 256-bit key
 * 
 * @param[out] out ciphertext
 * @param[in] in plaintext
 * @param[in] rks round keys
 */ 
void aria256_expand_key_enc(uint8_t* rks, const uint8_t* mk)
{
    uint8_t idx[3] = {2, 0, 1};

    uint8_t w[16 * 4] = {0, };
    uint8_t* w0 = w;
    uint8_t* w1 = w0 + 16;
    uint8_t* w2 = w1 + 16;
    uint8_t* w3 = w2 + 16;

    init_key_material(w, idx, mk, 32);

    gen_4round_keys(rks, w, 19);
    rks += 4 * 16 ;

    gen_4round_keys(rks, w, 31);
    rks += 4 * 16 ;

    gen_4round_keys(rks, w, 128 - 61);
    rks += 4 * 16 ;

    gen_4round_keys(rks, w, 128 - 31);
    rks += 4 * 16 ;
    
    // 17th
    xor_with_rotation(rks, w0, w1, 128-19); 
}

/**
 * ARIA key expansion for decryption with 256-bit key
 * 
 * @param[out] out ciphertext
 * @param[in] in plaintext
 * @param[in] rks round keys
 */ 
void aria256_expand_key_dec(uint8_t* rks, const uint8_t* mk)
{
    uint8_t tmp[17 * 16] = {0};
    aria256_expand_key_enc(tmp, mk);
    aria_expand_key_dec(rks, tmp, ARIA256_ROUNDS);
}

/**
 * ARIA encryption with 256-bit key
 * 
 * @param[out] out ciphertext
 * @param[in] in plaintext
 * @param[in] rks round keys
 */ 
void aria256_encrypt(uint8_t* dst, const uint8_t* src, const uint8_t* rks)
{
    aria_encrypt(dst, src, rks, ARIA256_ROUNDS);
}

/**
 * ARIA decryption with 256-bit key
 * 
 * @param[out] out plaintext
 * @param[in] in ciphertext
 * @param[in] rks round keys
 */ 
void aria256_decrypt(uint8_t* dst, const uint8_t* src, const uint8_t* rks)
{
    aria_decrypt(dst, src, rks, ARIA256_ROUNDS);
}
