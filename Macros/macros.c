#include <stdio.h>

// Macro for bitwise choice
// Same logic as boolean choice, but converted to bitwise operators.
#define CHOICE(e, f, g) (((e) & (f)) ^ (~(e) & (g)))

// Macro for bitwise median
// If any pair of the three inputs are both 1, then the median will be 1. Then do it bitwise.
#define MEDIAN(e, f, g) (((e) & (f)) | ((f) & (g)) | ((g) & (e)))

// Macro for bitwise right rotate
// x >> n shifts the bits to the right
// x << sizeof(x)*8 - n wraps the digits from the right back around to the left
// | puts them together so that the n leftmost digits that needed to be wrapped around are where they should be
// and the bits shifted to the right are where they should be.
#define ROTATE(x, n) (((x) >> (n)) | ((x) << (sizeof(x) * 8 - (n))))

void printb(char n) {

        for (int i = 3; i >= 0; i--) {
                // n >> i moves the binary representation of n by i places, which means
                //      the ith bit of n moves i places to the right, ending at the rightmost bit
                // & 1 extracts the last bit and prints it (1 if last bit is 1, 0 if last bit is 0)
                printf("%d", (n >> i) & 1);
        }
        printf("\n");

}

int main() {

	return 0;

}

