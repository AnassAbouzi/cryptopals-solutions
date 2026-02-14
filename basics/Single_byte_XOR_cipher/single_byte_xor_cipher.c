#include <ctype.h>
#include <stdio.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "utilities.h"

int single_byte_xor_atk(char *ct, int ct_len, char *pt) {
	double max_score = -100;
	double score;
	uint8_t best_candidate[ct_len / 2];
	uint8_t ct_bytes[ct_len / 2];
	uint8_t candidate[ct_len / 2];
	hex_to_bytes(ct, ct_len, ct_bytes);
	for (int i = 1; i < 256; i++) {
		for (int j = 0; j < ct_len / 2; j++) {
			candidate[j] = ct_bytes[j] ^ i;
		}
		score = score_unigram_loglik(candidate, ct_len / 2);
		if (score > max_score) {
			max_score = score;
			memcpy(best_candidate, candidate, ct_len / 2);
		}
	}
	memcpy(pt, best_candidate, ct_len / 2);
	pt[ct_len / 2] = '\0';
	return 0;
}

int main() {
	char *ct = "1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736";
	int ct_len = strlen(ct);
	char pt[ct_len / 2 + 1];
	single_byte_xor_atk(ct, ct_len, pt);
	printf("%s\n", pt);
	return 0;
}
