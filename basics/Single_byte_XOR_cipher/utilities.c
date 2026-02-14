#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>

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
            score += log(0.18); // rough space probability in English-ish text
        } else if (c >= 32 && c <= 126) {
            // neutral-ish for punctuation
            score += log(0.02);
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

