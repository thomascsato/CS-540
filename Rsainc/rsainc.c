#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/* Simple prime generator */
uint64_t six_k_plus_one(uint64_t k) {
    uint64_t candidate = 6 * k + 1;
    uint64_t i, is_prime;
    
    /* Infinite loop until return when prime is found */
    while (1) {
        is_prime = 1;
	/* Main loop */
        for (i = 2; i * i <= candidate; i++) {
	    /* If we hit a factor, it is not prime and we move on */
            if (candidate % i == 0) {
                is_prime = 0;
                break;
            }
        }
    
    /* If we do not find a factor we assume prime... */
    if (is_prime) {
        return candidate;
    }

    /* Add six until we find the prime number */
    candidate += 6;

    }
}

uint64_t gcd(uint64_t a, uint64_t b) {
    while (b != 0) {
	/* Gets remainder of dividing the numbers and their remainders until there is no remainder
	 * Then, once there is no remainder the dividend will be the GCD. */
        uint64_t temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

uint64_t lcm(uint64_t a, uint64_t b) {
    return (a / gcd(a, b)) * b;
}

uint64_t egcd(uint64_t a, uint64_t b, int *x, int *y) {
    int x1, y1;
    uint64_t g;
    if (a == 0) {
        *x = 0;
        *y = 1;
        return b;
    }
    g = egcd(b % a, a, &x1, &y1);
    *x = y1 - (b / a) * x1;
    *y = x1;
    return g;
}

uint64_t mod_inverse(uint64_t e, uint64_t lmdb) {
    int x, y, g;
    g = egcd(e, lmdb, &x, &y);
    if (g != 1) return -1;
    while (x < 0) x += lmdb;
    return x;
}

int main() {
    uint64_t p = six_k_plus_one(5000000);
    uint64_t q = six_k_plus_one(1234567);
    uint64_t n = p * q;
    uint64_t e = 65537;
    uint64_t lmdb = lcm(p - 1, q - 1);
    uint64_t d = mod_inverse(e, lmdb);

    printf("%ld\n%ld\n%ld\n", n, e, d);

    return 0;
}
