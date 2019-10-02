#include "bignum.h"

/* Check if the given character is a digit, i.e. '0',...'9'.
 * ----------------------------------------------------------------------------
 * char c -- The character to check if it's a digit.
 *
 * return -- 1 if the character is a digit, 0 if not.
 */
int8_t is_digit(char c) {
    return c >= '0' && c <= '9';
}

/* Check if a string is entirely composed of digits, i.e. characters
 * '0',..,'9'. The string may also optionally have a '+' or '-' at the start.
 * ----------------------------------------------------------------------------
 * const char *str -- The string to check if it is a digit string.
 *
 * return          -- 1 if the string is a digit string, 0 if not.
 */
int8_t isdigit_str(const char* str) {
    /* If the string is empty it is not a digit string (convention). */
    if(!str || strlen(str) == 0) return 0;

    /* Allow the first digit of the string to be '+' or '-'. */
    if(*str == '+' || *str == '-') {
        /* Though if the string is just "+" or "-", don't allow it. */
        if(strlen(str) == 1) return 0;
        str++;
    }
    /* Go through each character of the string, if it's not a digit, then the
     * string is not a digit string. */
    while(*str!='\0') {
        if(!is_digit(*str)) return 0;
        str++;
    }

    /* If we reach the end of the string, it is a digit string. */
    return 1;
}

/* Check if the given string is entirely composed of '0' characters.
 * ----------------------------------------------------------------------------
 * const char *str -- The string to check if it is entirely composed of '0'.
 *
 * return          -- 1 if the string only comprises '0', 0 otherwise.
 */
int8_t iszero_str(const char *str) {
    /* IF the string is empty, it is not a zero string by convention. */
    if(!str) return 0;
    size_t len = strlen(str);
    if(len == 0) return 0;

    /* Go through each character of the string. If the character is non-zero,
     * the string is non-zero. */
    for(int i = 0; i < len; i++) {
        if(str[i] != '0') return 0;
    }

    /* If we reach the end of the string, it must be a zero string. */
    return 1;
}

/* Add the second string to the first string, in a digit wise sense.
 * ----------------------------------------------------------------------------
 * char *in1             -- The first string, to which the second is added.
 * const char *const in2 -- The second string, to be added to the first.
 *
 * return                -- A pointer to `in1'.
 */
char *bn_str_add(char *in1, const char *const in2) {

    /* If either of the strings is empty, return NULL. */
    if(!in1 || !in2) return NULL;

    size_t len1 = strlen(in1);
    size_t len2 = strlen(in2);

    if(len1 == 0 || len2 == 0) return NULL;

    if(iszero_str(in1) && iszero_str(in2)) return strdup("0");

    /* Determine which string is longer. */
    uint32_t shorter, longer;
    const char *longer_str;
    if(len1 < len2) {
        shorter = len1;
        longer = len2;
        longer_str = in2;
    } else {
        shorter = len2;
        longer = len1;
        longer_str = in1;
    }

    /* Alloc enough space for the added string, one extra char for carry. */
    char temp_str[longer + 2];
    temp_str[longer + 1] = '\0';

    uint16_t remainder = 0;
    uint16_t added = 0;
    int i = 1;
    for(; i <= shorter; i++) {
        /* Add each digit from the two strings, minding carry. */
        added = in1[len1 - i] - '0' + in2[len2 - i] - '0' + remainder;
        temp_str[longer - i + 1] = (added % 10) + '0';
        remainder = added / 10;
    }

    for(; i <= longer; i++) {
        /* Add the digits from the longer string, possibly with carries. */
        added = longer_str[longer - i] - '0' + remainder;
        temp_str[longer - i + 1] = (added % 10) + '0';
        remainder = added / 10;
    }
    temp_str[0] = (remainder % 10) + '0';

    if( remainder ) {
        /* If there was a final remainder, copy the whole string. */
        in1 = realloc(in1, longer + 2);
        strncpy(in1, temp_str, longer + 2);
    } else {
        /* If not, skip the leading zero. */
        in1 = realloc(in1, longer + 1);
        strncpy(in1, temp_str + 1, longer + 1);
    }

    return in1;
}

/* "Multiply" the given string by the digit k. That is, interpret the string as
 * a decimal integer, and multiply each digit of the string by the digit,
 * taking care of overflow, and return the string of the multiplied digits. The
 * digit k must be between 0 and 9.
 * ----------------------------------------------------------------------------
 * const char *in -- The string representing a number to be multiplied.
 * int8_t k       -- The digit to multiply the string by.
 *
 * return         -- The multiplied by string. This may be freed after use.
 *                   Or NULL, if we could not multiply the string.
 */
