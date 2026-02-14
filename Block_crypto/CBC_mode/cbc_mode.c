#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../../utilities/utilities.h"
#include "../../utilities/AES.h"

int aes_encrypt_CBC(uint8_t *pt, int len, uint8_t *key, uint8_t *iv, uint8_t *ct) {
	if (!pt || !key || !iv || !ct || (len % 16 != 0)) return -1;

	int i;
	uint8_t *tmp_ct = (uint8_t*) malloc(sizeof(uint8_t) * len);
	memcpy(tmp_ct, pt, len);

	xor_bytes(tmp_ct, iv, tmp_ct, BLK_LEN);
	for (i = 0; i + BLK_LEN < len; i += BLK_LEN) {
		aes_encrypt_blk(tmp_ct + i, key, ct + i);
		xor_bytes(ct + i, tmp_ct + i + BLK_LEN, tmp_ct + i + BLK_LEN, BLK_LEN);
	}
	aes_encrypt_blk(tmp_ct + i, key, ct + i);

	free(tmp_ct);
	return 0;
}

int aes_decrypt_CBC(uint8_t *ct, int len, uint8_t *key, uint8_t *iv, uint8_t *pt) {
	if (!pt || !key || !iv || !ct || (len % 16 != 0)) return -1;

	int i;
	uint8_t *tmp_pt = (uint8_t*) malloc(sizeof(uint8_t) * len);
	memcpy(tmp_pt, ct, len);

	aes_decrypt_blk(ct, key, tmp_pt);
	xor_bytes(tmp_pt, iv, pt, BLK_LEN);

	for (i = BLK_LEN; i < len; i += BLK_LEN) {
		aes_decrypt_blk(ct + i, key, tmp_pt + i);
		xor_bytes(tmp_pt + i, ct + i - BLK_LEN, pt + i, BLK_LEN);
	}

	free(tmp_pt);
	return 0;
}


int main() {

	char *b64_ct = read_entire_file("10.txt");
	int b64_len = strlen(b64_ct);
	char clean_b64_ct[b64_len + 1];
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

	uint8_t *key = (uint8_t *)"YELLOW SUBMARINE";
	uint8_t iv[16] = {0};
	uint8_t pt[ct_len + 1];

	aes_decrypt_CBC(ct, ct_len, key, iv, pt);
	pt[ct_len] = '\0';

	printf("%s\n", (char *) pt);

	return -1;
}
