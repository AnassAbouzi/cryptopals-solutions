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

int repeating_key_xor(char *hex_pt, int len_pt, char *hex_key, int len_key, char *hex_ct);

unsigned char single_byte_xor_atk(unsigned char *ct, int ct_len, unsigned char *pt);

int hamming_weight(unsigned char b);

int hamming_dist(unsigned char *b1, unsigned char *b2, int len);

int slice(char *s, int len, int start, int end, char *out);

int b64_to_bytes(char *b64_string, int len, unsigned char *bytes, int len_bytes);

char *read_entire_file(char *path);

int l_shift_cyclic(uint8_t *arr, int len, int n);

int r_shift_cyclic(uint8_t *arr, int len, int n);

int xor_bytes(uint8_t *b1, uint8_t *b2, uint8_t *out, int len);

int random_bytes(uint8_t *out, int len);

int writen(int fd, uint8_t *buf, int n);
int readn(int fd, uint8_t *buf, int n);

