#include "bignum.h"



void bn_add_2(bignum in1, bignum in2, bignum *out) {

  bignum a, b;

  int8_t c = 0;
  uint8_t longer = 0, remainder = 0;
  uint16_t temp = 0;
  uint32_t addLength = 0, numLength = 0;

  bn_inits(&a);
  bn_inits(&b);

  bn_copy(&a, in1);
  bn_copy(&b, in2);

  //If the signs aren't the same, we are really subtracting one from the other
  if(a.sign != b.sign) {
    if(in1.sign == 1) { //if the first is +ve, we are doing a-b
      bn_minus_ptr(&b);
      bn_subtract_2(a,b, out);
    } else { //if the first is -ve, we are doing b-a
      bn_minus_ptr(&a);
      bn_subtract_2(b, a, out);
    }
    bn_destroy(&a);
    bn_destroy(&b);
  }

  c = bn_compare(a, b);

  // if a>b, we add b.noBlocks together, and a+b is a.noBlocks long
  if(c==1) {
    longer = 1;
    addLength = b.noBlocks;
    numLength = a.noBlocks;
  }
  //if a<b, we add a.noBlocks together, and a+b is b.noBlocks long
  else if(c==-1) {
    longer = 2;
    addLength = a.noBlocks;
    numLength = b.noBlocks;
  } else {
    addLength = a.noBlocks;
    numLength = a.noBlocks;
  }

  bn_init(out, numLength);

  //Add the first addLength blocks from a and b, taking care of overflows
  for(uint32_t i = 0; i<addLength; i++) {
    temp = (uint16_t)a.blocks[i] + (uint16_t)b.blocks[i] + (uint16_t)remainder;
    out->blocks[i] = (uint8_t)(temp%256);
    remainder = temp>>8;
  }

  //Copy the final blocks from a or b, with overflow from previous addition
  if(longer == 1) {
    for(uint32_t i=addLength; i<numLength; i++) {
      temp = (uint16_t)a.blocks[i] + (uint16_t)remainder;
      out->blocks[i] = (uint8_t)(temp%256);
      remainder = temp>>8;
    }
  } else if(longer == 2) {
    for(uint32_t i=addLength; i<numLength; i++) {
      temp = (uint16_t)b.blocks[i] + (uint16_t)remainder;
      out->blocks[i] = (uint8_t)(temp%256);
      remainder = temp>>8;
    }
  }

  //If there is still overflow, put it in to a new block at the end
  if(remainder > 0) {
    bn_addblock(out);
    out->blocks[out->noBlocks-1] = remainder;
  }

  if(a.sign==-1) { //If a is -ve, both a and b are -ve, so a+b is -ve
    bn_minus_ptr(out);
  }

  bn_removezeros(out);

  bn_destroy(&a);
  bn_destroy(&b);
}

/*Add a single byte in2 to the final byte of in1*/
void bn_add_byte(bignum *in1, uint8_t in2) {
	
	uint8_t remainder = 0;
	uint16_t temp = 0;
	uint32_t j = 1;
	
	//Add in2 to the final block of in1, taking care of overflow
	temp = (*in1).blocks[0] + in2;
	(*in1).blocks[0] = temp%256;
	remainder = temp>>8;
	
	//If we have overflow, go through the blocks until we don't
	while(remainder>0) {
		//If we have reached the end of in1, add a new block, and put the overflow into it
		if(j>=(*in1).noBlocks) {
			bn_addblock(in1);
			(*in1).blocks[(*in1).noBlocks-1] = remainder;
			break;
		}
		temp = (*in1).blocks[j]+remainder;
		(*in1).blocks[j] = temp%256;
		remainder = temp>>8;
		j++;
	}
}

/*Increment num; num++*/
void bn_add_1(bignum *num) {
	bn_add_byte(num, 1);
}
