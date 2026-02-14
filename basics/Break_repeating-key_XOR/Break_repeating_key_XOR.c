#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../utilities/utilities.h"

int guess_key_size(unsigned char *ct, int len, int *best_guess) {
	//best_guess = [2,2,2]
	int max_size = len / 2 > 40 ? 40 : len / 2;
	double lowest_dist[3] = {321, 321, 321};
	double dist;
	char first_blob[max_size + 1];
	char second_blob[max_size + 1];
	int nblocks;
	for (int guess = 2; guess <= max_size; guess++) {
		dist = 0;
		nblocks = len / guess;
		if (nblocks < 7) continue;
		for (int i = 0; i < nblocks - 1; i++) {
			if (slice((char*)ct, len, i*guess, (i + 1)*guess, first_blob)) return -1;
			if (slice((char*)ct, len, (i + 1)*guess, guess*(i + 2), second_blob)) return -1;
			dist += (double)hamming_dist((unsigned char*)first_blob, (unsigned char*)second_blob, guess) / (double)guess;
		}
		dist /= (double)(nblocks - 1);
		if (dist < lowest_dist[0]) {
			lowest_dist[2] = lowest_dist[1];
			best_guess[2] = best_guess[1];
			lowest_dist[1] = lowest_dist[0];
			best_guess[1] = best_guess[0];
			lowest_dist[0] = dist;
			best_guess[0] = guess;
		} else if (dist < lowest_dist[1]) {
			lowest_dist[2] = lowest_dist[1];
			best_guess[2] = best_guess[1];
			lowest_dist[1] = dist;
			best_guess[1] = guess;
		} else if (dist < lowest_dist[2]) {
			lowest_dist[2] = dist;
			best_guess[2] = guess;
		}
	}
	return 0;
}

int atk_vigenere_with_key_size(unsigned char *ct, int len, int key_size, unsigned char *key, unsigned char *pt) {
	for (int j = 0; j < key_size; j++) {
		unsigned char blk[len];
		unsigned char pt_blk[len];
		int bl = 0;

		for (int i = j; i < len; i += key_size) {
			blk[bl++] = ct[i];
		}

		key[j] = single_byte_xor_atk(blk, bl, pt_blk);

		bl = 0;
		for (int k = j; k < len; k += key_size) {
			pt[k] = pt_blk[bl++];
		}
	}

	pt[len] = '\0';
	key[key_size] = '\0';

	printf("key bytes: ");
	for (int i = 0; i < key_size; i++) printf("%02x ", key[i]);
	printf("\nkey as chars: ");
	for (int i = 0; i < key_size; i++) putchar((key[i] >= 32 && key[i] <= 126) ? key[i] : '.');
	printf("\n");
	return 0;
}

int main() {

	char *b64_ct = read_entire_file("6.txt");
	int b64_len = strlen(b64_ct);
	char clean_b64_ct[b64_len];
	int clean_b64_len = 0;

	for (int i = 0; i < b64_len; i++) {
		if (b64_ct[i] == '\n' || b64_ct[i] == '\r' || b64_ct[i] == ' ' || b64_ct[i] == '\t') continue;
		clean_b64_ct[clean_b64_len++] = b64_ct[i];
	}
	clean_b64_ct[clean_b64_len] = '\0';

	int pad = 0;
	if (clean_b64_len >= 2 && clean_b64_ct[clean_b64_len - 1] == '=') pad++;
	if (clean_b64_len >= 2 && clean_b64_ct[clean_b64_len - 2] == '=') pad++;

	int ct_len = (clean_b64_len / 4) * 3 - pad;

	unsigned char ct[ct_len + 1];
	b64_to_bytes(clean_b64_ct, clean_b64_len, ct, ct_len);
	free(b64_ct);

	int best_guess[3] = {2, 2, 2};
	//guess the key size (we take the 3 best guesses)
	guess_key_size(ct, ct_len, best_guess);

	//then for each guess we perform the vigenere atk and calculate the pt score
	unsigned char pt_candidate[ct_len + 1];
	unsigned char pt[ct_len + 1];
	unsigned char key_candidate[40];
	unsigned char key[40];
	double score;
	double best_score = -10000;
	for (int i = 0; i < 3; i++) {
		printf("%d\n", best_guess[i]);
		atk_vigenere_with_key_size(ct, ct_len, best_guess[i], key_candidate, pt_candidate);
		score = score_unigram_loglik(pt_candidate, ct_len);
		if (score > best_score) {
			best_score = score;
			memcpy(pt, pt_candidate, ct_len);
			memcpy(key, key_candidate, best_guess[i]);
			pt[ct_len] = '\0';
			key[best_guess[i]] = '\0';
		}
	}

	printf("the plaintext is : %s\n", (char*) pt);
	printf("the key is : %s\n", (char*) key);

	return 0;
}
