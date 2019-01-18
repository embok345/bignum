#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdarg.h>
#include <pthread.h>

typedef struct bignum bignum;

extern const bignum ZERO;
extern const bignum ONE;

/*structure.c*/
//void bn_inits(bignum *, uint32_t);
void bn_init(bignum **);
void bn_resize(bignum *, uint32_t);
void bn_set(bignum *, uint32_t, uint8_t *, int8_t);
void bn_rand(bignum *, uint32_t);
void bn_clone(bignum *, const bignum *);
void bn_destroy(bignum *);
void bn_nuke(bignum **);
int8_t bn_isempty(const bignum*);
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
int8_t bn_setBlock(bignum *, uint32_t, uint8_t);
int8_t bn_ispositive(const bignum *);
int8_t bn_isnegative(const bignum *);
void bn_setpositive(bignum *);
void bn_setnegative(bignum *);
void bn_signSwap(bignum *);

/*multiply.c*/
void bn_mul_byte(const bignum *, uint8_t, bignum *);

/*divide.c*/
void bn_div_2(bignum *);

/*comparisons.c*/
int8_t bn_compare(const bignum *, const bignum *);
int8_t bn_equals(const bignum *, const bignum *);
int8_t bn_iseven(const bignum *);
int8_t bn_isodd(const bignum *);
uint32_t bn_min_ui(uint32_t, uint32_t);
uint32_t bn_max_ui(uint32_t, uint32_t);
int64_t bn_min_sl(int64_t, int64_t);
int64_t bn_max_sl(int64_t, int64_t);

/*print.c*/
void bn_prnt_blocks(bignum *);
void bn_prnt_dec(bignum *);

/*conversion.c*/
char* bn_conv_bn2str(bignum *);

/*strings.c*/
void bn_str_mul256(char *, char *);
void bn_str_add(char *, char *, char *);

/*add.c*/
void bn_add(const bignum *, const bignum *, bignum *);
void bn_add_byte(const bignum *, uint8_t, bignum *);
void bn_add_1(bignum *);

/*operations.c*/
void bn_subtract(const bignum *, const bignum *, bignum *);
