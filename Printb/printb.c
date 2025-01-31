#include <stdio.h>

void printb(char n) {
    
	for (int i = 3; i >= 0; i--) {
		// n >> i moves the binary representation of n by i places, which means
		// 	the ith bit of n moves i places to the right, ending at the rightmost bit
		// & 1 extracts the last bit and prints it (1 if last bit is 1, 0 if last bit is 0)
		printf("%d", (n >> i) & 1);
	}
	printf("\n");

}

int main() {
    
    char test_values[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    for (int i = 0; i < sizeof(test_values) / sizeof(test_values[0]); i++) {
        printb(test_values[i]);
    }

    return 0;
}

