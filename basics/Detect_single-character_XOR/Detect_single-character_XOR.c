#include "single_byte_xor_cipher.h"
#include "../../utilities/utilities.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
	FILE *fp = fopen("4.txt", "r");
	if (!fp) {
		perror("fopen");
		return 1;
	}

	char line[1024];
	char decrypted_line[512];
	double score;
	char best_decrypted_line[512];
	double best_score = -10000;
	while(fgets(line, sizeof(line), fp)) {
		int len = strlen(line);
		if (len > 0 && line[len - 1] == '\n') {
			line[len - 1] = '\0'; // removing the \n char to keep length pair and avoid potential bugs
		}
		single_byte_xor_atk(line, len - 1, decrypted_line);
		score = score_unigram_loglik((unsigned char*)decrypted_line, (len - 1) / 2);
		if (score > best_score) {
			best_score = score;
			memcpy(best_decrypted_line, decrypted_line, (len - 1) / 2 + 1);
		}
		//printf("%s", decrypted_line);
	}
	printf("%s", best_decrypted_line);
	fclose(fp);
	return 0;
}
