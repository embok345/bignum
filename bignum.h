#include <inttypes.h>

typedef struct bignum* bn_t;

extern const bn_t BN_ZERO;
extern const bn_t BN_ONE;

/* Initialization */
int8_t bn_init(bn_t *) __attribute__((warn_unused_result));
int8_t bn_inits(int, ...) __attribute__((warn_unused_result));
int8_t bn_resize(bn_t, uint32_t) __attribute__((warn_unused_result));

/* Destruction */
void bn_free(bn_t);
void bn_deinit(bn_t *);
void bn_deinits(int, ...);

/* Utility */
void bn_swap(bn_t, bn_t);
void bn_rand_blocks(bn_t, uint32_t);

/* Comparisons */
int8_t bn_iszero(const bn_t);
int8_t bn_ispositive(const bn_t);
int8_t bn_isnegative(const bn_t);
int8_t bn_iseven(const bn_t);
int8_t bn_isodd(const bn_t);
int8_t bn_compare(const bn_t, const bn_t);
int8_t bn_equals(const bn_t, const bn_t);
uint32_t bn_min_ui(uint32_t, uint32_t);
uint32_t bn_max_ui(uint32_t, uint32_t);
int32_t bn_min_si(int32_t, int32_t);
int32_t bn_max_si(int32_t, int32_t);
int64_t bn_min_sl(int64_t, int64_t);
int64_t bn_max_sl(int64_t, int64_t);

/* Conversion */
void bn_conv_str2bn(const char *, bn_t);
void bn_conv_ui2bn(uint32_t, bn_t);
void bn_conv_ub2bn(uint8_t, bn_t);
uint32_t bn_conv_bn2ui(const bn_t);
char* bn_conv_bn2str(const bn_t);

/* Addition */
void bn_add(const bn_t, const bn_t, bn_t);
void bn_add_ub(const bn_t, uint8_t, bn_t);
void bn_increment(bn_t);

/* Subtraction */
void bn_sub(const bn_t, const bn_t, bn_t);
void bn_decrement(bn_t);

/* Multiplication */
void bn_mul(const bn_t, const bn_t, bn_t);
void bn_mul_ub(const bn_t, uint8_t, bn_t);

/* Division */
void bn_div(const bn_t, const bn_t, bn_t, bn_t);
void bn_div_rem(const bn_t, const bn_t, bn_t);
void bn_div_quot(const bn_t, const bn_t, bn_t);

/* Output */
void bn_prnt_dec(const bn_t);
