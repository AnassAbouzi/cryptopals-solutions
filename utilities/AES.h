#include <stdint.h>

/* AES block length in bytes (defaults to 16 in AES.c). */
extern int BLK_LEN;

/* --- Core byte substitution helpers --- */
uint8_t subbyte(uint8_t x);
uint8_t inv_subbyte(uint8_t y);

/* --- Round transformation helpers (operate on 16-byte state in column-major order) --- */
int subbytes(uint8_t *blk, int len);
int inv_subbytes(uint8_t *blk, int len);

int shift_rows(uint8_t *blk);
int inv_shift_rows(uint8_t *blk);

int mix_column(uint8_t *c);
int inv_mix_column(uint8_t *c);

int mix_columns(uint8_t *blk);
int inv_mix_columns(uint8_t *blk);

int add_round_key(uint8_t *blk, uint8_t *key, int len);

/*
 * Expand the AES-128 key into round keys.
 *
 * round_keys must point to storage for 11 round keys of BLK_LEN bytes each.
 * The parameter type matches the definition in AES.c (VLA second dimension).
 */
int key_schedule(uint8_t *key, uint8_t (*round_keys)[BLK_LEN]);

/* --- High-level encryption/decryption --- */
int aes_encrypt_blk(uint8_t *blk, uint8_t *key, uint8_t *ct);
int aes_decrypt_blk(uint8_t *blk, uint8_t *key, uint8_t *pt);

int aes_encrypt_ECB(uint8_t *pt, int len, uint8_t *key, uint8_t *ct);
int aes_decrypt_ECB(uint8_t *ct, int len, uint8_t *key, uint8_t *pt);

int pkcs7_pad(uint8_t *pt, int len, uint8_t *padded_pt);
int pkcs7_unpad(uint8_t *padded_pt, int len, uint8_t *pt);

int aes_encrypt_CBC(uint8_t *pt, int len, uint8_t *key, uint8_t *iv, uint8_t *ct);
int aes_decrypt_CBC(uint8_t *ct, int len, uint8_t *key, uint8_t *iv, uint8_t *pt);
