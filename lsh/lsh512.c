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

#include "lsh.h"
#include <string.h>

const static size_t NUMSTEP = 28;

const static size_t BLOCKSIZE = 256;

const static uint64_t IV[] = {
    0xadd50f3c7f07094eL, 0xe3f3cee8f9418a4fL, 0xb527ecde5b3d0ae9L, 0x2ef6dec68076f501L,
    0x8cb994cae5aca216L, 0xfbb9eae4bba48cc7L, 0x650a526174725feaL, 0x1f9a61a73f8d8085L,
    0xb6607378173b539bL, 0x1bc99853b0c0b9edL, 0xdf727fc19b182d47L, 0xdbef360cf893a457L,
    0x4981f5e570147e80L, 0xd00c4490ca7d3e30L, 0x5d73940c0e4ae1ecL, 0x894085e2edb2d819L,
};

const static uint64_t STEP_CONSTANT[] = {
    0x97884283c938982aL, 0xba1fca93533e2355L, 0xc519a2e87aeb1c03L, 0x9a0fc95462af17b1L,
    0xfc3dda8ab019a82bL, 0x02825d079a895407L, 0x79f2d0a7ee06a6f7L, 0xd76d15eed9fdf5feL,
    0x1fcac64d01d0c2c1L, 0xd9ea5de69161790fL, 0xdebc8b6366071fc8L, 0xa9d91db711c6c94bL,
    0x3a18653ac9c1d427L, 0x84df64a223dd5b09L, 0x6cc37895f4ad9e70L, 0x448304c8d7f3f4d5L,
    0xea91134ed29383e0L, 0xc4484477f2da88e8L, 0x9b47eec96d26e8a6L, 0x82f6d4c8d89014f4L,
    0x527da0048b95fb61L, 0x644406c60138648dL, 0x303c0e8aa24c0edcL, 0xc787cda0cbe8ca19L,
    0x7ba46221661764caL, 0x0c8cbc6acd6371acL, 0xe336b836940f8f41L, 0x79cb9da168a50976L,
    0xd01da49021915cb3L, 0xa84accc7399cf1f1L, 0x6c4a992cee5aeb0cL, 0x4f556e6cb4b2e3e0L,
    0x200683877d7c2f45L, 0x9949273830d51db8L, 0x19eeeecaa39ed124L, 0x45693f0a0dae7fefL,
    0xedc234b1b2ee1083L, 0xf3179400d68ee399L, 0xb6e3c61b4945f778L, 0xa4c3db216796c42fL,
    0x268a0b04f9ab7465L, 0xe2705f6905f2d651L, 0x08ddb96e426ff53dL, 0xaea84917bc2e6f34L,
    0xaff6e664a0fe9470L, 0x0aab94d765727d8cL, 0x9aa9e1648f3d702eL, 0x689efc88fe5af3d3L,
    0xb0950ffea51fd98bL, 0x52cfc86ef8c92833L, 0xe69727b0b2653245L, 0x56f160d3ea9da3e2L,
    0xa6dd4b059f93051fL, 0xb6406c3cd7f00996L, 0x448b45f3ccad9ec8L, 0x079b8587594ec73bL,
    0x45a50ea3c4f9653bL, 0x22983767c1f15b85L, 0x7dbed8631797782bL, 0x485234be88418638L,
    0x842850a5329824c5L, 0xf6aca914c7f9a04cL, 0xcfd139c07a4c670cL, 0xa3210ce0a8160242L,
    0xeab3b268be5ea080L, 0xbacf9f29b34ce0a7L, 0x3c973b7aaf0fa3a8L, 0x9a86f346c9c7be80L,
    0xac78f5d7cabcea49L, 0xa355bddcc199ed42L, 0xa10afa3ac6b373dbL, 0xc42ded88be1844e5L,
    0x9e661b271cff216aL, 0x8a6ec8dd002d8861L, 0xd3d2b629beb34be4L, 0x217a3a1091863f1aL,
    0x256ecda287a733f5L, 0xf9139a9e5b872fe5L, 0xac0535017a274f7cL, 0xf21b7646d65d2aa9L,
    0x048142441c208c08L, 0xf937a5dd2db5e9ebL, 0xa688dfe871ff30b7L, 0x9bb44aa217c5593bL,
    0x943c702a2edb291aL, 0x0cae38f9e2b715deL, 0xb13a367ba176cc28L, 0x0d91bd1d3387d49bL,
    0x85c386603cac940cL, 0x30dd830ae39fd5e4L, 0x2f68c85a712fe85dL, 0x4ffeecb9dd1e94d6L,
    0xd0ac9a590a0443aeL, 0xbae732dc99ccf3eaL, 0xeb70b21d1842f4d9L, 0x9f4eda50bb5c6fa8L,
    0x4949e69ce940a091L, 0x0e608dee8375ba14L, 0x983122cba118458cL, 0x4eeba696fbb36b25L,
    0x7d46f3630e47f27eL, 0xa21a0f7666c0dea4L, 0x5c22cf355b37cec4L, 0xee292b0c17cc1847L,
    0x9330838629e131daL, 0x6eee7c71f92fce22L, 0xc953ee6cb95dd224L, 0x3a923d92af1e9073L,
    0xc43a5671563a70fbL, 0xbc2985dd279f8346L, 0x7ef2049093069320L, 0x17543723e3e46035L,
    0xc3b409b00b130c6dL, 0x5d6aee6b28fdf090L, 0x1d425b26172ff6edL, 0xcccfd041cdaf03adL,
    0xfe90c7c790ab6cbfL, 0xe5af6304c722ca02L, 0x70f695239999b39eL, 0x6b8b5b07c844954cL,
    0x77bdb9bb1e1f7a30L, 0xc859599426ee80edL, 0x5f9d813d4726e40aL, 0x9ca0120f7cb2b179L,
    0x8f588f583c182cbdL, 0x951267cbe9eccce7L, 0x678bb8bd334d520eL, 0xf6e662d00cd9e1b7L,
    0x357774d93d99aaa7L, 0x21b2edbb156f6eb5L, 0xfd1ebe846e0aee69L, 0x3cb2218c2f642b15L,
    0xe7e7e7945444ea4cL, 0xa77a33b5d6b9b47cL, 0xf34475f0809f6075L, 0xdd4932dce6bb99adL,
    0xacec4e16d74451dcL, 0xd4a0a8d084de23d6L, 0x1bdd42f278f95866L, 0xeed3adbb938f4051L,
    0xcfcf7be8992f3733L, 0x21ade98c906e3123L, 0x37ba66711fffd668L, 0x267c0fc3a255478aL,
    0x993a64ee1b962e88L, 0x754979556301faaaL, 0xf920356b7251be81L, 0xc281694f22cf923fL,
    0x9f4b6481c8666b02L, 0xcf97761cfe9f5444L, 0xf220d7911fd63e9fL, 0xa28bd365f79cd1b0L,
    0xd39f5309b1c4b721L, 0xbec2ceb864fca51fL, 0x1955a0ddc410407aL, 0x43eab871f261d201L,
    0xeaafe64a2ed16da1L, 0x670d931b9df39913L, 0x12f868b0f614de91L, 0x2e5f395d946e8252L,
    0x72f25cbb767bd8f4L, 0x8191871d61a1c4ddL, 0x6ef67ea1d450ba93L, 0x2ea32a645433d344L,
    0x9a963079003f0f8bL, 0x74a0aeb9918cac7aL, 0x0b6119a70af36fa3L, 0x8d9896f202f0d480L,
    0x654f1831f254cd66L, 0x1318a47f0366a25eL, 0x65752076250b4e01L, 0xd1cd8eb888071772L,
    0x30c6a9793f4e9b25L, 0x154f684b1e3926eeL, 0x6c7ac0b1fe6312aeL, 0x262f88f4f3c5550dL,
    0xb4674a24472233cbL, 0x2bbd23826a090071L, 0xda95969b30594f66L, 0x9f5c47408f1e8a43L,
    0xf77022b88de9c055L, 0x64b7b36957601503L, 0xe73b72b06175c11aL, 0x55b87de8b91a6233L,
    0x1bb16e6b6955ff7fL, 0xe8e0a5ec7309719cL, 0x702c31cb89a8b640L, 0xfba387cfada8cde2L,
    0x6792db4677aa164cL, 0x1c6b1cc0b7751867L, 0x22ae2311d736dc01L, 0x0e3666a1d37c9588L,
    0xcd1fd9d4bf557e9aL, 0xc986925f7c7b0e84L, 0x9c5dfd55325ef6b0L, 0x9f2b577d5676b0ddL,
    0xfa6e21be21c062b3L, 0x8787dd782c8d7f83L, 0xd0d134e90e12dd23L, 0x449d087550121d96L,
    0xecf9ae9414d41967L, 0x5018f1dbf789934dL, 0xfa5b52879155a74cL, 0xca82d4d3cd278e7cL,
    0x688fdfdfe22316adL, 0x0f6555a4ba0d030aL, 0xa2061df720f000f3L, 0xe1a57dc5622fb3daL,
    0xe6a842a8e8ed8153L, 0x690acdd3811ce09dL, 0x55adda18e6fcf446L, 0x4d57a8a0f4b60b46L,
    0xf86fbfc20539c415L, 0x74bafa5ec7100d19L, 0xa824151810f0f495L, 0x8723432791e38ebbL,
    0x8eeaeb91d66ed539L, 0x73d8a1549dfd7e06L, 0x0387f2ffe3f13a9bL, 0xa5004995aac15193L,
    0x682f81c73efdda0dL, 0x2fb55925d71d268dL, 0xcc392d2901e58a3dL, 0xaa666ab975724a42L,
};

