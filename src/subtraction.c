#include "bignum.h"

int8_t bn_add_abs(const bn_t, const bn_t, bn_t);
int8_t bn_sub_abs(const bn_t, const bn_t, bn_t);

void bn_sub(const bn_t in1, const bn_t in2, bn_t out) {
  if(bn_ispositive(in1) && bn_isnegative(in2)) {
    bn_add_abs(in1, in2, out);
    return;
  }
  if(bn_isnegative(in1) && bn_isnegative(in2)) {
    bn_sub_abs(in2, in1, out);
    return;
  }
  if(bn_isnegative(in1) && bn_ispositive(in2)) {
    bn_add_abs(in1, in2, out);
    bn_setnegative(out);
    return;
  }
  bn_sub_abs(in1, in2, out);
}

void bn_decrement(bn_t in1) {
  //TODO
}

/*Signs are ignored. just do |in1|-|in2| */
int8_t bn_sub_abs(const bn_t in1, const bn_t in2, bn_t out) {
  int8_t c = bn_compare(in1, in2);

  //if |in1|<|in2|, do the opposite, and make the result negative
  if(c==-1) {
    bn_sub_abs(in2, in1, out);
    bn_setnegative(out);
    return 1;
  }
  if(c==0) {
    bn_setzero(out);
    return 1;
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


  return 1;
}