char *bn_str_mul(const char *in, uint8_t k) {
    /* If the digit is out of range, or the input is null, or empty,
     * return null. */
    if( k >= 10 || in == NULL) return NULL;
    size_t len = strlen(in);
    if(len == 0) return NULL;

    /* If the digit to multiply by or the string is 0, just return "0". */
    if( k == 0 || iszero_str(in) )
        return strdup("0");

    char *out;

    /* If the digit to multiply by is 1, just return a copy of the input. */
    if( k == 1 ) {
        return strdup(in);
    }

    /* Alloc the string. We may need one extra character for carry. */
    out = malloc(len + 2);
    out[len + 1] = '\0';

    /* Traverse the string, from the least significant digit. */
    uint8_t remainder = 0;
    for(int i = len; i >= 1; i--) {
        if( in[i - 1] < '0' || in[i - 1] > '9' ) {
            /* If the current character is not a digit, return NULL. */
            free(out);
            return NULL;
        }
        /* Get the current char, convert to an int, multiply by the digit,
         * then set the same digit in the output. */
        uint8_t temp = ((in[i - 1] - '0') * k) + remainder;
        out[i] = (temp%10) + '0';
        remainder = temp / 10;
    }

    /* Add the final carry to the beginning of the string.
     * TODO this prepends '0' if there is no final remainder, which we probably
     *  don't want, but i can't see an easy way to get rid of it. */
    out[0] = remainder + '0';

    return out;
}

/* Multiplies the given string by 10**k. That is, appends k zeros to the end
 * of the string. This reallocates the input string, and returns it.
 * ----------------------------------------------------------------------------
 * char *in  -- The string to append zeros to.
 * uint8_t k -- The number of zeros to append to the string.
 *
 * return    -- A pointer to the input string, after reallocing.
 */
char *bn_str_mul10(char *in, uint8_t k) {
    /* If the string is null, or we need to append no zeros, just return the
     * input. */
    if(!in || k == 0) return in;

    size_t len = strlen(in);

    /* If the string is empty, just return it. */
    if(len == 0) return in;

    /* If the string is just composed of zeros, return a single zero. */
    if(iszero_str(in)) {
        in = realloc(in, 2);
        in[0] = '0';
        in[1] = '\0';
        return in;
    }

    /* Resize the string. */
    size_t new_len = len + k;
    in = realloc(in, new_len + 1);

    /* Set the new bytes to be '0'. */
    memset(in + len, '0', k);
    in[new_len] = '\0';

    return in;
}

/* Multiplies the given string, in the above sense, by 256.
 * ----------------------------------------------------------------------------
 * const char *const in -- The string to be multiplied by 256.
 *
 * return               -- A freshly alloced string, the result of multiplying
 *                         the given string by 256, in a digit wise sense.
 */
char *bn_str_mul256(const char *const in) {

    /* Check for any degenerative cases. */
    if(!in) return NULL;
    if(strlen(in) == 0) return strdup(in);
    if(iszero_str(in)) return strdup("0");

    /* Multiply by 2, then 10. */
    char *out = bn_str_mul(in, 2);
    out = bn_str_mul10(out, 1);
    /* Multiply the original by 5, add to the previous, and multiply by 10. */
    char *str5 = bn_str_mul(in, 5);
    out = bn_str_mul10(bn_str_add(out, str5), 1);
    /* Multiply original by 6, and add to the previous. */
    char *str6 = bn_str_mul(in, 6);
    out = bn_str_add(out, str6);

    free(str5);
    free(str6);

    /* Check how many leading zeros there are. */
    size_t len = strlen(out);
    int no_zeros = 0;
    for(int i = 0; i < len; i++) {
        if(out[i] == '0') no_zeros++;
        else break;
    }

    /* If the string is entirely composed of zeros, return "0". */
    if( len > 1 && no_zeros == len ) {
        free(out);
        return strdup("0");
    }

    /* Otherwise, strip the leading zeros. */
    if( len > 1 && no_zeros > 0 ) {
        char *temp = malloc(len - no_zeros + 1);
        memcpy(temp, out + no_zeros, len - no_zeros + 1);
        free(out);
        return temp;
    }

    return out;
}
