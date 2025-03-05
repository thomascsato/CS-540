#include "4096_t.h"

uint64_t bigsub(uint64_t *min, uint64_t *sub, uint64_t *dif) {

    size_t i;
    uint64_t carry = 0, tmp;
    for (i = 0; i < S; i++) {
          tmp = min[i] - sub[i] - carry;
          carry = min[i] < sub[i];
          dif[i] = tmp;
    }

    return carry;

}

uint64_t bigadd(uint64_t *in0, uint64_t *in1, uint64_t *sum) {
    
    int i;
    uint64_t carry = 0; 

    for (i = 0; i < 64; i++) {
        __uint128_t temp = (__uint128_t)in0[i] + in1[i] + carry; 
        sum[i] = (uint64_t)temp;   
        carry = temp >> 64;        
    }

    return carry;
		 
}

uint64_t bigmul(uint64_t *in0, uint64_t *in1, uint64_t *out) {

    int i, j;

    memset(out, 0, 128 * sizeof(uint64_t)); 

    for (i = 0; i < 64; i++) {

        uint64_t carry = 0;

        for (j = 0; j < 64; j++) {

            __uint128_t temp = (__uint128_t)in0[i] * in1[j]; 
            temp += out[i + j];
            temp += carry;

            out[i + j] = (uint64_t)temp;
            carry = temp >> 64; 

        }

        out[i + 64] = carry; 

    }

    return 0;

}

uint64_t bigquo(uint64_t *num, uint64_t *den, uint64_t *quo){
    (void)num;
    (void)den;
    (void)quo;
    return 0;
}

uint64_t bigrem(uint64_t *num, uint64_t *den, uint64_t *rem){
    (void)num;
    (void)den;
    (void)rem;
    return 0;
}

/* print the big value as a string */
void seebig(uint64_t *a) {

    size_t i;
    for (i = S-1; i < S ; i--) {
        printf("%016lx ", a[i]);
        if ((i % 8 == 0 && i)) {
            printf("\n");
        }
    }
    printf("\n\n");
    return;

}

int main(){
    return 0;
}
