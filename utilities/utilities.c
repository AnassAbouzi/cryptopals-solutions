#include <stddef.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <sys/random.h>

const char HEX[] = "0123456789abcdef";

// convert hex character to int value
int hex_to_val(char c) {
	if ((unsigned)(c - '0') <= 9) return (int)(c - '0');
        c |= 0x20; // force lowercase
        if ((unsigned)(c - 'a') <= 5) return (int)(c - 'a' + 10);
        return -1; // if not alphanum then return error
}


/*
 * hex_to_bytes
 *  - hex: input hex string (may contain uppercase/lowercase)
 *  - hex_len: number of hex characters to read (must be even)
 *  - out: output buffer for bytes
 * Returns number of bytes written on success, or -1 on error.
 */
int hex_to_bytes(char *hex, int hex_len, uint8_t *out) {
    if (!hex || !out) return -1;
    if (hex_len % 2 != 0) return -1;

    int nbytes = hex_len / 2;

    for (int i = 0; i < nbytes; i++) {
        int h = hex_to_val(hex[2 * i]);
        int l = hex_to_val(hex[2 * i + 1]);
        if ((h | l) < 0) return -1;
        out[i] = (uint8_t)((h << 4) | l);
    }
    return nbytes;
}

/*
 * bytes_to_hex
 *  - bytes: input byte array
 *  - n: number of input bytes
 *  - out: output buffer for hex chars (2*n + 1 needed)
 * Returns number of hex chars written (excluding NUL) on success, or -1 on error.
 */
int bytes_to_hex(uint8_t *bytes, int n, char *out) {
    if (!bytes || !out) return -1;

    for (int i = 0; i < n; i++) {
        out[2 * i]     = HEX[bytes[i] >> 4];
        out[2 * i + 1] = HEX[bytes[i] & 0x0F];
    }
    out[2 * n] = '\0';
    return (2 * n);
}


int fixed_xor_hex(char *s1, char *s2, char *out) {
        size_t len = strlen(s1);
        if (len != strlen(s2)) return -1;
        if (len & 1) return -1;

        uint8_t b1[len / 2];
        uint8_t b2[len / 2];
        uint8_t out_bytes[len / 2];

        hex_to_bytes(s1, len, b1);
        hex_to_bytes(s2, len, b2);

        for (size_t i = 0; i < len / 2; i++) {
                out_bytes[i] = b1[i] ^ b2[i];
        }

        bytes_to_hex(out_bytes, len / 2, out);

        return 0;
}

int print_bytes(uint8_t *bytes, int len, int nl) {
        for (int i = 0; i < len; i++) {
                printf("%c", (char)bytes[i]);
        }
        if (nl) printf("\n");
        return 0;
}

double score_unigram_loglik(unsigned char *s, int n) {
    // Approx English letter frequencies A..Z (must sum to ~1)
    static const double p[26] = {
        0.08167,0.01492,0.02782,0.04253,0.12702,0.02228,0.02015,0.06094,
        0.06966,0.00153,0.00772,0.04025,0.02406,0.06749,0.07507,0.01929,
        0.00095,0.05987,0.06327,0.09056,0.02758,0.00978,0.02360,0.00150,
        0.01974,0.00074
    };

    double score = 0.0;
    int letters = 0;

    for (int i = 0; i < n; i++) {
        unsigned char c = s[i];
        if (isalpha(c)) {
            c = (unsigned char)toupper(c);
            score += log(p[c - 'A']);
            letters++;
        } else if (c == ' ') {
            // optionally give a small bump for spaces
            score += log(0.00018); // rough space probability in English-ish text
        } else if (c >= 32 && c <= 126) {
            // neutral-ish for punctuation
            score += log(0.00002);
        } else {
            // strong penalty for non-printable
            score += log(1e-9);
        }
    }

    // Normalize to reduce bias toward longer strings
    if (n > 0) score /= (double)n;

    // If there are almost no letters, likely not English
    if (letters < n / 20) score -= 2.0;

    return score;
}



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

unsigned char single_byte_xor_atk(unsigned char *ct, int ct_len, unsigned char *pt) {
	double max_score = -100;
	double score;
	unsigned char best_candidate[ct_len];
	unsigned char candidate[ct_len];
	unsigned char k = 0;
	for (int i = 1; i < 256; i++) {
		for (int j = 0; j < ct_len; j++) {
			candidate[j] = ct[j] ^ i;
		}
		score = score_unigram_loglik(candidate, ct_len);
		if (score > max_score) {
			max_score = score;
			memcpy(best_candidate, candidate, ct_len);
			k = (unsigned char)i;
		}
	}
	memcpy(pt, best_candidate, ct_len);
	pt[ct_len] = '\0';
	return k;
}

int hamming_weight(unsigned char b) {
	int count = 0;
	for (int i = 0; i < 8; i++) {
		if ((b & 1) == 1) count++;
		b = b >> 1;
	}
	return count;
}

