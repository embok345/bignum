#include "bignum.h"

int8_t bn_isprime_milrab(bignum n, uint32_t k) {
	bignum in, d, x;
	int8_t done = 0;
	bn_copy(&in, n);
	//in = n-1
	bn_sub_1(&in);
	bn_removezeros(&in);
	
	//n-1 = 2^r d
	bn_copy(&d, in);
	uint32_t r = bn_div_2s(&d);
	
	for(uint32_t i = 0; i<k; i++) {
	
		bignum a;
		bn_init(&a, 1);
		do {
			bn_destroy(&a);
			bn_rand(&a, rand()%(in.noBlocks-1));
		} while(bn_compare(a, in)>=1);
		
		x = bn_powmod_bn(a, d, n);
		
		bn_removezeros(&x);
		
		if(bn_equals(x, ONE) || bn_equals(x, in)) {
			continue;
		}
		done = 1;
		for(uint32_t j = 0; j<r-1; j++) {
			bignum temp = bn_powmod_i(x, 2, n);
			bn_destroy(&x);
			bn_copy(&x, temp);
			bn_destroy(&temp);
			bn_removezeros(&x);
			if(bn_equals(x, in)) {
				done = 0;
				break;
			}
		}
		if(!done) {
			continue;
		}
		
		return 0;
	}
	
	return 1;
}