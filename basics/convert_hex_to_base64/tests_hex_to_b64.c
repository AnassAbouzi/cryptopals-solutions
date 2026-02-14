#include "hex_to_b64.h"
#include <stdio.h>
#include <string.h>

#define RUN_TEST(name) do { \
    if (!(name())) { \
        fprintf(stderr, "FAIL: %s\n", #name); \
        return 1; \
    } \
    printf("PASS: %s\n", #name); \
} while (0)

static int expect_ok(char *hex, char *expected_b64) {
    char out[256];
    int rc = hex_to_b64(hex, strlen(hex), out);
    if (rc < 0) return 0;
    return strcmp(out, expected_b64) == 0;
}

static int expect_fail(char *hex) {
    char out[256];
    int rc = hex_to_b64(hex, strlen(hex), out);
    return rc <= 0;
}


static int test_known_vector1(void) {
    return expect_ok("49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d", "SSdtIGtpbGxpbmcgeW91ciBicmFpbiBsaWtlIGEgcG9pc29ub3VzIG11c2hyb29t");
}

static int test_known_vector2(void) {
    return expect_ok("48656c6c6f", "SGVsbG8=");
}

static int test_with_0x_prefix(void) {
    return expect_ok("0x48656c6c6f", "SGVsbG8=");
}

static int test_single_byte_00(void) {
    return expect_ok("00", "AA==");
}

static int test_single_byte_ff(void) {
    return expect_ok("ff", "/w==");
}

static int test_invalid_char(void) {
    return expect_fail("zz");
}

static int test_odd_length(void) {
    return expect_fail("abc");
}

int main(void) {
    RUN_TEST(test_known_vector1);
    RUN_TEST(test_known_vector2);
    RUN_TEST(test_with_0x_prefix);
    RUN_TEST(test_single_byte_00);
    RUN_TEST(test_single_byte_ff);
    RUN_TEST(test_invalid_char);
    RUN_TEST(test_odd_length);
    puts("All tests passed.");
    return 0;
}