const static uint64_t ALPHA_EVEN = 23;
const static uint64_t ALPHA_ODD = 7;

const static uint64_t BETA_EVEN = 59;
const static uint64_t BETA_ODD= 3;

const static uint64_t GAMMA[] = {0, 16, 32, 48, 8, 24, 40, 56};

static inline uint64_t rol64(uint64_t value, size_t rot) 
{
    return (value << rot) | (value >> (64 - rot));
}

static void expand_message(lsh512_context* ctx, const uint8_t* in)
{
    uint64_t* msg = ctx->msg;
    memcpy(msg, in, 32 * sizeof(uint64_t));

    for (size_t i = 2; i <= NUMSTEP; ++i) {
        size_t idx = 16 * i;
        msg[idx     ] = msg[idx - 16] + msg[idx - 29];
        msg[idx +  1] = msg[idx - 15] + msg[idx - 30];

        msg[idx +  2] = msg[idx - 14] + msg[idx - 32];
        msg[idx +  3] = msg[idx - 13] + msg[idx - 31];

        msg[idx +  4] = msg[idx - 12] + msg[idx - 25];
        msg[idx +  5] = msg[idx - 11] + msg[idx - 28];

        msg[idx +  6] = msg[idx - 10] + msg[idx - 27];
        msg[idx +  7] = msg[idx -  9] + msg[idx - 26];

        msg[idx +  8] = msg[idx -  8] + msg[idx - 21];
        msg[idx +  9] = msg[idx -  7] + msg[idx - 22];

        msg[idx + 10] = msg[idx -  6] + msg[idx - 24];
        msg[idx + 11] = msg[idx -  5] + msg[idx - 23];

        msg[idx + 12] = msg[idx -  4] + msg[idx - 17];
        msg[idx + 13] = msg[idx -  3] + msg[idx - 20];

        msg[idx + 14] = msg[idx -  2] + msg[idx - 19];
        msg[idx + 15] = msg[idx -  1] + msg[idx - 18];
    }
}

