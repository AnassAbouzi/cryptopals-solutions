#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "../../utilities/utilities.h"

int detect_ECB(uint8_t *ct, int len) {
	uint8_t blk1[16];
	uint8_t blk2[16];

	for (int j = 0 ; j < len - 16; j += 16) {
		memcpy(blk1, ct + j, 16);
		for (int k = j + 16; k < len; k += 16) {
			memcpy(blk2, ct + k, 16);
			if (memcmp(blk1, blk2, 16) == 0) {
				return 1;
			}
		}
	}
	return 0;
}

int main() {
	FILE *fp = fopen("8.txt", "rb");
	if (!fp) return -1;

	char hex_ct[8192];
	uint8_t ct[4096];
	int len;
	int hex_len;
	int i;

	while (fgets(hex_ct, 8192, fp)) {
		i = 0;
		while (hex_ct[i] != '\0') i++;
		hex_len = i;
		if (i > 0 && hex_ct[i - 1] == '\n') {
			hex_ct[i - 1] = '\0';
			hex_len = i - 1;
		}

		len = hex_to_bytes(hex_ct, hex_len, ct);

		if (len % 16 != 0) return -1;

		if (detect_ECB(ct, len)) {
			printf("found the ECB ct : %s\n", hex_ct);
		}
	}

	fclose(fp);
	return 0;
}
