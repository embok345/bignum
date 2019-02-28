#include "bignum.h"

void bn_subtract_2(bignum in1, bignum in2, bignum* out) {
  return;
}

bignum bn_subtract(bignum in1, bignum in2) {
	bignum out;
	
	bignum a;
	bignum b;
	bn_copy(&a, in1);
	bn_copy(&b, in2);
		
	if(a.sign != b.sign) {
		//printf("here3\n");
		//printf("Really adding\n");
		//printf("%"PRId8"\n%"PRId8"\n", in1.sign, in2.sign);
		if(a.sign == 1) {
			//bignum minusIn2 = bn_minus_ret(in2);
			bn_minus_ptr(&b);
			bn_removezeros(&b);
			out = bn_add(a, b);
		} else {
			bn_minus_ptr(&a);
			bn_removezeros(&a);
			out = bn_add(a, b);
			bn_removezeros(&out);
			bn_minus_ptr(&out);
		}
		bn_destroy(&b);
		bn_destroy(&a);
		return out;
	} else if(a.sign == -1) {
		a.sign = 1;
		b.sign = 1;
		out = bn_subtract(b, a);
		bn_destroy(&a);
		bn_destroy(&b);
		return out;
	}
	
	bn_removezeros(&a);
	bn_removezeros(&b);
		
	if(a.noBlocks<b.noBlocks) {
		//printf("here1\n");
		//printf("second is bigger\n");
		out = bn_subtract(b, a);
		bn_minus_ptr(&out);
		bn_destroy(&a);
		bn_destroy(&b);
		return out;
	} else if(a.noBlocks == b.noBlocks) {
		//printf("here2\n");
		int8_t switchy = 1;
		for(uint32_t i = a.noBlocks-1; i>0; i--) {
			if(a.blocks[i]<b.blocks[i]) {
				//printf("%"PRIu8" < %"PRIu8"\n", in1.blocks[i],in2.blocks[i]);
				out = bn_subtract(b, a);
				bn_minus_ptr(&out);
				bn_destroy(&a);
				bn_destroy(&b);
				return out;
			} else if(a.blocks[i]>b.blocks[i]){
				switchy = -1;
				break;
			}
		}
		if(a.blocks[0]<b.blocks[0] && switchy==1) {
			//printf("second is bigger\n");
			out = bn_subtract(b, a);
			bn_minus_ptr(&out);
			bn_destroy(&a);
			bn_destroy(&b);
			return out;
		}
	}
	
	//printf("sub 1 : ");bn_prnt_dec(a);
	//printf("sub 2 : ");bn_prnt_dec(b);
	
	//printf("Subtracting\n");
	
	bn_init(&out, a.noBlocks);
	
	int8_t remainder = 0;
	for(uint32_t i = 0; i<b.noBlocks; i++) {
		
		int16_t temp = (int16_t)a.blocks[i] - (int16_t)b.blocks[i]-remainder;
		//printf("%"PRId8" - %"PRId8" - %"PRId8" = %"PRId16"\n", (int16_t)in1.blocks[i], (int16_t)in2.blocks[i],remainder, temp);
		remainder = 0;
		if(temp<0) {
			temp+=256;
			remainder = 1;
		}
		out.blocks[i] = temp;
	}
	for(uint32_t i=b.noBlocks; i<a.noBlocks; i++) {
		int16_t temp = (int16_t)a.blocks[i] - remainder;
		remainder = 0;
		if(temp < 0) {
			temp+=256;
			remainder = 1;
		}
		out.blocks[i] = temp;
	}
	
	bn_removezeros(&out);
	
	bn_destroy(&a);
	bn_destroy(&b);
	
	return out;
}

void bn_sub_1(bignum *in) {
	bn_removezeros(in);
	if(in->sign == -1) {
		bn_add_1(in);
		return;
	}
	if(bn_equals((*in), ZERO)) {
		in->sign = -1;
		bn_add_1(in);
		return;
	}
	uint32_t remainder = 1; 
	uint32_t j = 0;
	while(remainder>0) {
		remainder = 0;
		if(in->blocks[j] == 0) {
			in->blocks[j++] = 255;
			remainder = 1;
		} else {
			in->blocks[j++]--;
		}
	}
}

bignum bn_minus_ret(bignum in) {
	bignum out;
	bn_copy(&out, in);
	out.sign = -1*in.sign;
	return out;
}

void bn_minus_ptr(bignum *in) {
	(*in).sign = -1*(*in).sign;
}

void bn_abs(bignum *in) {
	(*in).sign = 1;
}

