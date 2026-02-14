#include <stddef.h>
#include <stdint.h>

extern const char HEX[];

int hex_to_val(char c);

/*
 * hex_to_bytes
 *  - hex: input hex string (may contain uppercase/lowercase)
 *  - hex_len: number of hex characters to read (must be even)
 *  - out: output buffer for bytes
 * Returns number of bytes written on success, or -1 on error.
 */
int hex_to_bytes(char *hex, int hex_len, uint8_t *out);

/*
 * bytes_to_hex
 *  - bytes: input byte array
 *  - n: number of input bytes
 *  - out: output buffer for hex chars (2*n + 1 needed)
 * Returns number of hex chars written (excluding NUL) on success, or -1 on err>
 */
int bytes_to_hex(uint8_t *bytes, int n, char *out);

int fixed_xor_hex(char *s1, char *s2, char *out);

int print_bytes(uint8_t *bytes, int len, int nl);

double score_unigram_loglik(unsigned char *s, int n);
