#include "bignum.h"

void bn_powmod(const bignum *b,
               const bignum *e,
               const bignum *m,
               bignum *out) {

  if(bn_equals(m, &ONE)) {
    bn_setzero(out);
    return;
  }

  bignum *base, *result, *exponent, *loops;
  bn_inits(4, &base, &result, &exponent, &loops);
  bn_clone(base, b);
  bn_clone(result, &ONE);
  bn_clone(exponent, e);
  bn_setzero(loops);

  bn_div_rem(base, m, base);
  while(!bn_iszero(exponent)) {
    if(!bn_iseven(exponent)) {
      bn_add_1(loops);
      bn_mul(result, base, result);
      bn_div_rem(result, m, result);
    }
    bn_add_1(loops);
    bn_div_2(exponent);
    bn_mul(base, base, base);
    bn_div_rem(base, m, base);
  }
  bn_clone(out, result);

  printf("no loops = %B\n", loops);

  bn_nukes(4, &base, &result, &exponent, &loops);
}


