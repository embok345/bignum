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
