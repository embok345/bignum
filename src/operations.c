#include "bignum.h"

void bn_gcd(const bn_t in1, const bn_t in2, bn_t out) {
  if(bn_iszero(in2)) {
    bn_clone(out, in1);
    return;
  }
  bn_t r;
  bn_init(&r);
  bn_div_rem(in1, in2, r);
  bn_gcd(in2, r, out);
  bn_nuke(&r);
}

void bn_gcd2(const bn_t in1, const bn_t in2, bn_t out) {

  if(bn_iszero(in1)) {
    bn_clone(out, in2);
    return;
  }
  if(bn_iszero(in2)) {
    bn_clone(out, in1);
    return;
  }

  bn_t u, v;
  bn_init(&u);
  bn_init(&v);
  bn_clone(u, in1);
  bn_clone(v, in2);

  uint32_t shift1 = bn_oddPart(u);
  uint32_t shift2 = bn_oddPart(v);
  uint32_t shift = bn_min_ui(shift1, shift2);

  do {
    bn_oddPart(v);

    if(bn_compare(u, v)>0) {
      bn_swap(u, v);
    }

    bn_sub(v, u, v);

    bn_removezeros(v);
    bn_removezeros(u);
  } while(!bn_iszero(v));

  bn_nuke(&v);

  bn_bitshift(u, shift);
  bn_clone(out, u);

  bn_nuke(&u);
}


void bn_sqrt(const bn_t in, bn_t out) {
  if(bn_trueLength(in) <= 4) {
    bn_conv_ui2bn(sqrt(bn_conv_bn2ui(in)), out);
    return;
  }

  bn_t x, temp;
  bn_inits(2,&x,&temp);
  uint32_t len = bn_trueLength(in);

  if(len%2 != 0) {
    bn_resize(x, (len-1)>>1);
    bn_setBlock(x, bn_length(x)-1, 64);
  } else {
    bn_resize(x, len>>1);
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
    bn_clone(x, temp);

    j++;
    if(j>=2*len) {
      //printf("%sTaking too long to converge; just giving you what I got%s\n", RED, NORMAL);
      //sleep(5);
      //TODO do something when convergence takes too long
      break;
    }
  }

  bn_clone(out, x);

  bn_nukes(2, &x, &temp);

  bn_removezeros(out);
}

