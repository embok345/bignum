#include "bignum.h"

void bn_powmod(const bignum *b,
               const bignum *e,
               const bignum *m,
               bignum *out) {

  if(bn_equals(m, &ONE)) {
    bn_setzero(out);
    return;
  }

  bignum *base, *result, *exponent;
  bn_inits(3, &base, &result, &exponent);
  bn_clone(base, b);
  bn_clone(result, &ONE);
  bn_clone(exponent, e);

  bn_div_rem(base, m, base);
  while(!bn_iszero(exponent)) {
    if(!bn_iseven(exponent)) {
      bn_mul(result, base, result);
      bn_div_rem(result, m, result);
    }
    bn_div_2(exponent);
    bn_mul(base, base, base);
    bn_div_rem(base, m, base);
  }
  bn_clone(out, result);
  bn_nukes(3, &base, &result, &exponent);
}


