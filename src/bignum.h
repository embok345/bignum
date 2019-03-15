#include <stdio.h>
#include <printf.h>
#include <math.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>
#include <stdarg.h>
#include <pthread.h>

typedef struct bignum bignum;
typedef struct bn_float bn_float;

extern const bignum ZERO;
extern const bignum ONE;

#define RED "\x1B[31m"
#define NORMAL "\033[0m"

/*structure.c*/
//void bn_inits(bignum *, uint32_t);
void bn_init(bignum **);
void bn_inits(int, ...);
void bn_resize(bignum *, uint32_t);
void bn_setzero(bignum *);
void bn_set(bignum *, uint32_t, const uint8_t *, int8_t);
void bn_rand_blocks(bignum *, uint32_t);
void bn_rand(bignum *, const bignum *);
void bn_clone(bignum *, const bignum *);
void bn_swap(bignum *, bignum *);
void bn_destroy(bignum *);
void bn_nuke(bignum **);
void bn_nukes(int, ...);
int8_t bn_isempty(const bignum*);
int8_t bn_iszero(const bignum*);
void bn_addblock(bignum *);
void bn_addblocks(bignum *, uint32_t);
void bn_blockshift(bignum *, int32_t);
void bn_bitshift(bignum *, int64_t);
void bn_removezeros(bignum *);
uint32_t bn_leadingZeros(const bignum *);
void bn_littleblocks(const bignum *, uint32_t, bignum*);
void bn_bigblocks(const bignum *, uint32_t, bignum*);
uint32_t bn_length(const bignum *);
uint32_t bn_trueLength(const bignum *);
uint8_t bn_getBlock(const bignum *, uint32_t);
void bn_setBlock(bignum *, uint32_t, uint8_t);
int8_t bn_ispositive(const bignum *);
int8_t bn_isnegative(const bignum *);
void bn_setpositive(bignum *);
void bn_setnegative(bignum *);
void bn_signSwap(bignum *);
int8_t bn_getSign(const bignum *);

/*float.c*/
void bnf_init(bn_float **);
void bnf_inits(int, ...);
void bnf_nuke(bn_float **);
void bnf_nukes(int, ...);
void bnf_bn2bnf(const bignum *, bn_float *);
void bnf_prnt(const bn_float *);
void bnf_add(const bn_float *, const bn_float *, bn_float *);
void bnf_mul(const bn_float *, const bn_float *, bn_float *);
void bnf_mul_bn(const bn_float *, const bignum *, bn_float *);
void bnf_integerPart(const bn_float *, bignum *);
void bnf_invert(const bignum *, bn_float *);

/*multiply.c*/
void bn_mul_byte(const bignum *, uint8_t, bignum *);
void bn_mul(const bignum *, const bignum *, bignum *);

/*divide.c*/
int8_t bn_div(const bignum*, const bignum*, bignum *, bignum *);
uint8_t bn_div_close(const bignum*, const bignum*, bignum *);
void bn_div_rem(const bignum *, const bignum *, bignum *);
void bn_div_quot(const bignum *, const bignum *, bignum *);
void bn_div_2(bignum *);
uint32_t bn_div_2s(bignum *);

/*comparisons.c*/
int8_t bn_compare(const bignum *, const bignum *);
int8_t bn_equals(const bignum *, const bignum *);
int8_t bn_iseven(const bignum *);
int8_t bn_isodd(const bignum *);
uint32_t bn_min_ui(uint32_t, uint32_t);
uint32_t bn_max_ui(uint32_t, uint32_t);
int32_t bn_min_si(int32_t, int32_t);
int32_t bn_max_si(int32_t, int32_t);
int64_t bn_min_sl(int64_t, int64_t);
int64_t bn_max_sl(int64_t, int64_t);

/*print.c*/
void bn_prnt_blocks(const bignum *);
void bn_prnt_dec(const bignum *);
int bn_printf(FILE *, const struct printf_info *, const void *const *);
int bn_printf_info(const struct printf_info *, size_t, int *, int *);

/*conversion.c*/
void bn_conv_str2bn(const char *, bignum *);
void bn_conv_int2bn(uint32_t, bignum *);
void bn_conv_byte2bn(uint8_t, bignum *);
uint32_t bn_conv_bn2int(const bignum *);
char* bn_conv_bn2str(const bignum *);

/*strings.c*/
int8_t isdigit_str(const char *);
void bn_str_mul256(char *, char *);
void bn_str_add(char *, char *, char *);

/*add.c*/
void bn_add(const bignum *, const bignum *, bignum *);
void bn_add_abs(const bignum *, const bignum *, bignum *);
void bn_add_byte(const bignum *, uint8_t, bignum *);
void bn_add_1(bignum *);

/*operations.c*/
void bn_subtract(const bignum *, const bignum *, bignum *);
void bn_subtract_abs(const bignum *, const bignum *, bignum *);
void bn_gcd(const bignum *, const bignum *, bignum *);
void bn_gcd2(const bignum *, const bignum *, bignum *);
void bn_sqrt(const bignum *, bignum *);

/*power.c*/
void bn_powmod(const bignum *, const bignum *, const bignum *, bignum *);
void bn_powmod_int(const bignum *, uint32_t, const bignum *, bignum *);