bignum bn_gcd(bignum in1, bignum in2) {
	bignum a, b, remainder;
	bn_copy(&a, in1);
	bn_copy(&b, in2);
	
	if(bn_equals(a, ZERO)) {
		bn_destroy(&a);
		return b;
	}
	
	//printf("\n");
	//printDecBignum(a);
	//printDecBignum(b);
	
	while(!bn_equals(b, ZERO)) {
		remainder = bn_div_r(a, b);
		bn_destroy(&a);
		bn_copy(&a, b);
		bn_destroy(&b);
		bn_copy(&b, remainder);
		bn_destroy(&remainder);
		
		//printf("\n");
		//printDecBignum(a);
		//printDecBignum(b);
	}
	//printf("\n");
	//printDecBignum(a);
	//printDecBignum(b);
	//printf("\n");
	bn_destroy(&b);
	return a;
}

bignum bn_gcd2(bignum in1, bignum in2) {
	bignum u, v, temp;
	bn_copy(&u, in1);
	bn_copy(&v, in2);
	bn_removezeros(&u);
	bn_removezeros(&v);
	
	if(bn_equals(u, ZERO)) {
		bn_destroy(&u);
		return v;
	}
	if(bn_equals(v, ZERO)) {
		bn_destroy(&v);
		return u;
	}
	
	uint32_t shift1 = bn_div_2s(&u);
	uint32_t shift2 = bn_div_2s(&v);
	uint32_t shift = bn_min_ui(shift1, shift2);
	
	
	do {
		bn_div_2s(&v);
		
		if(bn_compare(u, v)>0) {
			bn_copy(&temp, v);
			bn_destroy(&v);
			bn_copy(&v, u);
			bn_destroy(&u);
			bn_copy(&u, temp);
			bn_destroy(&temp);
			
			//v.blocks = u.blocks;
			
		}
		
		temp = bn_subtract(v, u);
		bn_destroy(&v);
		bn_copy(&v, temp);
		bn_destroy(&temp);
		
		bn_removezeros(&v);
		bn_removezeros(&u);
		//bn_prnt_dec(u);
		//bn_prnt_dec(v);
		//exit(1);
	} while(!bn_equals(v, ZERO));
	
	bn_destroy(&v);
	
	bn_bitshift(&u, shift);
	
	return u;
}

bignum bn_sqrt(bignum in) {	
	if(in.noBlocks == 1) {
		bignum out;
		bn_init(&out, 1);
		out.blocks[0] = sqrt(in.blocks[0]);
		return out;
	}
	
	bignum out;
	
	bignum x;
	
	if(in.noBlocks%2 != 0) {
		bn_init(&x, (in.noBlocks-1)>>1);
		x.blocks[x.noBlocks-1] = 64;
	} else {
		bn_init(&x, (in.noBlocks)>>1);
		x.blocks[x.noBlocks-1] = 4;
	}
	
	bn_removezeros(&x);
	
	int j=0;
	
	while(1) {
		bignum q = bn_div_q(in, x);
		bignum temp = bn_add(x, q);
		bn_destroy(&q);
		bn_div_2(&temp);
		bn_removezeros(&temp);
		
		if(bn_equals(temp, x)) {
			break;
		}
		
		bn_destroy(&x);
		bn_copy(&x, temp);
		bn_destroy(&temp);
		
		//printDecBignum(x);
		j++;
		if(j>=2*in.noBlocks) {
			printf("%sTaking too long to converge; just giving you what I got%s\n", RED, NORMAL);
			sleep(5);
			break;
		}
	}
	
	bn_copy(&out, x);
	
	bn_destroy(&x);
	
	bn_removezeros(&out);
	
	//printDecBignum(out);
	
	return out;
}

uint32_t bn_log2(bignum a) {
	uint32_t out;
	bignum b;
	bn_copy(&b, a);
	bn_removezeros(&b);
	
	out = (b.noBlocks-1)*8;
	out += floor(log2(b.blocks[b.noBlocks-1]));
	
	bn_destroy(&b);
	return out;
}

uint32_t bn_int_eratosthenes(uint32_t limit, uint32_t **primes) {
	uint32_t srt = sqrt(limit);
	uint32_t noPrimes = 0;
	
	uint8_t *checks = malloc(limit);
	for(uint32_t i = 0; i<limit; i++) {
		checks[i] = 1;
	}
	for(uint32_t i = 2; i<=srt; i++) {
		if(checks[i]!=1) continue;
		noPrimes++;
		for(uint32_t j=i; j*i<limit; j++) {
			checks[i*j] = 0;
		}
	}
	
	for(uint32_t i = srt+1; i<limit; i++) {
		if(checks[i]==1) noPrimes++;
	}
	
	(*primes) = malloc(sizeof(uint32_t)*noPrimes);
	uint32_t j = 0;
	
	for(uint32_t i = 2; i<limit; i++) {
		if(checks[i] == 1) {
			(*primes)[j++] = i;
		}
	}
	
	free(checks);
	
	return noPrimes;
}
