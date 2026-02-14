#include <stdio.h>
#include <string.h>
#include "fixed_xor.h"

#define RUN_TEST(name) do { \
    if (!(name())) { \
        fprintf(stderr, "FAIL: %s\n", #name); \
        return 1; \
    } \
    printf("PASS: %s\n", #name); \
} while (0)

static int expect_ok(const char *a, const char *b, const char *expected) {
    char out[512];

    int rc = fixed_xor_hex(a, b, out);
    if (rc != 0) return 0;

    return strcmp(out, expected) == 0;
}

static int expect_fail(const char *a, const char *b) {
    char out[512];
    int rc = fixed_xor_hex(a, b, out);
    return rc != 0; // should fail
}

/* --- Tests --- */

// Cryptopals Set 1 Challenge 2 known vector
static int test_cryptopals_vector(void) {
    const char *s1 = "1c0111001f010100061a024b53535009181c";
    const char *s2 = "686974207468652062756c6c277320657965";
    const char *exp = "746865206b696420646f6e277420706c6179";
    return expect_ok(s1, s2, exp);
}

static int test_all_zeros_xor(void) {
    // 00 ^ 00 = 00 (per nibble too)
    return expect_ok("00", "00", "00");
}

static int test_ff_xor_0f(void) {
    // ff ^ 0f = f0
    return expect_ok("ff", "0f", "f0");
}

static int test_case_insensitive_input(void) {
    // A0 ^ 0a = aa (note output should be lowercase in your implementation)
    return expect_ok("A0", "0a", "aa");
}

static int test_mismatched_lengths_fail(void) {
    return expect_fail("00", "0000");
}

static int test_odd_length_fail(void) {
    // odd number of hex chars should fail
    return expect_fail("abc", "def");
}

static int test_invalid_char_fail(void) {
    return expect_fail("0g", "00");
}

int main(void) {
    RUN_TEST(test_cryptopals_vector);
    RUN_TEST(test_all_zeros_xor);
    RUN_TEST(test_ff_xor_0f);
    RUN_TEST(test_case_insensitive_input);
    RUN_TEST(test_mismatched_lengths_fail);
    RUN_TEST(test_odd_length_fail);
    RUN_TEST(test_invalid_char_fail);

    puts("All tests passed.");
    return 0;
}
