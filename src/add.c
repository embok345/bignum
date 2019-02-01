#include "bignum.h"

void bn_add(const bignum *in1, const bignum *in2, bignum *out) {
  if(bn_ispositive(in1) && bn_isnegative(in2)) {
    bn_subtract_abs(in1, in2, out);
    return;
  }
  if(bn_isnegative(in1) && bn_ispositive(in2)) {
    bn_subtract_abs(in2, in1, out);
    return;
  }
  if(bn_isnegative(in1) && bn_isnegative(in2)) {
    bn_add_abs(in1, in2, out);
    bn_setnegative(out);
    return;
  }
  bn_add_abs(in1, in2, out);
}

//Just does |in1|+|in2|, so is always positive
void bn_add_abs(const bignum *in1, const bignum *in2, bignum *out) {

  uint8_t longer = 0, remainder = 0;
  uint16_t temp = 0;
  uint32_t addLength = 0, numLength = 0;

  if(bn_isempty(in1)) {
    bn_clone(out, in2);
    bn_setpositive(out);
    return;
  }
  if(bn_isempty(in2)) {
    bn_clone(out, in1);
    bn_setpositive(out);
    return;
  }

  uint32_t len1 = bn_trueLength(in1);
  uint32_t len2 = bn_trueLength(in2);

  if(len1>len2) {
    bn_resize(out, len1);
    addLength = len2;
    numLength = len1;
    longer = 1;
  } else {
    bn_resize(out, len2);
    addLength = len1;
    numLength = len2;
    longer = 2;
  }

  //Add the first addLength blocks from a and b, taking care of overflows
  for(uint32_t i = 0; i<addLength; i++) {
    temp = (uint16_t)bn_getBlock(in1, i) + (uint16_t)bn_getBlock(in2, i) +
        (uint16_t)remainder;
    bn_setBlock(out, i, (uint8_t)(temp%256));
    remainder = temp>>8;
  }

  //Copy the final blocks from a or b, with overflow from previous addition
  if(longer == 1) {
    for(uint32_t i=addLength; i<numLength; i++) {
      temp = (uint16_t)bn_getBlock(in1, i) + (uint16_t)remainder;
      bn_setBlock(out, i, (uint8_t)(temp%256));
      remainder = temp>>8;
    }
  } else if(longer == 2) {
    for(uint32_t i=addLength; i<numLength; i++) {
      temp = (uint16_t)bn_getBlock(in2, i) + (uint16_t)remainder;
      bn_setBlock(out, i, (uint8_t)(temp%256));
      remainder = temp>>8;
    }
  }

  //If there is still overflow, put it in to a new block at the end
  if(remainder > 0) {
    bn_addblock(out);
    bn_setBlock(out, numLength, remainder);
  }

  bn_setpositive(out);
  bn_removezeros(out);
}

void bn_add_byte(const bignum *in1, uint8_t in2, bignum *out) {
  uint8_t remainder = 0;
  uint16_t temp = 0;
  uint32_t j = 1;

  if(bn_isempty(in1)) {
    bn_resize(out, 1);
    bn_setBlock(out, 0, in2);
    return;
  }

  if(in2 == 0) {
    bn_clone(out, in1);
    return;
  }

  uint32_t len = bn_length(in1);

  bn_clone(out, in1);

  //Add in2 to the final block of in1, taking care of overflow
  temp = bn_getBlock(in1, 0) + in2;
  bn_setBlock(out, 0, temp%256);
  remainder = temp>>8;

  //If we have overflow, go through the blocks until we don't
  while(remainder>0) {
    //If we have reached the end of in1, add a new block, and put the overflow into it
    if(j>=len) {
      bn_addblock(out);
      bn_setBlock(out, len, remainder);
    }
    temp = bn_getBlock(in1, j) + remainder;
    bn_setBlock(out, j, temp%256);
    remainder = temp>>8;
    j++;
  }

  bn_removezeros(out);
}

/*Increment num; num++*/
void bn_add_1(bignum *num) {
  bn_add_byte(num, 1, num);
}
