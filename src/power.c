#include "bignum.h"

void bn_powmod(const bignum *b,
               const bignum *e,
               const bignum *m,
               bignum *out) {

  if(bn_equals(m, &ONE)) {
    bn_setzero(out);
    return;
  }

  uint32_t loops = 0;

  bn_float *mod, *quot;
  bnf_inits(2, &mod, &quot);
  bnf_invert(m, mod);

  bignum *base_1, *base_2, *result_1, *result_2, *exponent, *q, *temp;
  bn_inits(8, &base_1, &base_2, &result_1, &result_2, &exponent, &q, &temp);
  bn_clone(base_1, b);
  bn_clone(base_2, b);
  bn_clone(result_1, &ONE);
  bn_clone(result_2, &ONE);
  bn_clone(exponent, e);

  if(bn_compare(base_1, m) > 0) {
    bnf_mul_bn(mod, base_1, quot);
    bnf_integerPart(quot, q);
    bn_mul(q, m, temp);
    bn_subtract(base_1, temp, base_1);
  }
  //bn_div_rem(base_2, m, base_2);

  //printf("b1 = %B\nb2 = %B\n", base_1, base_2);
  //printf("r1 = %B\nr2 = %B\n", result_1, result_2);

  //printf("%"PRId8", %"PRId8"\n", bn_getSign(base_1), bn_getSign(base_2));

  while(!bn_iszero(exponent)) {
    if(!bn_iseven(exponent)) {
      loops++;
      bn_mul(result_1, base_1, result_1);
      //bn_mul(result_2, base_2, result_2);

      if(bn_compare(result_1, m) > 0) {
        bnf_mul_bn(mod, result_1, quot);
        bnf_integerPart(quot, q);
        bn_mul(q, m, temp);
        bn_subtract(result_1, temp, result_1);
      }

      //bn_div_rem(result_2, m, result_2);
    }
    loops++;
    bn_div_2(exponent);
    bn_mul(base_1, base_1, base_1);
    //bn_mul(base_2, base_2, base_2);

    if(bn_compare(base_1, m) > 0) {
      bnf_mul_bn(mod, base_1, quot);
      bnf_integerPart(quot, q);
      bn_mul(q, m, temp);
      bn_subtract(base_1, temp, base_1);
    }
    //bn_div_rem(base_2, m, base_2);
  }

  //printf("result1 = %B\n", result_1);
  //printf("result2 = %B\n", result_2);

  bn_clone(out, result_1);

  bn_nukes(7, &base_1, &base_2, &result_1, &result_2, &exponent, &q, &temp);
}


