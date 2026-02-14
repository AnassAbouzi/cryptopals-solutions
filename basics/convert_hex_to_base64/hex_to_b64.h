#include <stddef.h>

// Converts hex (optionally starting with 0x) to Base64
// Returns number of chars written (excluding '\0'), or -1 on invalid input
int hex_to_b64(char *hex, int hex_len, char *out);
