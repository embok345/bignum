#include "bignum.h"

void bn_div_2(bignum *num) {
  uint8_t remainder = 0;
  uint16_t temp;
  uint32_t noBlocks = bn_length(num);
  for(uint32_t i = 1; i<=noBlocks; i++) {
    temp = (remainder<<7) + (bn_getBlock(num, noBlocks-i) >> 1);
    //remainder = num->blocks[num->noBlocks-i]%2;
    remainder = bn_getBlock(num, noBlocks-i);
    //num->blocks[num->noBlocks-i] = temp%256;
    bn_setBlock(num, noBlocks-i, temp%256);
  }
  bn_removezeros(num);
}
