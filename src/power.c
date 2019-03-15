#include "bignum.h"

void bn_powmod_int(const bignum *b, uint32_t e, const bignum *m, bignum *out) {
  if(bn_equals(m, &ONE)) {
    bn_setzero(out);
    return;
  }

  bignum *base, *result;
  bn_inits(2, &base, &result);
  bn_clone(base, b);
  bn_clone(result, &ONE);

  //printf("b = %B\nresult = %B\n", base, result);

  while(e!=0) {
    //printf("%"PRIu32"\n", e);
    if(e%2==1) {
      bn_mul(result, base, result);
      bn_div_rem(result, m, result);
      //printf("result*base mod m = %B\n", result);
    }
    e>>=1;
    bn_mul(base, base, base);
    bn_div_rem(base, m, base);
    //printf("base * base mod m = %B\n", base);
  }

  bn_clone(out, result);

  bn_nukes(2, &base, &result);

}

void bn_powmod(const bignum *b,
               const bignum *e,
               const bignum *m,
               bignum *out) {

  if(bn_equals(m, &ONE)) {
    bn_setzero(out);
    return;
  }

  uint32_t loops = 0;

  /*bn_float *mod, *quot;
  bnf_inits(2, &mod, &quot);
  bnf_invert(m, mod);*/

  bignum *base, *result, *exponent, *q, *temp;
  bn_inits(5, &base, &result, &exponent, &q, &temp);
  bn_clone(base, b);
  bn_clone(result, &ONE);
  bn_clone(exponent, e);

  /*if(bn_compare(base_1, m) > 0) {
    bnf_mul_bn(mod, base_1, quot);
    bnf_integerPart(quot, q);
    bn_mul(q, m, temp);
    bn_subtract(base_1, temp, base_1);
  }*/
  bn_div_rem(base, m, base);

  //printf("b1 = %B\nb2 = %B\n", base_1, base_2);
  //printf("r1 = %B\nr2 = %B\n", result_1, result_2);

  //printf("%"PRId8", %"PRId8"\n", bn_getSign(base_1), bn_getSign(base_2));

  while(!bn_iszero(exponent)) {
    if(!bn_iseven(exponent)) {
      loops++;
      //bn_mul(result_1, base_1, result_1);
      bn_mul(result, base, result);

      /*if(bn_compare(result_1, m) > 0) {
        bnf_mul_bn(mod, result_1, quot);
        bnf_integerPart(quot, q);
        bn_mul(q, m, temp);
        bn_subtract(result_1, temp, result_1);
      }*/

      bn_div_rem(result, m, result);
    }
    loops++;
    bn_div_2(exponent);
    //bn_mul(base_1, base_1, base_1);
    bn_mul(base, base, base);

    /*if(bn_compare(base_1, m) > 0) {
      bnf_mul_bn(mod, base_1, quot);
      bnf_integerPart(quot, q);
      bn_mul(q, m, temp);
      bn_subtract(base_1, temp, base_1);
    }*/
    bn_div_rem(base, m, base);
  }

  //printf("result1 = %B\n", result_1);
  //printf("result2 = %B\n", result_2);

  //bn_clone(out, result_1);
  bn_clone(out, result);

  bn_nukes(5, &base, &result, &exponent, &q, &temp);
}