static inline void permute_word(lsh512_context* ctx)
{
    ctx->cv[ 0] = ctx->tcv[ 6];
    ctx->cv[ 1] = ctx->tcv[ 4];
    ctx->cv[ 2] = ctx->tcv[ 5];
    ctx->cv[ 3] = ctx->tcv[ 7];
    
    ctx->cv[ 4] = ctx->tcv[12];
    ctx->cv[ 5] = ctx->tcv[15];
    ctx->cv[ 6] = ctx->tcv[14];
    ctx->cv[ 7] = ctx->tcv[13];

    ctx->cv[ 8] = ctx->tcv[ 2];
    ctx->cv[ 9] = ctx->tcv[ 0];
    ctx->cv[10] = ctx->tcv[ 1];
    ctx->cv[11] = ctx->tcv[ 3];

    ctx->cv[12] = ctx->tcv[ 8];
    ctx->cv[13] = ctx->tcv[11];
    ctx->cv[14] = ctx->tcv[10];
    ctx->cv[15] = ctx->tcv[ 9];
}

static void step(lsh512_context* ctx, size_t idx, uint64_t alpha, uint64_t beta)
{
    uint64_t vl, vr;
    for (size_t col = 0; col < 8; ++col) {
        vl = ctx->cv[col    ] ^ ctx->msg[16 * idx + col    ];
        vr = ctx->cv[col + 8] ^ ctx->msg[16 * idx + col + 8];

        vl = rol64(vl + vr, alpha) ^ STEP_CONSTANT[8 * idx + col];
        vr = rol64(vl + vr, beta);
        
        ctx->tcv[col    ] = vl + vr;
        ctx->tcv[col + 8] = rol64(vr, GAMMA[col]);
    }

    permute_word(ctx);
}

