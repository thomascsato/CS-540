#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/* Macro for bitwise choice
   Same logic as boolean choice, but converted to bitwise operators. */
#define CHOICE(e, f, g) (((e) & (f)) ^ (~(e) & (g)))

/* Macro for bitwise median
   If any pair of the three inputs are both 1, then the median will be 1. Then do it bitwise. */
#define MEDIAN(e, f, g) (((e) & (f)) | ((f) & (g)) | ((g) & (e)))

/* Macro for bitwise right rotate
   x >> n shifts the bits to the right
   x << sizeof(x)*8 - n wraps the digits from the right back around to the left
   | puts them together so that the n leftmost digits that needed to be wrapped around are where they should be
   and the bits shifted to the right are where they should be. */
#define ROTATE(x, n) (((x) >> (n)) | ((x) << (sizeof(x) * 8 - (n))))

/* Other macros for processing */
#define SIG0(x) (ROTATE(x, 7) ^ ROTATE(x, 18) ^ (x >> 3))
#define SIG1(x) (ROTATE(x, 17) ^ ROTATE(x, 19) ^ (x >> 10))
#define BIG_SIG0(x) (ROTATE(x, 2) ^ ROTATE(x, 13) ^ ROTATE(x, 22))
#define BIG_SIG1(x) (ROTATE(x, 6) ^ ROTATE(x, 11) ^ ROTATE(x, 25))

/* initial hash values (h_0) */
/* network endian */ 
uint32_t H[8] = {
    0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,
    0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19
};

/* round constants */
/* network endian */
uint32_t K[64] = {
    0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,
    0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
    0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,
    0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
    0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,
    0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
    0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,
    0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
    0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,
    0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
    0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,
    0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
    0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,
    0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
    0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,
    0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

unsigned char *pad_message(uint8_t *message, uint64_t last_chunk_length, uint64_t total_length, uint64_t *padded_length) {

    uint64_t i;
    uint64_t bit_length = total_length * 8;
    uint64_t new_length;
    unsigned char *padded_message;

    /* Adds an extra chunk of zeroes based on if the 0x80 + bit length overflows >64 bytes */
    if (last_chunk_length <= 55) {
        new_length = 64;
    } else {
        new_length = 128;
    }

    /* Empty the memory of padded_message for it to be copied into */
    padded_message = (unsigned char*)calloc(new_length, 1);
    if (!padded_message) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    /* Copy the last chunk into the padded message */
    memcpy(padded_message, message, last_chunk_length);
    
    /* Sets the last byte of the message to 1 */
    padded_message[last_chunk_length] = 0x80;
  
    /* Manually zeroing out the shit that needs to be zeroed out */
    for (i = last_chunk_length + 1; i < new_length - 8; i++) {
        padded_message[i] = 0;
    }
    
    /* Appends the bit length of the message to the end of the new message */
    for (i = 0; i < 8; i++) {
	padded_message[new_length - 8 + i] = (bit_length >> (56 - i * 8)) & 0xFF;
    }

    /* Storing the length of the new message to the value pointed to by padded_length */
    *padded_length = new_length;

    return padded_message;

}

void process_chunk(const uint8_t *chunk, uint32_t *H) {

    int i;
    uint32_t W[64];
    uint32_t a, b, c, d, e, f, g, h;

    /* Load the chunk into the first 16 words (big-endian), each stored in W[i] */
    for (i = 0; i < 16; i++) {
        W[i] = (chunk[i * 4] << 24) | (chunk[i * 4 + 1] << 16) |
               (chunk[i * 4 + 2] << 8) | chunk[i * 4 + 3];
    }

    /* Extend the first 16 words into the remaining 48 words, deriving new words and diffusing information */
    for (i = 16; i < 64; i++) {
        W[i] = SIG1(W[i - 2]) + W[i - 7] + SIG0(W[i - 15]) + W[i - 16];
    }

    /* Initialize working variables */
    a = H[0]; b = H[1]; c = H[2]; d = H[3];
    e = H[4]; f = H[5]; g = H[6]; h = H[7];

    /* Perform the 64 SHA-256 computation rounds */
    for (i = 0; i < 64; i++) {
        uint32_t T1 = h + BIG_SIG1(e) + CHOICE(e, f, g) + K[i] + W[i];
        uint32_t T2 = BIG_SIG0(a) + MEDIAN(a, b, c);

        h = g;
        g = f;
        f = e;
        e = d + T1;
        d = c;
        c = b;
        b = a;
        a = T1 + T2;
    }

    /* Update hash values */
    H[0] += a; H[1] += b; H[2] += c; H[3] += d;
    H[4] += e; H[5] += f; H[6] += g; H[7] += h;

}

void print_hash(uint32_t *H) {

    int i;
    for (i = 0; i < 8; i++) {
        printf("%08x", H[i]);
    }

    printf("\n");

}


int main(int argc, char *argv[]) {

    size_t i, l, total_length = 0;
    uint8_t m[64];
    uint64_t padded_length;
    unsigned char *padded_message;
    FILE *fp;

    if (!(argc > 1)) {
        fprintf(stderr, "No filename argument provided, exiting...\n");
        exit(1);
    }

    fp = fopen(argv[1], "rb"); /* read mode */

    if (fp == NULL) {
        fprintf(stderr, "fopen fails on argv[1] \"%s\", exiting...\n", argv[1]);
        exit(1);
    }

    /* While the size of the chunk is 64 bytes, process it and increase the total length */
    while ((l = fread(m, 1, sizeof(m), fp)) == 64) {
        process_chunk(m, H);
        total_length += l;
    } 

    /* Increase total length one more time at the end */
    total_length += l;

    /* Handle padding on the final chunk of m */
    padded_message = pad_message(m, l, total_length, &padded_length);

    /* Process padded chunks */
    for (i = 0; i < padded_length; i += 64) {
        process_chunk(padded_message + i, H);
    }   

    print_hash(H);

    free(padded_message);

    if (fclose(fp) == EOF) {
        fprintf(stderr, "fclose fails on argv[1] \"%s\", exiting...\n", argv[1]);
        exit(1);
    }

    return 0;
}
