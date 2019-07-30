#ifndef _BIGNUM_H
#define _BIGNUM_H

#include <printf.h>
#include <math.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>

#ifdef USE_LOGGER
 #include "log.h"
 extern const struct log_t BN_LOG;
 #define BN_LOG_T(MSG) LOG_T(BN_LOG, MSG)
 #define BN_LOG_C(MSG) LOG_C(BN_LOG, MSG)
 #define BN_LOG_W(MSG) LOG_W(BN_LOG, MSG)
 #define BN_LOG_E(MSG) LOG_W(BN_LOG, MSG)
#else
 #include <stdio.h>
 #define LOG(A,B,C)
 #define BN_LOG
 #define LOG_T
 #define LOG_C
 #define LOG_E
 #define LOG_W
 #define BN_LOG_T
 #define BN_LOG_C
 #define BN_LOG_E
 #define BN_LOG_W
#endif
#ifndef BN_LOG_LEVEL
 #define BN_LOG_LEVEL 0
#endif
#ifndef BN_LOG_LOCATION
 #define BN_LOG_LOCATION "bn_log.log"
#endif

typedef struct bignum* bn_t;
typedef struct bn_float bn_float;

extern const bn_t BN_ZERO;
extern const bn_t BN_ONE;

/* structure.c */
int8_t bn_init(bn_t *) __attribute__ ((warn_unused_result));
int8_t bn_inits(int, ...) __attribute__ ((warn_unused_result));
int8_t bn_resize(bn_t, uint32_t) __attribute__((warn_unused_result));

void bn_free(bn_t);
void bn_deinit(bn_t*);
void bn_deinits(int,...);

int8_t bn_clone(bn_t, const bn_t) __attribute__((warn_unused_result));
int8_t bn_setzero(bn_t);

int8_t bn_rand_blocks(bn_t, uint32_t);
int8_t bn_rand(bn_t, const bn_t);

int8_t bn_isempty(const bn_t);
int8_t bn_iszero(const bn_t);

int8_t bn_addblock(bn_t);
int8_t bn_addblocks(bn_t, uint32_t);

int8_t bn_blockshift(bn_t, int32_t);

int8_t bn_lowerblocks(const bn_t, uint32_t, bn_t);
int8_t bn_upperblocks(const bn_t, uint32_t, bn_t);

void bn_removezeros(bn_t);
uint32_t bn_leadingZeros(const bn_t);
uint32_t bn_length(const bn_t);
uint32_t bn_trueLength(const bn_t);

uint8_t bn_getBlock(const bn_t, uint32_t);
void bn_setBlock(bn_t, uint32_t, uint8_t);

int8_t bn_ispositive(const bn_t);
int8_t bn_isnegative(const bn_t);
void bn_setpositive(bn_t);
void bn_setnegative(bn_t);
void bn_togglesign(bn_t);

/*float.c*/
void bnf_init(bn_float **);
void bnf_inits(int, ...);
void bnf_nuke(bn_float **);
void bnf_nukes(int, ...);
void bnf_bn2bnf(const bn_t, bn_float *);
void bnf_prnt(const bn_float *);
void bnf_add(const bn_float *, const bn_float *, bn_float *);
void bnf_mul(const bn_float *, const bn_float *, bn_float *);
void bnf_mul_bn(const bn_float *, const bn_t, bn_float *);
void bnf_integerPart(const bn_float *, bn_t);
void bnf_invert(const bn_t, bn_float *);

/*multiply.c*/
void bn_mul(const bn_t, const bn_t, bn_t);
void bn_mul_ub(const bn_t, uint8_t, bn_t);
int8_t bn_bitshift(bn_t, int64_t);

/*divide.c*/
int8_t bn_div(const bn_t, const bn_t, bn_t, bn_t);
void bn_div_rem(const bn_t, const bn_t, bn_t);
void bn_div_quot(const bn_t, const bn_t, bn_t);
void bn_half(bn_t);
uint32_t bn_oddPart(bn_t);

/*comparisons.c*/
int8_t bn_compare(const bn_t, const bn_t);
int8_t bn_equals(const bn_t, const bn_t);
int8_t bn_iseven(const bn_t);
int8_t bn_isodd(const bn_t);
uint32_t bn_min_ui(uint32_t, uint32_t);
uint32_t bn_max_ui(uint32_t, uint32_t);
int32_t bn_min_si(int32_t, int32_t);
int32_t bn_max_si(int32_t, int32_t);
int64_t bn_min_sl(int64_t, int64_t);
int64_t bn_max_sl(int64_t, int64_t);

/*print.c*/
void bn_prnt_blocks(const bn_t);
void bn_prnt_blocks_hex(const bn_t);
void bn_prnt_dec(const bn_t);
int bn_printf(FILE *, const struct printf_info *, const void *const *);
int bn_printf_info(const struct printf_info *, size_t, int *, int *);

/*conversion.c*/
void bn_conv_str2bn(const char *, bn_t);
void bn_conv_ui2bn(uint32_t, bn_t);
void bn_conv_ub2bn(uint8_t, bn_t);
uint32_t bn_conv_bn2ui(const bn_t);
char* bn_conv_bn2str(const bn_t);

/*add.c*/
int8_t bn_add(const bn_t, const bn_t, bn_t);
int8_t bn_add_ub(const bn_t, uint8_t, bn_t);
void bn_increment(bn_t);

/* subtraction.c */
void bn_sub(const bn_t, const bn_t, bn_t);
void bn_sub_ub(const bn_t, uint8_t, bn_t);//TODO
void bn_decrement(bn_t);//TODO

/* operations.c */
void bn_gcd(const bn_t, const bn_t, bn_t);
void bn_gcd2(const bn_t, const bn_t, bn_t);
void bn_sqrt(const bn_t, bn_t);

/*power.c*/
void bn_powmod(const bn_t, const bn_t, const bn_t, bn_t);
void bn_powmod_int(const bn_t, uint32_t, const bn_t, bn_t);

#endif