static void compress(lsh512_context* ctx, const uint8_t* data)
{
    expand_message(ctx, data);

    for (size_t i = 0; i < NUMSTEP; i += 2) {
        step(ctx, i, ALPHA_EVEN, BETA_EVEN);
        step(ctx, i + 1, ALPHA_ODD, BETA_ODD);
    }

    for (size_t i = 0; i < 16; ++i) {
        ctx->cv[i] ^= ctx->msg[16 * NUMSTEP + i];
    }

}

void lsh512_init(lsh512_context* ctx)
{
    ctx->bidx = 0;
    ctx->length = 0;
    memset(ctx->block, 0, BLOCKSIZE);

    memcpy(ctx->cv, IV, 16 * sizeof(uint64_t));
    memset(ctx->tcv, 0, 16 * sizeof(uint64_t));
    memset(ctx->msg, 0, 16 * (NUMSTEP + 1) * sizeof(uint64_t));
}

void lsh512_update(lsh512_context* ctx, const uint8_t* data, size_t length)
{
    ctx->length += length;

    if (ctx->bidx > 0) {
        size_t gap = BLOCKSIZE - ctx->bidx;

        if (length >= gap) {
            memcpy(ctx->block + ctx->bidx, data, gap);            
            compress(ctx, ctx->block);
            ctx->bidx = 0;
            data += gap;
            length -= gap;

        } else {
            memcpy(ctx->block + ctx->bidx, data, length);
            ctx->bidx += length;
            data += length;
            length = 0;
        }
    }
    
    while (length >= BLOCKSIZE) {
        compress(ctx, data);
        data += BLOCKSIZE;
        length -= BLOCKSIZE;
    }

    if (length > 0) {
        memcpy(ctx->block + ctx->bidx, data, length);
        ctx->bidx += length;
    }
}

void lsh512_final(lsh512_context* ctx, uint8_t* digest)
{
    uint64_t* result = (uint64_t*) digest;

    ctx->block[(ctx->bidx)++] = (uint8_t) 0x80;
    memset(ctx->block + ctx->bidx, 0, BLOCKSIZE - ctx->bidx);
    compress(ctx, ctx->block);

    for (size_t i = 0; i < 8; ++i) {
        result[i] = ctx->cv[i] ^ ctx->cv[i + 8];
    }

    lsh512_init(ctx);
}
