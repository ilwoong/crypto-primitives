# crypto-primitives
C implementations of cryptograhpic algorithm privitives.

## Primitives

### AES
AES is a block cipher algorithm which supports 128, 192, and 256-bit key.

#### Implementations
* AES reference implementation
* AES lookup table implementation - sbox only
* AES lookup table implementation - sbox and mixcolumn indivisually
* AES lookup table implementation - sbox and mixcolumn together
* AES-NI implementation

### ARIA
ARIA is a block cipher algorithm which supports 128, 192, and 256-bit key.

#### Implementations
* ARIA reference implementation

### CHAM
CHAM is a family of block ciphers which consists of CHAM-64/128, CHAM-128/128, and CHAM-128/256.

#### Implementations
* C implementation

### LEA
LEA is a 128-bit block cipher algorithm which supports 128, 192, and 256-bit key.

#### Implementations
* C implementation
* SIMD implementation using AVX2

### LSH
LSH is a hash function family which consists of LSH-256 and LSH-512.

#### Implementations
* C implementation
* SIMD implementation using SSE4, and AVX2