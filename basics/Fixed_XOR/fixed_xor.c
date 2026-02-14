#include <stddef.h>
#include <string.h>

// convert hex character to int value
int hex_to_val(char c) {
        if ((unsigned)(c - '0') <= 9) return (int)(c - '0');
        c |= 0x20; // force lowercase
        if ((unsigned)(c - 'a') <= 5) return (int)(c - 'a' + 10);
        return -1; // if not alphanum then return error
}

const char *HEX_TABLE = "0123456789abcdef";

int fixed_xor_hex(char *s1, char *s2, char *out) {
	if (strlen(s1) != strlen(s2)) return -1;
	if (strlen(s1) & 1) return -1;

	int i = 0;
	while (s1[i] != '\0' ) {
		int a = hex_to_val(s1[i]);
		int b = hex_to_val(s2[i]);
		if ((a | b) < 0) return -1;
		out[i++] = HEX_TABLE[a ^ b];
	}
	out[i] = '\0';
	return 0;
}

