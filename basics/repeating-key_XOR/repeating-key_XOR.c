#include <stdio.h>
#include <string.h>
#include "../../utilities/utilities.h"

int repeating_key_xor(char *hex_pt, int len_pt, char *hex_key, int len_key, char *hex_ct) {
	char pt[len_pt / 2 + 1];
	char ct[len_pt / 2 + 1];
	char key[len_key / 2 + 1];
	hex_to_bytes(hex_pt, len_pt, (unsigned char*)pt);
	hex_to_bytes(hex_key, len_key, (unsigned char*)key);
	for (int i = 0; i < len_pt / 2 + 1; i++) {
		ct[i] = pt[i] ^ key[i % (len_key / 2)];
	}
	ct[len_pt / 2] = '\0';
	bytes_to_hex((unsigned char*)ct, len_pt / 2, hex_ct);
	return 0;
}

int main() {
	const char *pt = "Burning 'em, if you ain't quick and nimble\nI go crazy when I hear a cymbal";
	int len_pt = strlen((char*)pt) * 2;
	char hex_pt[len_pt + 1];
	char hex_ct[len_pt + 1];
	char *hex_key = "494345";
	bytes_to_hex((unsigned char*)pt, len_pt / 2, hex_pt);
	repeating_key_xor(hex_pt, len_pt, hex_key, strlen(hex_key), hex_ct);

	printf("%s", hex_ct);
	return 0;
}
