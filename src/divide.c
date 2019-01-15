#include "bignum.h"

/*Divide in1 by in2, and return the pair (q, r), where q is the quotient and r is the remainder of the division*/
/*divisionPair bn_div(bignum in1, bignum in2) {

	int8_t c;
	uint8_t q;
	bignum a, b, remainder, temp;
	divisionPair out;
	
	bn_copy(&a, in1);
	bn_copy(&b, in2);
	bn_removezeros(&a);
	bn_removezeros(&b);
	
	//TODO return something better when in2 = 0
	if(bn_equals(a, ZERO)) {
		printf("%sbn/bnDivision by 0!%s\n", RED, NORMAL);
		bn_copy(&out.q, ZERO);
		bn_copy(&out.r, ZERO);
		sleep(2);
		bn_destroy(&a);
		bn_destroy(&b);
		return out;
	}

	c = bn_compare(a, b);
	
	if(c == -1) { //if a<b, return (0, a)
		bn_copy(&out.q, ZERO);
		bn_copy(&out.r, a);
		bn_destroy(&a);
		bn_destroy(&b);
		return out;
	}
	if(c == 0) { //if a==b, return (1, 0)
		bn_copy(&out.q, ONE);
		bn_copy(&out.r, ZERO);
		out.q.sign = a.sign*b.sign;
		bn_destroy(&a);
		bn_destroy(&b);
		return out;
	}

	bn_init(&out.q, a.noBlocks-b.noBlocks+1);
	remainder = bn_bigblocks(a, b.noBlocks);	
	bn_removezeros(&remainder);
	c = bn_compare(remainder, b);
	
	if(c<0) { // if the top b.noBlocks blocks of a are less than b, the first quotient is 0 
		out.q.blocks[a.noBlocks-b.noBlocks] = 0;
	} else if(c==0) { // if they are equal, the first quotient is 1, with 0 remainder.
		out.q.blocks[a.noBlocks-b.noBlocks] = 1;
		bn_destroy(&remainder);
		bn_copy(&remainder, ZERO);
	} else { // otherwise, we divide them
		q = bn_div_close(remainder, b, &remainder);
		out.q.blocks[a.noBlocks-b.noBlocks] = q;
	}
	
	for(uint32_t i = 1; i<=in1.noBlocks-in2.noBlocks; i++) {
		//Get the next smaller block from a
		bn_blockshift(&remainder, 1);
		remainder.blocks[0] = a.blocks[a.noBlocks-b.noBlocks-i];
		bn_removezeros(&remainder);
		c = bn_compare(remainder, b);
		
		if(c<0) { // if the current blocks are less than b, the quotient is 0
			out.q.blocks[a.noBlocks-b.noBlocks-i] = 0;
			continue;
		}
		if(c==0) { // if the current blocks are equal to b, the quotient is 1, and remainder is 0
			out.q.blocks[a.noBlocks-b.noBlocks-i] = 1;
			bn_destroy(&remainder);
			bn_copy(&remainder, ZERO);
			continue;
		}
		
		//Otherwise, we divide them 
		bn_copy(&temp, remainder);
		bn_destroy(&remainder);
		q = bn_div_close(temp, b, &remainder);
		bn_destroy(&temp);
		out.q.blocks[a.noBlocks-b.noBlocks-i] = q;
		
	}
	
	bn_copy(&(out.r), remainder);
	bn_destroy(&remainder);
	bn_destroy(&a);
	bn_destroy(&b);
	
	bn_removezeros(&(out.q));
	bn_removezeros(&(out.r));
	
	return out;
	
	//
}

/*Divide in1 by in2, and return the quotient of division; in1//in2 
bignum bn_div_q(bignum in1, bignum in2) {
	divisionPair dp = bn_div(in1, in2);
	bn_destroy(&(dp.r));
	return dp.q;
}

/*Divide in1 by in2, and return the remainder of the division; in1 % in2 
bignum bn_div_r(bignum in1, bignum in2) {
	divisionPair dp = bn_div(in1, in2);
	bn_destroy(&(dp.q));
	return dp.r;
}

/*Divide in1 by in2, when they differ by less than one block, returning the quotient, and storing the remainder in remainder 
uint8_t bn_div_close(bignum in1, bignum in2, bignum *remainder) {
	
	int8_t c = 0;
	uint16_t q = 0;
	bignum a, b, r, temp;
	
	bn_copy(&a, in1);
	bn_copy(&b, in2);
	bn_removezeros(&a);
	bn_removezeros(&b);
	
	//TODO handle properly when close division doesn't work
	if(a.noBlocks>b.noBlocks+1 || a.noBlocks<b.noBlocks) {
		printf("%sCan't divide these 1%s\n", RED, NORMAL);
		bn_destroy(&a);
		bn_destroy(&b);
		exit(1);
	}
	if(a.noBlocks == b.noBlocks && a.blocks[a.noBlocks-1] < b.blocks[b.noBlocks-1]) {
		printf("%sCan't divide these 2%s\n", RED, NORMAL);
		bn_destroy(&a);
		bn_destroy(&b);
		exit(1);
	}
	if(a.noBlocks == b.noBlocks+1 && a.blocks[a.noBlocks-1] > b.blocks[b.noBlocks-1]) {
		printf("%sCan't divide these 3%s\n", RED, NORMAL);
		bn_destroy(&a);
		bn_destroy(&b);
		exit(1);
	}
	
	//Get division of first block of in1 by first block of in2
	q=a.blocks[a.noBlocks-1];
	
	if(a.noBlocks>b.noBlocks) {
		q*=256;
		q+=a.blocks[a.noBlocks-1];
	}
	q/=b.blocks[b.noBlocks-1];

	//Compute r = a-qb
	temp = bn_mul_byte(b, q);
	r = bn_subtract(a, temp);
	bn_destroy(&temp);
	
	//If r<0, q is an over estimate for the quotient, so add copies of b to r until r>=0, and decrement q
	if(r.sign<0) {
		do {
			bn_removezeros(&r);
			
			temp = bn_add(b, r);
			
			bn_destroy(&r);	
			bn_copy(&r, temp);
			bn_destroy(&temp);
			
			q--;
		} while(r.sign<0);
	}
	
	bn_removezeros(&r);
	
	//If r>b, we need to reduce q until r<b, by subtracting copies of b
	c = bn_compare(r,b);
	while(c>=0) {
		temp = bn_subtract(r, b);
		
		bn_destroy(&r);	
		bn_copy(&r, temp);
		bn_destroy(&temp);
			
		q++;	
		
		bn_removezeros(&r);
		
		c = bn_compare(r, b);
	}
	
	bn_destroy(remainder);
	bn_copy(remainder, r);
	
	bn_destroy(&r);
	bn_destroy(&a);
	bn_destroy(&b);
	
	return q;
}

/*Divide the bignum in1 by the int in2, and return the qutoient
bignum bn_div_intq(bignum in1, uint32_t in2) {
	if(in2 == 0) {
		printf("%sbn//iDivision by 0!%s\n", RED, NORMAL);
		sleep(2);
		return ZERO;
	}
	
	bignum out, in;
	uint32_t numInts, newBlocks; 
	uint64_t temp = 0, remainder = 0;
	
	if(in1.noBlocks<=4) {
		temp = bn_conv_bn2int(in1);
		temp/=in2;
		out = bn_conv_int2bn(temp);
		return out;
	}
	
	bn_init(&out, in1.noBlocks);
	out.sign = in1.sign;
	
	bn_copy(&in, in1);
	bn_removezeros(&in);
	
	if(in2 == 1) {
		bn_destroy(&out);
		return in;
	}
	
	numInts = in.noBlocks/4;
	temp = 0;
	//Deal with the top fewer than 4 blocks
	for(uint8_t i = 0; i<in.noBlocks%4; i++) {
		temp += ((uint32_t)in.blocks[in.noBlocks-i-1]) << (8*((in.noBlocks%4)-i-1));
	}
	remainder = temp%in2;
	newBlocks = temp/in2;
	for(uint8_t i = 0; i<in.noBlocks%4; i++) {
		out.blocks[out.noBlocks-i-1] = (uint8_t)(newBlocks>>(8*((in.noBlocks%4)-i-1)));
	}
	
	//Iterate over all chunks of 4 blocks
	for(uint32_t i = numInts; i>0; i--) {
		temp = 0;
		for(uint8_t j=0; j<4; j++) {
			temp += ((uint32_t)in.blocks[i*4-j-1]) << 8*(3-j);
		}
		temp = (uint64_t)temp + (((uint64_t)remainder)<<32);
		remainder = temp%in2;
		newBlocks = temp/in2;
		for(uint8_t j=0; j<4; j++) {
			out.blocks[i*4-j-1] = (uint8_t)(newBlocks>>(8*(3-j)));
		}
	}
	
	bn_destroy(&in);
	bn_removezeros(&out);
	return out;
}

/*Divide the bignum in1 by the int in2, and return the remainder
uint32_t bn_div_intr(bignum in1, uint32_t in2) {
	
	if(in2 == 0) {
		printf("%sbn%%iDivision by 0!%s\n", RED, NORMAL);
		sleep(2);
		return 0;
	}
	
	uint32_t numInts, out;
	uint64_t remainder = 0, temp=0;
	bignum in;
	
	bn_copy(&in, in1);
	bn_removezeros(&in);
	
	//If in has only 4 blocks, it can be used as an int
	if(in.noBlocks <= 4) {
		temp = bn_conv_bn2int(in);
		bn_destroy(&in);
		return temp%in2;
	}
	
	numInts = in.noBlocks/4;
	temp = 0;
	//Get the top 3 or fewer blocks
	for(uint8_t i = 0; i<in.noBlocks%4; i++) {
		temp += ((uint32_t)in.blocks[in.noBlocks-i-1]) << (8*((in.noBlocks%4)-i-1));
	}
	remainder = (uint64_t)temp%in2;
	
	//Iterate over all chunks of 4 blocks
	for(uint32_t i = numInts; i>0; i--) {
		temp = 0;
		for(uint8_t j = 0; j<4; j++) {
			temp+=((uint32_t)in.blocks[i*4-j-1]) << 8*(3-j);
		}
		temp = (uint64_t)temp + (((uint64_t)remainder)<<32);
		remainder = temp%in2;
	}
	
	out = (uint32_t)(remainder);
	
	bn_destroy(&in);
	return out;
}

/*Divide the bignum in1 by the byte in2, and return the quotient
bignum bn_div_byteq(bignum in1, uint8_t in2) {
	if(in2 == 0) {
		printf("%sbn//bDivision by 0!%s\n", RED, NORMAL);
		exit(1);
	}
	
	uint16_t remainder = 0;
	uint16_t temp;
	bignum out;
	
	bn_init(&out, in1.noBlocks);
	out.sign = in1.sign;
	
	for(uint32_t i = in1.noBlocks-1; i>0; i--) {
		temp = (uint16_t)in1.blocks[i] + (remainder<<8);
		out.blocks[i] = temp/in2;
		remainder = temp%in2;
	}
	temp = (uint16_t)in1.blocks[0] + (remainder<<8);
	out.blocks[0] = temp/in2;
	
	bn_removezeros(&out);
	
	return out;
}

/*Divide the bignum in1 by the byte in2, and return the remainder
uint8_t bn_div_byter(bignum in1, uint8_t in2) {
	uint8_t out;
	uint16_t remainder = 0;
	uint16_t temp;
	
	for(uint32_t i = in1.noBlocks-1; i>0; i--) {
		temp = (uint16_t)in1.blocks[i] + (remainder<<8);
		remainder = temp%in2;
	}
	temp = (uint16_t)in1.blocks[0] + (remainder<<8);
	
	out = temp%in2;
	
	return out;
}

/*Divide num by 2, directly*/
void bn_div_2(bignum *num) {
  uint8_t remainder = 0;
  uint16_t temp;
  for(uint32_t i = 1; i<=num->noBlocks; i++) {
    temp = (remainder<<7) + (num->blocks[num->noBlocks-i]>>1);
    remainder = num->blocks[num->noBlocks-i]%2;
    num->blocks[num->noBlocks-i] = temp%256;
  }
  bn_removezeros(num);
}

uint32_t bn_div_2s(bignum *num) {
  uint32_t no2s = 0;
  uint32_t noZeroBlocks = 0;
  for(uint32_t i = 0; i<(*num).noBlocks; i++) {
    if((*num).blocks[i] == 0) {
      noZeroBlocks++;
    } else {
      break;
    }
  }
  bn_blockshift(num, -noZeroBlocks);
  no2s = noZeroBlocks*8;
  while(bn_iseven((*num))) {
    bn_div_2(num);
    no2s++;
  }
  return no2s;
}
