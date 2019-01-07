#include "bignum.h"

/*Compare the magnitude of in1 and in2; 
//-1 if |in1|<|in2|
// 0 if |in1|=|in2|
// 1 if |in1|>|in2| */
int8_t bn_compare(bignum in1, bignum in2) {
	bignum i1;
	bignum i2;
	
	//Make sure there aren't any leading zeros making one seem larger than the other
	bn_copy(&i1, in1);
	bn_copy(&i2, in2);
	bn_removezeros(&i1);
	bn_removezeros(&i2);
	
	//If one has more blocks than the other, it must be bigger
	if(i1.noBlocks<i2.noBlocks) {
		bn_destroy(&i1);
		bn_destroy(&i2);
		return -1;
	}
	if(i1.noBlocks>i2.noBlocks) {
		bn_destroy(&i1);
		bn_destroy(&i2);
		return 1;
	}
	
	//Go through each of the blocks until we find one which is bigger, and return that, or the're all the same and we return 0
	for(uint32_t i = i1.noBlocks-1; i>0; i--) {
		if(i1.blocks[i]<i2.blocks[i]) {
			bn_destroy(&i1);
			bn_destroy(&i2);
			return -1;
		}
		if(i1.blocks[i]>i2.blocks[i]) {
			bn_destroy(&i1);
			bn_destroy(&i2);
			return 1;
		}
	}
	if(i1.blocks[0]<i2.blocks[0]) {
		bn_destroy(&i1);
		bn_destroy(&i2);
		return -1;
	}
	if(i1.blocks[0]>i2.blocks[0]) {
		bn_destroy(&i1);
		bn_destroy(&i2);
		return 1;
	}
	bn_destroy(&i1);
	bn_destroy(&i2);
	return 0;
}

int8_t bn_compare_float(bn_float_t in1, bn_float_t in2) {
	
	bn_float_t i1, i2;
	
	bn_copy_float(&i1, in1);
	bn_copy_float(&i2, in2);
	
	int8_t comp = bn_compare(i1.n, i2.n);
	
	if(comp == -1 && i1.a<i2.a) {
		bn_destroy_float(&i1);
		bn_destroy_float(&i2);
		return -1;
	}
	if(comp == 1 && i1.a > i2.a) {
		bn_destroy_float(&i1);
		bn_destroy_float(&i2);
		return 1;
	}
	if(comp == 0) {
		if(i1.a == i2.a) {
			bn_destroy_float(&i1);
			bn_destroy_float(&i2);
			return 0;
		}
		if(i1.a > i2.a) {
			bn_destroy_float(&i1);
			bn_destroy_float(&i2);
			return 1;
		}
		if(i1.a < i2.a) {
			bn_destroy_float(&i1);
			bn_destroy_float(&i2);
			return -1;
		}
	}
	
	uint32_t difference = 0;
	
	if(i1.a > i2.a) {
		difference = i1.a-i2.a;
		bn_bitshift(&i1.n, difference);
		i1.a -= difference;
	} else {
		difference = i2.a-i1.a;
		bn_bitshift(&i2.n, difference);
		i2.a -= difference;
	}
	
	int8_t ret = bn_compare(i1.n, i2.n);
	bn_destroy_float(&i1);
	bn_destroy_float(&i2);
	
	return ret;
}

/*Returns a copy of the bignum with smaller magnitude*/
bignum bn_min(bignum in1, bignum in2) {
	bignum out;
	if(bn_compare(in1, in2) == -1) {
		bn_copy(&out, in1);
	} else {
		bn_copy(&out, in2);
	}
	return out;
}

/*Returns a copy of the bignum with larger magnitude*/
bignum bn_max(bignum in1, bignum in2) {
	bignum out;
	if(bn_compare(in1, in2) == -1) {
		bn_copy(&out, in2);
	} else {
		bn_copy(&out, in1);
	}
	return out;
}

/*Checks if |in1| = |in2|*/
int8_t bn_equals(bignum in1, bignum in2) {
	return (bn_compare(in1, in2) == 0) ? 1 : 0;
}

/*Checks if in is even*/
int8_t bn_iseven(bignum in) {
	return (in.blocks[0]%2 == 0) ? 1 : 0;
}
/*Checks if in is odd*/
int8_t bn_isodd(bignum in) {
	return (in.blocks[0]%2 == 0) ? 0 : 1;
}

int8_t bn_ispowerof2(bignum in) {
	bignum a;
	bn_copy(&a, in);
	bn_removezeros(&a);
	uint32_t j = a.noBlocks-1;
	
	if(a.blocks[j] != 1 && a.blocks[j] != 2 && a.blocks[j] != 4 && a.blocks[j]!=8 && a.blocks[j]!=16 && a.blocks[j]!=32 && a.blocks[j]!=64 && a.blocks[j]!=128) {
		bn_destroy(&a);
		return 0;
	}
	
	for(uint32_t i = 0; i<a.noBlocks-1; i++) {
		if(a.blocks[i] != 0) {
			bn_destroy(&a);
			return 0;
		}
	}
	
	return 1;
}

/*Returns the smaller of in1 and in2*/
uint32_t bn_min_ui(uint32_t in1, uint32_t in2) {
	return (in1<in2) ? in1 : in2;
}
/*Returns the larger of in1 and in2*/
uint32_t bn_max_ui(uint32_t in1, uint32_t in2) {
	return (in1<in2) ? in2 : in1;
}

int64_t bn_min_sl(int64_t in1, int64_t in2) {
	return (in1 < in2) ? in1 : in2;
}

int64_t bn_max_sl(int64_t in1, int64_t in2) {
	return (in1 < in2) ? in2 : in1;
}