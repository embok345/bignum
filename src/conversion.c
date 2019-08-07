#include "bignum.h"

int8_t isdigit_str(const char *);
char *bn_str_mul256(const char *const);
char *bn_str_add(char *, const char *const);

/* Converts a bignum into a decimal string.
 * ----------------------------------------------------------------------------
 * const bn_t num -- The number to convert to a string.
 *
 * return         -- A decimal string representation of the bignum, which may
 *                   be freed later.
 */
char *bn_conv_bn2str(const bn_t num) {

    /* If the number is zero, just return "0" */
    if(bn_iszero(num)) return strdup("0");

    char *out = strdup("0"), *next_block = malloc(4);
    uint32_t numBlocks = bn_trueLength(num);

    /* Oterate throught each block of the number. */
    for(uint32_t i = numBlocks-1; i>0; i--) {
        /* Convert the current block to a string. */
        snprintf(next_block, 4, "%"PRIu8, bn_getBlock(num, i));

        /* Add new block to the end of the string. */
        out = bn_str_add(out, next_block);

        /* Multiply the string by 256. */
        char *temp = bn_str_mul256(out);
        free(out);
        out = temp;
    }

    /* Add the final block to the string. */
    snprintf(next_block, 4, "%"PRIu8, bn_getBlock(num, 0));
    out = bn_str_add(out, next_block);

    /* If the number is negative, add a minus sign to the front. */
    if(bn_isnegative(num)) {
        size_t len = strlen(out);
        out = realloc(out, len + 2);
        memmove(out + 1, out, len + 1);
        out[0] = '-';
    }

    free(next_block);

    return out;
}

/* Converts the given string into a bignum. The string must be a decimal string
 * possibly with a leading '+' or '-'.
 * TODO: it may be nice to allow other bases, using "0x...", "0o..." or "0b..."
 * ----------------------------------------------------------------------------
 * const char *str -- The string to convert into a bignum.
 * bn_t num        -- The bignum into which the string is stored.
 *
 * return          -- 1 if the conversion was successful, 0 otherwise.
 */
int8_t bn_conv_str2bn(const char* str, bn_t num) {

    /* If the string is not a digit string, we can't convert. */
    if(!isdigit_str(str)) return 0;

    size_t len = strlen(str);

    size_t i = 0;
    /* If the first character is '+', increment the index. */
    if(str[0] == '+') i++;
    if(str[0] == '-') {
        /* If the first character is '-', set the number to be negative. */
        bn_setnegative(num);
        i++;
    } else {
        /* Otherwise make sure the number is positive. */
        bn_setpositive(num);
    }

    /* Set the number to be most significant digit of the string. */
    bn_conv_ub2bn(str[i++] - '0', num);

    for(; i < len; i++) {
        /* Multiply the number by 10, then add the next digit. */
        bn_mul_ub(num, 10, num);
        bn_add_ub(num, str[i] - '0', num);
    }

    return 1;
}

/* Converts a bignum into an unsigned integer. It takes just the lowest four
 * blocks from the bignum and ignores the rest, and also ignores the sign of
 * the bignum.
 * ----------------------------------------------------------------------------
 * const bn_t in -- The bignum to convert to an unsigned integer.
 *
 * return        -- The lowest four blocks of the bignum, interpretted as an
 *                  integer.
 */
uint32_t bn_conv_bn2ui(const bn_t in) {
    uint32_t out = 0;
    for(int i = 3; i >= 0; i--) {
        /* Multiply the output by 256, then add the next lower block. */
        out <<= 8;
        out += bn_getBlock(in, i);
    }
    return out;
}

/* Converts an unsigned integer into a bignum.
 * ----------------------------------------------------------------------------
 * uint32_t in -- The integer to convert into a bignum.
 * bn_yt out   -- The bignum into which the integer is stored.
 */
void bn_conv_ui2bn(uint32_t in, bn_t out) {
    if(!bn_resize(out, 4)) return;

    bn_setBlock(out, 0, in%256);
    bn_setBlock(out, 1, (in>>8)%256);
    bn_setBlock(out, 2, (in>>16)%256);
    bn_setBlock(out, 3, (in>>24)%256);

    bn_removezeros(out);
}

/* Convert an unsigned byte into a bignum.
 * ----------------------------------------------------------------------------
 * uint8_t in -- The byte to converted into a bignum.
 * bn_t out   -- The bignum into which the byte is stored.
 */
void bn_conv_ub2bn(uint8_t in, bn_t out) {
    if(!bn_resize(out, 1)) return;
    bn_setBlock(out, 0, in);
}
