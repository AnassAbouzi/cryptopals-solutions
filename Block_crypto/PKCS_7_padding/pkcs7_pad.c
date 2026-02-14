#include <stdio.h>
#include <stdint.h>
#include <string.h>

int pkcs7_pad(uint8_t *pt, int len, uint8_t *padded_pt) {
	if (!pt || !padded_pt) return -1;

	int pad = 16 - (len % 16);

	if ((len + pad) % 16 != 0) return -1;

	memcpy(padded_pt, pt, len);
	memset(padded_pt + len, pad, pad);

	return len + pad;
}

int pkcs7_unpad(uint8_t *padded_pt, int len, uint8_t *pt) {
	if (!padded_pt || !pt) return -1;
	if (len % 16 != 0) return -1;
	if (len <= 0) return -1;
	int pad = padded_pt[len - 1];


	if (pad > 16 || pad == 0 || pad > len) return -1;
	for (int i = 0; i < pad; i++) {
		if (padded_pt[len - pad + i] != pad) return -1;
	}
	memcpy(pt, padded_pt, len - pad);

	return len - pad;
}

int main() {
	char *pt = "YELLOW SUBMARINE";
	uint8_t a[33];
	int len = pkcs7_pad((uint8_t *)pt, 16, a);
	a[len] = '\0';
	printf("%d\n", (int)strlen((char *)a));
	return 0;
}
