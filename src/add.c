#include "bignum.h"

/* Adds the two numbers in1 and in2, and stores the result in out.
 * Note that this doesn't actually do any of the adding, it simply
 * calls the appropriate method depending upon the signs of the inputs.
 * ---------------------------------------------------------------------------
 * const bn_t in1 - The first operand.
 * const bn_t in2 - The second operand.
 * bn_t out - The result of the addition.
 */
void bn_add(const bn_t in1, const bn_t in2, bn_t out) {
  LOG(BN_LOG, LOG_LEVEL_FINE, "Starting to add");

  //If either operand is 0, just copy the other.
  if(bn_iszero(in1)) {
    bn_clone(out, in2);
    return;
  }
  if(bn_iszero(in2)) {
    bn_clone(out, in1);
    return;
  }

  //If the first is +ve, the second -ve, we really subtract the two.
  if(bn_ispositive(in1) && bn_isnegative(in2)) {
    bn_sub_abs(in1, in2, out);
    return;
  }

  //If the first is -ve, the second +ve, we really subtract the first from
  //the second.
  if(bn_isnegative(in1) && bn_ispositive(in2)) {
    bn_sub_abs(in2, in1, out);
    return;
  }

  //If they are both negative, we add the two, and set the result negative.
  if(bn_isnegative(in1) && bn_isnegative(in2)) {
    bn_add_abs(in1, in2, out);
    bn_setnegative(out);
    return;
  }

  //Otherwise, we just add them.
  bn_add_abs(in1, in2, out);

  LOG(BN_LOG, LOG_LEVEL_FINE, "Finished adding");
}

/* Adds the absolute values of in1 and in2, ignoring the signs.
 * ---------------------------------------------------------------------------
 * const bn_t in1 - The first operand
 * const bn_t in2 - The second operand
 * bn_t out - The result of the addition.
 */
void bn_add_abs(const bn_t in1, const bn_t in2, bn_t out) {

  uint8_t remainder = 0;
  uint16_t temp = 0;
  uint32_t addLength = 0, numLength = 0;

  //If either of the operands are 0, just copy the other one.
  if(bn_iszero(in1)) {
    bn_clone(out, in2);
    bn_setpositive(out);
    return;
  }
  if(bn_iszero(in2)) {
    bn_clone(out, in1);
    bn_setpositive(out);
    return;
  }

  uint32_t len1 = bn_trueLength(in1);
  uint32_t len2 = bn_trueLength(in2);

  const bn_t longerNum = (len1 > len2) ? in1 : in2;

  //Resize the output to be the same length as the longer input.
  if(len1>len2) {
    bn_resize(out, len1);
    addLength = len2;
    numLength = len1;
  } else {
    bn_resize(out, len2);
    addLength = len1;
    numLength = len2;
  }

  //Add all of the blocks from the shorter number to those of the longer
  for(uint32_t i = 0; i < addLength; i++) {
    temp = (uint16_t)bn_getBlock(in1, i) + (uint16_t)bn_getBlock(in2, i) +
        (uint16_t)remainder;
    bn_setBlock(out, i, (uint8_t)(temp%256));
    remainder = temp>>8;
  }

  //Copy the blocks from the longer number, with remainders from before
  for(uint32_t i = addLength; i < numLength; i++) {
    temp = (uint16_t)bn_getBlock(longerNum, i) + (uint16_t)remainder;
    bn_setBlock(out, i, (uint8_t)(temp%256));
    remainder = temp>>8;
  }

  //If there is still overflow, put it in to a new block at the end
  if(remainder > 0) {
    bn_addblock(out);
    bn_setBlock(out, numLength, remainder);
  }

  bn_setpositive(out);
  bn_removezeros(out);
}

/* Add the single byte in2 to the bignum in1, and store it in out
 * ---------------------------------------------------------------------------
 * const bn_t in1 - The bignum to be added to.
 * uint8_t in2 - The byte to add to the bignum.
 * bn_t out - Bignum to store the result in.
 */
void bn_add_ub(const bn_t in1, uint8_t in2, bn_t out) {
  uint8_t remainder = 0;
  uint16_t temp = 0;
  uint32_t j = 1;

  //If the bignum is empty, just set the first block to be in2.
  if(bn_iszero(in1)) {
    bn_resize(out, 1);
    bn_setBlock(out, 0, in2);
    return;
  }

  //If in2 is zero, just copy in1 to out
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
    //If we reach the end of in1, add a new block, and put the overflow into it
    if(j>=len) {
      bn_addblock(out);
      bn_setBlock(out, len, remainder);
      break;
    }
    temp = bn_getBlock(in1, j) + remainder;
    bn_setBlock(out, j, temp%256);
    remainder = temp>>8;
    //printf("%"PRIu8"\n", remainder);
    j++;
  }

  bn_removezeros(out);
}

/*Increment num; num++*/
void bn_increment(bn_t num) {
  bn_add_ub(num, 1, num);
}
