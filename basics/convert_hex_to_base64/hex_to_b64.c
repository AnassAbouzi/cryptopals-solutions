#include <stddef.h>
#include <stdint.h>

// convert hex character to int value
int hex_to_val(char c) {
	if ((unsigned)(c - '0') <= 9) return (int)(c - '0');
	c |= 0x20; // force lowercase
	if ((unsigned)(c - 'a') <= 5) return (int)(c - 'a' + 10);
	return -1; // if not alphanum then return error
}

static const char B64_TABLE[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int hex_to_b64(char *hex, int hex_len, char *out) {
	if (!hex || !out) return -1;

	if (hex_len & 1) return -1; // assert number of hex is pair

	// Optional 0x prefix
	if (hex_len >= 2 && hex[0] == '0' && (hex[1] == 'x' || hex[1] == 'X')) {
		hex += 2;
		hex_len -= 2;
	}

	int oi = 0;
	int i = 0;
	// 3 bytes ==> 4 b64 chars
	while(i + 6 <= hex_len) {
		// 6 hex chars = 3 bytes
		int h0 = hex_to_val(hex[i + 0]);
		int l0 = hex_to_val(hex[i + 1]);
		int h1 = hex_to_val(hex[i + 2]);
		int l1 = hex_to_val(hex[i + 3]);
		int h2 = hex_to_val(hex[i + 4]);
		int l2 = hex_to_val(hex[i + 5]);
		if ((h0|l0|h1|l1|h2|l2) < 0) return -1;

		uint8_t b0 = (uint8_t)((h0 << 4) | l0);
		uint8_t b1 = (uint8_t)((h1 << 4) | l1);
		uint8_t b2 = (uint8_t)((h2 << 4) | l2);

		out[oi++] = B64_TABLE[b0 >> 2];
		out[oi++] = B64_TABLE[((b0 & 0x03) << 4) | (b1 >> 4)];
		out[oi++] = B64_TABLE[((b1 & 0x0f) << 2) | (b2 >> 6)];
		out[oi++] = B64_TABLE[b2 & 0x3f];

		i += 6;
	}
	// now we handle remaining 1-2 bytes
	int rem_hex = hex_len - i;
	if (rem_hex == 2) {
		// 1 byte
		int h0 = hex_to_val(hex[i+0]);
		int l0 = hex_to_val(hex[i+1]);
		if ((h0|l0) < 0) return -1;
		uint8_t b0 = (uint8_t) ((h0 << 4) | l0);

		out[oi++] = B64_TABLE[b0 >> 2];
		out[oi++] = B64_TABLE[(b0 & 0x03) << 4];
		out[oi++] = '=';
		out[oi++] = '=';
	} else if (rem_hex == 4) {
		// 2 bytes
		int h0 = hex_to_val(hex[i+0]);
		int l0 = hex_to_val(hex[i+1]);
		int h1 = hex_to_val(hex[i+2]);
		int l1 = hex_to_val(hex[i+3]);
		if ((h0|l0|h1|l1) < 0) return -1;
		uint8_t b0 = (uint8_t) ((h0 << 4) | l0);
		uint8_t b1 = (uint8_t) ((h1 << 4) | l1);

		out[oi++] = B64_TABLE[b0 >> 2];
		out[oi++] = B64_TABLE[((b0 & 0x03) << 4) | (b1 >> 4)];
		out[oi++] = B64_TABLE[(b1 & 0x0f) << 2];
		out[oi++] = '=';
	}
	out[oi] = '\0';
	return oi;
}
