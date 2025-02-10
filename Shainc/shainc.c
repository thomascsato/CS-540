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

void pad_message(uint8_t *message, uint64_t total_length, uint64_t *padded_length) {

    uint64_t i;
    uint64_t bit_length = total_length * 8;

    /* Calculates the new length of the message after padding
     * i.e. rounds down to the nearest multiple of 64 after adding bytes */
    uint64_t new_length = (total_length + 1 + 8 + 63) & ~63;

    /* Sets the remaining bytes to zeroes from the last byte up until the new size */
    memset(message + total_length, 0, new_length - total_length);

    /* Sets the last byte of the message to 1 */
    message[total_length] = 0x80;
    
    /* Appends the bit length of the message to the end of the new message */
    for (i = 0; i < 8; i++) {
        message[new_length - 1 - i] = (bit_length >> (8 * i)) & 0xFF;
    }
    
    /* Storing the length of the new message to the value pointed to by padded_length */
    *padded_length = new_length;

}

void process_chunk(const uint8_t *chunk, uint32_t *H) {

    int i;
    uint32_t W[64];
    uint32_t a, b, c, d, e, f, g, h;

    /* Load the chunk into the first 16 words (big-endian) */
    for (i = 0; i < 16; i++) {
        W[i] = (chunk[i * 4] << 24) | (chunk[i * 4 + 1] << 16) |
               (chunk[i * 4 + 2] << 8) | chunk[i * 4 + 3];
    }

    /* Extend the first 16 words into the remaining 48 words */
    for (i = 16; i < 64; i++) {
        W[i] = SIG1(W[i - 2]) + W[i - 7] + SIG0(W[i - 15]) + W[i - 16];
    }

    /* Initialize working variables */
    a = H[0]; b = H[1]; c = H[2]; d = H[3];
    e = H[4]; f = H[5]; g = H[6]; h = H[7];

    /* Perform the 64 SHA-256 rounds */
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
    uint8_t last_chunk[64];
    uint64_t padded_length;
    FILE *fp;

    if (!(argc > 1)) {
        fprintf(stderr, "No filename argument provided, exiting...\n");
        exit(1);
    }

    fp = fopen(argv[1], "r"); /* read mode */

    if (fp == NULL) {
        fprintf(stderr, "fopen fails on argv[1] \"%s\", exiting...\n", argv[1]);
        exit(1);
    }

    /* Reads the file fp into the buffer m up to 64 bytes */
    while ((l = fread(m, 1, sizeof(m), fp)) > 0) {
        
	/* If the file hits the limit and is 64 bytes, process the chunk */
        if (l == 64) {

            process_chunk(m, H);

	/* If it is the last chunk of the file (< 64 bytes), then
	 * copy l bytes of the message m into last_chunk */
        } else {
	    
            memcpy(last_chunk, m, l);

        }

	/* The total length of the file in bytes is incremented by l */
	total_length += l;

    }
    
    /* Handle padding */
    pad_message(last_chunk, total_length, &padded_length);
    
    /* Process padded chunks */
    for (i = 0; i < padded_length; i += 64) {
        process_chunk(last_chunk + i, H);
    }   

    print_hash(H);

    if (fclose(fp) == EOF) {
        fprintf(stderr, "fclose fails on argv[1] \"%s\", exiting...\n", argv[1]);
        exit(1);
    }

    return 0;
}
