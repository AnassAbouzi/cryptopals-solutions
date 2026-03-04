#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>
#include "../../utilities/utilities.h"
#include "../../utilities/AES.h"



int ECB_CBC_oracle(uint8_t *pt, int len, uint8_t *ct, int ct_len, int *mode) {
	if (!pt || !ct || ct_len < len + 36) return -1;

	uint8_t key[16];
	uint8_t iv[16];
	random_bytes(key, 16);
	random_bytes(iv, 16);

	int pre_len = (rand() % 6) + 5;
	int suf_len = (rand() % 6) + 5;
	uint8_t *tmp_pt = (uint8_t *)malloc(sizeof(uint8_t) * (pre_len + len + suf_len));
	if (!tmp_pt) return -1;

	random_bytes(tmp_pt, pre_len);
	random_bytes(tmp_pt + pre_len + len, suf_len);
	memmove(tmp_pt + pre_len, pt, len);

	uint8_t *padded_pt = (uint8_t *)malloc(sizeof(uint8_t) * ct_len);
	if (!padded_pt) {
		free(tmp_pt);
		return -1;
	}

	int pad_pt_len = pkcs7_pad(tmp_pt, pre_len + len + suf_len, padded_pt);

	*mode = (rand() % 2);
	if (*mode) {
		aes_encrypt_ECB(padded_pt, pad_pt_len, key, ct);
	} else {
		aes_encrypt_CBC(padded_pt, pad_pt_len, key, iv, ct);
	}

	free(padded_pt);
	free(tmp_pt);
	return pad_pt_len;
}

int main() {
	srand(time(NULL));

	int s = socket(AF_INET, SOCK_STREAM, 0);
	int yes = 1;
	setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

	struct sockaddr_in a = {0};
	a.sin_family = AF_INET;
	a.sin_port = htons(1337);
	a.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	if (bind(s, (struct sockaddr *)&a, sizeof(a)) < 0 || listen(s, 16) < 0) return 1;

	for (;;) {
		int c = accept(s, NULL, NULL);
		if (c < 0) continue;

		uint32_t n_be = 0;
		int rr = readn(c, (uint8_t *)&n_be, sizeof(n_be));
		if (rr <= 0) { close(c); continue; }

		uint32_t n = ntohl(n_be);
		if (n == 0 || n > (1024u * 1024u)) {
			printf("n too big.");
			close(c);
			continue;
		}

		uint8_t pt[n];
		rr = readn(c, pt, n);
		if (rr <= 0) {
			printf("didn't receive any pt.");
			close(c);
			continue;
		}

		int ct_cap = (int)n + 36;
		uint8_t ct[ct_cap];
		int mode = 0;
		int ct_len = ECB_CBC_oracle(pt, n, ct, ct_cap, &mode);
		if (ct_len <= 0) {
			printf("ct is empty.");
			close(c);
			continue;
		}

		uint32_t m_be = htonl((uint32_t)ct_len);
		int sent = writen(c, (uint8_t *)&m_be, sizeof(m_be));
		if (sent <= 0) {
			printf("didn't send anything.");
			close(c);
			continue;
		}

		sent = writen(c, ct, ct_len);
		if (sent <= 0) {
			printf("didn't send ciphertext.");
			close(c);
			continue;
		}

		const char *prompt =
		"guess what AES mode was used :\n"
		"0- CBC\n"
		"1- ECB\n"
		"> ";

		sent = writen(c, (uint8_t *)prompt, strlen(prompt));
		if (sent <= 0) {
			printf("couldn't send the prompt.");
			close(c);
			continue;
		}

		uint8_t guess = 0;

		rr = readn(c, &guess, 1);
		if (rr <= 0) {
			printf("didn't receive the guess.");
			close(c);
			continue;
		}

		int mode_guess;
		if (guess == '0') mode_guess = 0;
		else if (guess == '1') mode_guess = 1;
		else mode_guess = -1;

		uint8_t correct = (mode_guess == mode) ? 1 : 0;
		const char *resp = correct ? "\nCorrect!\n" : "\nWrong!\n";
		sent = writen(c, (uint8_t *)resp, strlen(resp));

		close(c);
	}

	return 0;
}
