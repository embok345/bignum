#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdarg.h>
#include <pthread.h>

typedef struct {
	uint32_t noBlocks;
	uint8_t* blocks;
	int8_t sign;
} bignum;

typedef struct {
	int64_t a;
	bignum n;
} bn_float_t;

typedef struct {
	bignum q;
	bignum r;
} divisionPair;

extern const bignum ZERO;
extern const bignum ONE;

#define RED "\x1B[31m"
#define NORMAL "\033[0m"
//}}}

/*add.c*/
bignum bn_add(bignum, bignum);
void bn_add_2(bignum, bignum, bignum*);
void bn_add_byte(bignum *, uint8_t);
void bn_add_1(bignum *);

/*comparisons.c*/
int8_t bn_compare(bignum, bignum);
int8_t bn_compare_float(bn_float_t, bn_float_t);
bignum bn_min(bignum, bignum);
bignum bn_max(bignum, bignum);
int8_t bn_equals(bignum, bignum);
int8_t bn_iseven(bignum);
int8_t bn_isodd(bignum);
int8_t bn_ispowerof2(bignum);
uint32_t bn_min_ui(uint32_t, uint32_t);
uint32_t bn_max_ui(uint32_t, uint32_t);
int64_t bn_min_sl(int64_t, int64_t);
int64_t bn_max_sl(int64_t, int64_t);

/*conversion.c*/
bignum bn_conv_str2bn(char *);
bignum bn_conv_int2bn(uint32_t);
bignum bn_conv_byte2bn(uint8_t);
char *bn_conv_bn2str(bignum);
uint32_t bn_conv_bn2int(bignum);

/*divide.c*/
divisionPair bn_div(bignum, bignum);
bignum bn_div_q(bignum, bignum);
bignum bn_div_r(bignum, bignum);
uint8_t bn_div_close(bignum, bignum, bignum*);
bignum bn_div_intq(bignum, uint32_t);
uint32_t bn_div_intr(bignum, uint32_t);
bignum bn_div_byteq(bignum, uint8_t);
uint8_t bn_div_byter(bignum, uint8_t);
void bn_div_2(bignum *);
uint32_t bn_div_2s(bignum *);

/*factoring.c*/
bignum bn_factor_trial(bignum, uint32_t);
bignum bn_factor_prho(bignum);
bignum bn_factor_prho2(bignum);
void *prhoThrd(void *);

/*multiply.c*/
bignum bn_mul(bignum, bignum);
bignum bn_pow_i(bignum, int32_t);
bignum bn_powmod_bn(bignum, bignum, bignum);
bignum bn_powmod_i(bignum, int32_t, bignum);
bignum bn_mul_long(bignum[]);
bignum bn_mul_karat(bignum[]);
bignum bn_mul_tc3(bignum[]);
bignum bn_mul_byte(bignum, uint8_t);
void bn_mul_10(bignum *);
void bn_mul_2(bignum *);

/*operations.c*/
bignum bn_subtract(bignum, bignum);
void bn_subtract_2(bignum, bignum, bignum*);
void bn_sub_1(bignum *);
bignum bn_minus_ret(bignum);
void bn_minus_ptr(bignum *);
void bn_abs(bignum *);
bignum bn_gcd(bignum, bignum);
bignum bn_gcd2(bignum, bignum);
bignum bn_sqrt(bignum);
uint32_t bn_log2(bignum);
uint32_t bn_int_eratosthenes(uint32_t, uint32_t**);

/*primes.c*/
int8_t bn_isprime_milrab(bignum, uint32_t);

/*print.c*/
void bn_prnt_blocks(bignum);
void bn_prnt_dec(bignum);

/*strings.c*/
void bn_str_mul256(char *, char *);
void bn_str_mul6(char *, char *);
void bn_str_mul50(char *, char *);
void bn_str_mul200(char *, char *);
void bn_str_add(char *, char *, char *);

/*structure.c*/
void bn_inits(bignum *);
void bn_init(bignum *, uint32_t);
void bn_set(bignum *, uint32_t, uint8_t *, int8_t);
void bn_rand(bignum *, uint32_t);
void bn_copy(bignum *, bignum);
void bn_copy_float(bn_float_t *, bn_float_t);
void bn_destroy(bignum *);
void bn_destroy_float(bn_float_t *);
void bn_addblock(bignum *);
void bn_addblocks(bignum *, uint32_t);
void bn_blockshift(bignum *, int32_t);
void bn_bitshift(bignum *, int64_t);
void bn_removezeros(bignum *);
bignum bn_littleblocks(bignum, uint32_t);
bignum bn_bigblocks(bignum, uint32_t);

/*float.c*/
void bn_float_init(bn_float_t *);
void bn_float_destroy(bn_float_t *);
bn_float_t bn_float_add(bn_float_t, bn_float_t);
bn_float_t bn_float_mul(bn_float_t, bn_float_t);
bn_float_t bn_float_trunc(const bn_float_t, uint32_t);
bn_float_t bn_float_divb(const bn_float_t, uint32_t, uint32_t);
bn_float_t bn_float_powb(const bn_float_t, uint32_t, uint32_t);
bn_float_t bn_float_nrootb(const bn_float_t, uint32_t, uint32_t);
bn_float_t bn_float_nrootb2(const bn_float_t, uint32_t, uint32_t);
int8_t bn_ispower_check(bignum, bignum, uint32_t);
bignum bn_root_k(bignum, bn_float_t, uint32_t);
int8_t bn_ispower(bignum);
