#include "bignum.h"

void bn_gcd(const bn_t in1, const bn_t in2, bn_t out) {
  if(bn_iszero(in2)) {
    if(!bn_clone(out, in1)) return;
    return;
  }
  bn_t r;
  if(!bn_init(&r)) return;
  bn_div_rem(in1, in2, r);
  bn_gcd(in2, r, out);
  bn_deinit(&r);
}

void bn_gcd2(const bn_t in1, const bn_t in2, bn_t out) {

  if(bn_iszero(in1)) {
    if(!bn_clone(out, in2)) return;
    return;
  }
  if(bn_iszero(in2)) {
    if(!bn_clone(out, in1)) return;
    return;
  }

  bn_t u, v;
  if(!bn_inits(2, &u, &v) || !bn_clone(u, in1) || !bn_clone(v, in2))
      return;

  uint32_t shift1 = bn_oddPart(u);
  uint32_t shift2 = bn_oddPart(v);
  uint32_t shift = bn_min_ui(shift1, shift2);

  do {
    bn_oddPart(v);

    if(bn_compare(u, v)>0) {
      bn_t temp = u;
      u = v;
      v = temp;
    }

    bn_sub(v, u, v);

    bn_removezeros(v);
    bn_removezeros(u);
  } while(!bn_iszero(v));

  bn_deinit(&v);

  bn_bitshift(u, shift);
  if(!bn_clone(out, u)) return;

  bn_deinit(&u);
}


void bn_sqrt(const bn_t in, bn_t out) {
  if(bn_trueLength(in) <= 4) {
    bn_conv_ui2bn(sqrt(bn_conv_bn2ui(in)), out);
    return;
  }

  bn_t x, temp;
  if(!bn_inits(2,&x,&temp)) return;
  uint32_t len = bn_trueLength(in);

  if(len%2 != 0) {
    if(!bn_resize(x, (len-1)>>1)) return;
    bn_setBlock(x, bn_length(x)-1, 64);
  } else {
    if(!bn_resize(x, len>>1)) return;
    bn_setBlock(x, bn_length(x)-1, 4);
  }

  int j=0;

  while(1) {
    bn_div_quot(in, x, temp);
    bn_add(x, temp, temp);
    bn_half(temp);
    bn_removezeros(temp);

    if(bn_equals(temp, x)) {
      break;
    }
    if(!bn_clone(x, temp)) return;

    j++;
    if(j>=2*len) {
      //printf("%sTaking too long to converge; just giving you what I got%s\n", RED, NORMAL);
      //sleep(5);
      //TODO do something when convergence takes too long
      break;
    }
  }

  if(!bn_clone(out, x)) return;

  bn_deinits(2, &x, &temp);

  bn_removezeros(out);
}

