#include "lsh.h"
#include <stdio.h>

void print_hex(const uint8_t* data, size_t count)
{
    for (size_t i = 0; i < count; ++i) {
        printf("%02x", data[i]);

        if ( (i + 1) % 4 == 0) {
            printf(" ");
        }

        if ( (i + 1) % 32 == 0) {
            printf("\n");
        }
    }

    printf("\n");
}

void test_lsh256()
{
    uint8_t digest[32] = {0, };
    uint8_t data[384] = {0, };

    for (size_t i = 0; i < 384; ++i) {
        data[i] = (uint8_t) (i & 0xff);
    }

    lsh256_context ctx;
    lsh256_init(&ctx);
    lsh256_update(&ctx, data, 384);
    lsh256_final(&ctx, digest);

    print_hex(digest, 32);
}

void test_lsh512()
{
    uint8_t digest[64] = {0, };
    uint8_t data[384] = {0, };

    for (size_t i = 0; i < 384; ++i) {
        data[i] = (uint8_t) (i & 0xff);
    }

    lsh512_context ctx;
    lsh512_init(&ctx);
    lsh512_update(&ctx, data, 256);
    lsh512_final(&ctx, digest);

    print_hex(digest, 64);
}

int main()
{
    test_lsh256();
    test_lsh512();
    
    return 0;
}