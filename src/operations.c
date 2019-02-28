#include "bignum.h"

void bn_subtract(const bignum *in1, const bignum *in2, bignum *out) {
  if(bn_ispositive(in1) && bn_isnegative(in2)) {
    bn_add_abs(in1, in2, out);
    return;
  }
  if(bn_isnegative(in1) && bn_isnegative(in2)) {
    bn_subtract_abs(in2, in1, out);
    return;
  }
  if(bn_isnegative(in1) && bn_ispositive(in2)) {
    bn_add_abs(in1, in2, out);
    bn_setnegative(out);
    return;
  }
  bn_subtract_abs(in1, in2, out);
}

/*Signs are ignored. just do |in1|-|in2| */
void bn_subtract_abs(const bignum *in1, const bignum *in2, bignum *out) {
  int8_t c = bn_compare(in1, in2);

  //if |in1|<|in2|, do the opposite, and make the result negative
  if(c==-1) {
    bn_subtract_abs(in2, in1, out);
    bn_setnegative(out);
    return;
  }
  if(c==0) {
    bn_clone(out, &ZERO);
    return;
  }

  uint32_t len = bn_trueLength(in1);
  //uint32_t len2 = bn_trueLength(in2);

  bn_resize(out, len);

  int8_t remainder = 0;
  int16_t result = 0;
  for(uint32_t i=0; i<len; i++) {
    int16_t block1 = (int16_t)bn_getBlock(in1, i);
    int16_t block2 = (int16_t)bn_getBlock(in2, i);
    result = block1-block2-remainder;
    remainder = 0;
    if(result<0) {
      result += 256;
      remainder = 1;
    }
    bn_setBlock(out, i, (uint8_t)result);
  }

  bn_removezeros(out);
  bn_setpositive(out);
}



void bn_gcd(const bignum *in1, const bignum *in2, bignum *out) {
  if(bn_iszero(in2)) {
    bn_clone(out, in1);
    return;
  }
  bignum *r;
  bn_init(&r);
  bn_div_rem(in1, in2, r);
  bn_gcd(in2, r, out);
  bn_nuke(&r);
}

void bn_gcd2(const bignum *in1, const bignum *in2, bignum *out) {

  if(bn_iszero(in1)) {
    bn_clone(out, in2);
    return;
  }
  if(bn_iszero(in2)) {
    bn_clone(out, in1);
    return;
  }

  bignum *u, *v;
  bn_init(&u);
  bn_init(&v);
  bn_clone(u, in1);
  bn_clone(v, in2);

  uint32_t shift1 = bn_div_2s(u);
  uint32_t shift2 = bn_div_2s(v);
  uint32_t shift = bn_min_ui(shift1, shift2);

  do {
    bn_div_2s(v);

    if(bn_compare(u, v)>0) {
      bn_swap(u, v);
    }

    bn_subtract(v, u, v);

    bn_removezeros(v);
    bn_removezeros(u);
  } while(!bn_iszero(v));

  bn_nuke(&v);

  bn_bitshift(u, shift);
  bn_clone(out, u);

  bn_nuke(&u);
}


void bn_sqrt(const bignum *in, bignum *out) {
  if(bn_trueLength(in) <= 4) {
    bn_conv_int2bn(sqrt(bn_conv_bn2int(in)), out);
    return;
  }

  bignum *x, *temp;
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
    bn_div_2(temp);
    bn_removezeros(temp);

    if(bn_equals(temp, x)) {
      break;
    }
    bn_clone(x, temp);

    j++;
    if(j>=2*len) {
      printf("%sTaking too long to converge; just giving you what I got%s\n", RED, NORMAL);
      sleep(5);
      break;
    }
  }

  bn_clone(out, x);

  bn_nukes(2, &x, &temp);

  bn_removezeros(out);
}

