#include "bignum.h"

void bn_mul_byte(const bignum *in1, uint8_t in2, bignum *out) {
  uint32_t len = bn_length(in1);
  bn_resize(out, len);
  uint16_t temp;
  uint8_t remainder = 0;
  for(uint32_t i=0; i<len; i++) {
    //temp = (uint16_t)in1.blocks[i]*(uint16_t)in2 + remainder;
    temp = (uint16_t)bn_getBlock(in1, i)*(uint16_t)in2 + remainder;
    //out->blocks[i] = temp%256;
    bn_setBlock(out, i, temp%256);
    remainder = temp>>8;
  }
  if(remainder!=0) {
    bn_addblock(out);
    //out->blocks[out->noBlocks-1] = remainder;
    bn_setBlock(out, len, remainder);
  }
}
