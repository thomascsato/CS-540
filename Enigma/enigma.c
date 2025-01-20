#include <stdio.h>

#define ROTOR_F "BDFHJLCPRTXVZNYEIWGAKMUSQO"
#define ROTOR_M "AJDKSIRUXBLHWTMCQGZNPYFVOE"
#define ROTOR_S "EKMFLGDQVZNTOWYHXUSPAIBRCJ"
#define ROTOR_R "IXUHFEZDAOMTKQJWNSRLCYPBVG"

int NC = 0;

void rotor_length() {
	for ( ; ROTOR_F[NC] ; ) {
		NC++;
	}
}

char rapply(char *rotor, char letter)  {
	return rotor[letter - 'A'];
}

void invert_rotor(char *rotor, char *inverted_rotor) {

	int indices[NC];
	char characters[NC];

	int i, j;

    	for (i = 0; i < NC; i++) {
        	indices[i] = i;
		characters[i] = rotor[i];
    	}

	// Sort the pairs based on the characters
    	for (i = 0; i < NC - 1; i++) {
        	for (j = i + 1; j < NC; j++) {
            		if (characters[i] > characters[j]) {

                		// Swap characters
                		char temp_char = characters[i];
                		characters[i] = characters[j];
                		characters[j] = temp_char;

                		// Swap indices
                		int temp_index = indices[i];
                		indices[i] = indices[j];
                		indices[j] = temp_index;

            		}
        	}
    	}

	// Create the final inverted array
    	for (i = 0; i < NC; i++) {
		inverted_rotor[i] = (char)(indices[i] + 'A');
   	}

	inverted_rotor[NC] = '\0';

}

char letter_encrypt(char letter, char *invert_f, char *invert_m, char *invert_s) {
	// Good code
	return rapply(invert_f, rapply(invert_m, rapply(invert_s, rapply(ROTOR_R, rapply(ROTOR_S, rapply(ROTOR_M, rapply(ROTOR_F, letter)))))));
}

char nshift(char letter, int shift) {
	//return (char)(((int)letter - 'A' + shift) % NC + 'A')
	return (char)((((int)letter - 'A' + shift) % NC + NC) % NC + 'A');;
}

int pythonic_mod(int x) {
    return ((x % NC) + NC) % NC;  // Ensures non-negative result
}

int shifts(int prev_letters, int rotor_shift) {
    switch(rotor_shift) {
        case 0:
            return pythonic_mod(prev_letters % NC);
        case 1:
            return pythonic_mod(prev_letters / NC % NC);      
        case 2:
            return pythonic_mod(prev_letters / (NC * NC) % NC);
        case 3:
            return 0;      
        case 4:
            return pythonic_mod(prev_letters / (NC * NC) % NC); 
        case 5:
            return pythonic_mod(prev_letters / NC % NC);      
        case 6:
            return pythonic_mod(prev_letters % NC);     
        default:
            return 0;     
    }
}

char shiply(char letter, int shift, char *rotor) {
	return nshift(rapply(rotor, nshift(letter, shift)), -shift);
}

char single_letter_enigma(char letter, int prev_letters, char *invert_f, char *invert_m, char *invert_s) {

	char *rs[7] = {ROTOR_F, ROTOR_M, ROTOR_S, ROTOR_R, invert_s, invert_m, invert_f};

	for (int i = 0; i < 7; i++) {
        	letter = shiply(letter, shifts(prev_letters, i), rs[i]);
    	}
	return letter;

}

char * enigma(const char *input, char *output, char *invert_f, char *invert_m, char *invert_s) {

	int input_size = 0;
	for ( ; input[input_size] ; ) {
		input_size++;
	}
	
	int k;
	for (k = 0; k < input_size; k++) {
		output[k] = single_letter_enigma(input[k], k + 1, invert_f, invert_m, invert_s);
	}
    	output[input_size] = '\0';

	return output;
}

int main(int argc, char **argv) { 

	// Initializes NC rotor length
	rotor_length();

	char invert_f[NC + 1];
	char invert_m[NC + 1];
	char invert_s[NC + 1];

	invert_rotor(ROTOR_F, invert_f);
	invert_rotor(ROTOR_M, invert_m);
	invert_rotor(ROTOR_S, invert_s);

	char *output;
	printf("%s\n", enigma(argv[1], output, invert_f, invert_m, invert_s));

        return 0;

}
