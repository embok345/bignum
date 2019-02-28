#include "bignum.h"

/*int8_t bn_compare_float(bn_float_t in1, bn_float_t in2) {
	
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
}*/

/*Returns a copy of the bignum with smaller magnitude*/
/*bignum bn_min(bignum in1, bignum in2) {
	bignum out;
	if(bn_compare(in1, in2) == -1) {
		bn_copy(&out, in1);
	} else {
		bn_copy(&out, in2);
	}
	return out;
}*/

/*Returns a copy of the bignum with larger magnitude*/
/*bignum bn_max(bignum in1, bignum in2) {
	bignum out;
	if(bn_compare(in1, in2) == -1) {
		bn_copy(&out, in2);
	} else {
		bn_copy(&out, in1);
	}
	return out;
}*/

/*
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
}*/