int hamming_dist(unsigned char *b1, unsigned char *b2, int len) {
	int dist = 0;
	for (int i = 0; i < len; i++) {
		dist += hamming_weight(b1[i] ^ b2[i]);
	}
	return dist;
}

int slice(char *s, int len, int start, int end, char *out) {
	if (start < 0 || start >= len || end <= 0 || end > len || start >= end) return -1;
	for (int i = 0; i < end - start; i++) {
		out[i] = s[i + start];
	}
	out[end - start] = '\0';
	return 0;
}

int b64_to_value(char b64_char) {
	if (b64_char <= 'Z' && b64_char >= 'A') {
		return (int)(b64_char - 'A');
	} else if (b64_char <= 'z' && b64_char >= 'a') {
		return (int)(b64_char - 'a') + 26;
	} else if (b64_char <= '9' && b64_char >= '0') {
		return (int)(b64_char - '0') + 52;
	} else if (b64_char == '+') {
		return 62;
	} else if (b64_char == '/') {
		return 63;
	} else if (b64_char == '=') {
		return 0;
	} else {
		return -1;
	}
}

int b64_to_bytes(char *b64_string, int len, unsigned char *bytes, int len_bytes) {
	if (!b64_string || !bytes) return -1;

	if (((len_bytes + 2) / 3) * 4 != len) return -1;

	int bl = 0;
	int i;
	for (i = 0; i < len - 4; i += 4) {
		uint8_t v1 = b64_to_value(b64_string[i]);
		uint8_t v2 = b64_to_value(b64_string[i + 1]);
		uint8_t v3 = b64_to_value(b64_string[i + 2]);
		uint8_t v4 = b64_to_value(b64_string[i + 3]);

		bytes[bl++] = (v1 << 2) | (v2 >> 4);
		bytes[bl++] = (v2 << 4) | (v3 >> 2);
		bytes[bl++] = (v3 << 6) | v4;
	}

	if (b64_string[i + 2] == '=') {
		uint8_t v1 = b64_to_value(b64_string[i]);
                uint8_t v2 = b64_to_value(b64_string[i + 1]);

                bytes[bl++] = (v1 << 2) | (v2 >> 4);
	} else if (b64_string[i + 3] == '=') {
		uint8_t v1 = b64_to_value(b64_string[i]);
                uint8_t v2 = b64_to_value(b64_string[i + 1]);
                uint8_t v3 = b64_to_value(b64_string[i + 2]);

                bytes[bl++] = (v1 << 2) | (v2 >> 4);
                bytes[bl++] = (v2 << 4) | (v3 >> 2);
	}

	return 0;
}

char *read_entire_file(char *path) {
	FILE *fp = fopen(path, "rb");
	if (!fp) return NULL;

	if (fseek(fp, 0, SEEK_END) != 0) {
		fclose(fp);
		return NULL;
	}

	int n = ftell(fp);
	if (n < 0) {
		fclose(fp);
		return NULL;
	}
	rewind(fp);

	char *buf = malloc(n + 1);
	if (!buf) {
		fclose(fp);
		return NULL;
	}

	int got = fread(buf, 1, n, fp);
	fclose(fp);

	buf[got] = '\0';
	return buf;
}

int l_shift_cyclic(uint8_t *arr, int len, int n) {
        if (!arr || (len <= 0)) return -1;

        n %= len;
        if (n < 0) n += len;
        if (n == 0) return 0;

        uint8_t tmp[n];
        memcpy(tmp, arr, n);
        memmove(arr, arr + n, len - n);
        memcpy(arr + (len - n), tmp, n);

        return 0;
}

int r_shift_cyclic(uint8_t *arr, int len, int n) {
        if (!arr || (len <= 0)) return -1;

        n %= len;
        if (n < 0) n += len;
        if (n == 0) return 0;

        uint8_t tmp[n];
        memcpy(tmp, arr + (len - n), n);
        memmove(arr + n, arr, len - n);
        memcpy(arr, tmp, n);

        return 0;
}

int xor_bytes(uint8_t *b1, uint8_t *b2, uint8_t *out, int len) {
	if (!b1 || !b2) return -1;

	for (int i = 0; i < len; i++) {
		out[i] = b1[i] ^ b2[i];
	}

	return 0;
}

int random_bytes(uint8_t *out, int len) {
    int off = 0;
    while (off < len) {
        int n = getrandom(out + off, len - off, 0);
        if (n > 0) {
                off += n;
                continue;
        }
        return -1;
    }
    return 0;
}

int readn(int fd, uint8_t *buf, int n) {
	int total = 0;
	while(n > 0) {
		int r = read(fd, buf, n);
		if (r <= 0) return -1;
		buf += r;
		n -= r;
		total += r;
	}
	return total;
}

int writen(int fd, uint8_t *buf, int n) {
	int total = 0;
	while (n > 0) {
		int r = write(fd, buf, n);
		if (r <= 0) return -1;
		buf += r;
		n -= r;
		total += r;
	}
	return total;
}

