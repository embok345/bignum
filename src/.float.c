#include "bignum.h"

void bn_float_init(bn_float_t *in) {
	bn_init(&(*in).n, 1);
	(*in).a = 0;
}
void bn_float_destroy(bn_float_t *in) {
	bn_destroy(&(*in).n);
	(*in).a = 0;
}

bn_float_t bn_float_add(bn_float_t a, bn_float_t b) {
	bn_float_t out;
	int64_t f = bn_min_sl(a.a, b.a);
	
	out.a = f;
	
	bignum temp1, temp2;
	
	bn_copy(&temp1, a.n);
	bn_removezeros(&temp1);
	bn_bitshift(&temp1, a.a-f);
	
	bn_copy(&temp2, b.n);
	bn_removezeros(&temp2);
	bn_bitshift(&temp2, b.a-f);
	
	out.n = bn_add(temp1, temp2);
	
	bn_destroy(&temp1);
	bn_destroy(&temp2);
	
	while(bn_iseven(out.n) && !bn_equals(out.n, ZERO)) {
		out.a++;
		bn_div_2(&out.n);
	}
	
	return out;
}

bn_float_t bn_float_sub(bn_float_t a, bn_float_t b) {
	bn_float_t out;
	int64_t f = bn_min_sl(a.a, b.a);
	out.a = f;
	
	bignum temp1, temp2;
	
	bn_copy(&temp1, a.n);
	bn_removezeros(&temp1);
	bn_copy(&temp2, b.n);
	bn_removezeros(&temp2);
	
	bn_bitshift(&temp1, a.a-f);
	bn_bitshift(&temp2, b.a-f);
	
	out.n = bn_subtract(temp1, temp2);

	bn_destroy(&temp1);
	bn_destroy(&temp2);
	
	while(bn_iseven(out.n) && !bn_equals(out.n, ZERO)) {
		out.a++;
		bn_div_2(&out.n);
	}
	
	return out;
}

bn_float_t bn_float_mul(const bn_float_t a, const bn_float_t b) {
	bn_float_t out;
	out.n = bn_mul(a.n, b.n);
	out.a = a.a+b.a;
	while(bn_iseven(out.n) && !bn_equals(out.n, ZERO)) {
		out.a++;
		bn_div_2(&out.n);
	}
	return out;
}

bn_float_t bn_float_trunc(const bn_float_t r, uint32_t b) {
	bn_float_t out;
	//printf("trunc start\n");
	out = bn_float_divb(r, 1, b);
	//printf("trunc end\n");
	return out;
}

bn_float_t bn_float_divb(const bn_float_t r, uint32_t k, uint32_t b) {
	bn_float_t out;
	uint64_t f;
	
	f = bn_log2(r.n)+1;

	out.a = r.a+f-(uint32_t)ceil(log2(k))-b;
	
	out.n = bn_div_intq(r.n, k);
		
	bn_bitshift(&out.n, -f+(uint32_t)ceil(log2(k))+b);
	
	return out;
}

bn_float_t bn_float_powb(const bn_float_t r, uint32_t k, uint32_t b) {
	bn_float_t out, temp, temp2;
	
	//printf("powb start\n");
	//printf("k = %"PRIu32", b = %"PRIu32"\n", k, b);
	
	if(k == 1) {
		//printf("powb k=1\n");
		out = bn_float_trunc(r, b);
		//printf("powb k=1 done\n");
		return out;
	} else if(k%2 == 0) {
		//printf("powb k even\n");
		out = bn_float_powb(r, k/2, b);
		//printf("powb even powb done\n");
		temp = bn_float_mul(out, out);
		//printf("powb even mul done\n");
		bn_float_destroy(&out);
		out = bn_float_trunc(temp, b);
		//printf("powb even trunc done\n");
		bn_float_destroy(&temp);
		return out;
	} else {
		//printf("powb k odd\n");
		out = bn_float_powb(r, k-1, b);
		temp = bn_float_trunc(r, b);
		temp2 = bn_float_mul(out, temp);
		bn_float_destroy(&out);
		out = bn_float_trunc(temp2, b);
		bn_float_destroy(&temp);
		bn_float_destroy(&temp2);
		//printf("powb k odd done\n");
		return out;
	}
}

bn_float_t bn_float_nrootb(const bn_float_t y, uint32_t k, uint32_t b) {
	if(b>ceil(log2(8*k))) {
		//printf("big\n");
		return bn_float_nrootb2(y, k, b);
	}
	
	//printf("Small\n");
	
	bn_float_t z, r, temp1, temp2, temp3, lower, upper;
	int32_t g, a, B;
	
	lower.a = -10;
	lower.n = bn_conv_int2bn(993);
	upper.a = 0;
	bn_copy(&(upper.n), ONE);
	
	g = bn_log2(y.n);
	if(!bn_ispowerof2(y.n)) {
		g++;	
	}
	g+=y.a;
	
	a = (int32_t)floor(-(double)g/k);
	B = (int32_t)ceil(log2(66*((2*k)+1)));
	
	//Define 2^a and 2^a-1, and set z = 2^a + 2^a-1
	temp1.a = a;
	bn_copy(&temp1.n, ONE);
	temp2.a = a-1;
	bn_copy(&temp2.n, ONE);
	
	z = bn_float_add(temp1, temp2);
	
	//printf("z = (%"PRId64", %s)\n", z.a, bn_conv_bn2str(z.n));
	
	bn_destroy_float(&temp1);
	bn_destroy_float(&temp2);
	
	//printf("%"PRIu32"\n", b);
	
	for(uint32_t j = 1; j<b; j++) {
		//printf("%"PRIu32"\n", j);
		temp1 = bn_float_powb(z, k, B);
		temp2 = bn_float_trunc(y, B);
		temp3 = bn_float_mul(temp1, temp2);	
	    r = bn_float_trunc(temp3, B);
	    bn_destroy_float(&temp1);
	    bn_destroy_float(&temp2);
	    bn_destroy_float(&temp3);
	    if(bn_compare_float(r, lower)<=0) {
	    	//printf("lower\n");
	    	temp1.a = (int64_t)a-j-1;
	    	bn_copy(&temp1.n, ONE);
	    	temp2 = bn_float_add(z, temp1);
	    	bn_destroy_float(&z);
	    	bn_copy_float(&z, temp2);
	    	bn_destroy_float(&temp1);
	  		bn_destroy_float(&temp2);
	    } else if(bn_compare_float(r, upper) == 1){
	    	//printf("upper\n");
	    	temp1.a = (int64_t)a-j-1;
	    	bn_copy(&temp1.n, ONE);
	    	temp2 = bn_float_sub(z, temp1);
	    	bn_destroy_float(&z);
	    	bn_copy_float(&z, temp2);
	    	bn_destroy_float(&temp1);
	    	bn_destroy_float(&temp2);
	    }
	    bn_destroy_float(&r);
	}
	
	//printf("done\n");
	
	bn_destroy_float(&lower);
	bn_destroy_float(&upper);
	
	//printf("Small done\n");
	
	return z;
}

bn_float_t bn_float_nrootb2(const bn_float_t y, uint32_t k, uint32_t b) {
	if(b<=ceil(log2(8*k))) {
		//printf("small\n");
		return bn_float_nrootb(y, k, b);
	}
	
	//printf("Big\n");
	
	//printf("b = %"PRIu32"\n", b);
	b = ceil(log2(k*2)) + ceil((b-ceil(log2(k*2)))/2);
	//printf("b' = %"PRIu32"\n", b);
	uint32_t B = b*2 + 4 - ceil(log2(k));
	
	bn_float_t z = bn_float_nrootb(y, k, b);
	
	//r2 = trunc(z, B) * (k+1)
	bn_float_t r2 = bn_float_trunc(z, B);
	bignum tempbn1 = bn_conv_int2bn(k+1);
	bignum tempbn2 = bn_mul(r2.n, tempbn1);
	bn_destroy(&(r2.n));
	bn_copy(&r2.n, tempbn2);
	bn_destroy(&tempbn1);
	bn_destroy(&tempbn2);
	
	//r3 = trunc(pow(z, k+1, B)*trunc(y, B), B)
	//printf("r3 start\n");
	bn_float_t temp1 = bn_float_powb(z, k+1, B);
	//printf("r3 powb\n");
	bn_float_t temp2 = bn_float_trunc(y, B);
	//printf("r3 trunc\n");
	bn_float_t temp3 = bn_float_mul(temp1, temp2);
	//printf("r3 mul\n");
	bn_float_t r3 = bn_float_trunc(temp3, B);
	//printf("r3 end\n");
	
	bn_destroy_float(&temp1);
	bn_destroy_float(&temp2);
	bn_destroy_float(&temp3);
	
	temp1=bn_float_sub(r2, r3);
	bn_float_t r4 = bn_float_divb(temp1, k, B);
	//printf("r4\n");
	
	bn_destroy_float(&temp1);
	bn_destroy_float(&r2);
	bn_destroy_float(&r3);
	bn_destroy_float(&z);
	
	//printf("(%"PRId64", %s)\n", r4.a, bn_conv_bn2str(r4.n));
	//printf("Big done\n");
	
	return r4;
}

int8_t bn_ispower_check(bignum n, bignum x, uint32_t k) {
	uint32_t f = bn_log2(n) + 1;
	int8_t ret = 0;
	uint32_t b = 1;
	bn_float_t nf, xf, r, temp1, temp2;
	bn_copy(&nf.n, n);
	bn_copy(&xf.n, x);
	nf.a = 0;
	xf.a = 0;
	while(1) {
		r = bn_float_powb(xf, k, b+ceil(log2(8*k)));
		if(bn_compare_float(nf, r) == -1) {
			ret = -1;
			break;
		}
		
		bn_copy_float(&temp1, r);
		temp1.a-=b;
		temp2 = bn_float_add(r, temp1);
		if(bn_compare_float(temp2, nf) <= 0) {
			ret = 1;
			bn_destroy_float(&temp1);
			bn_destroy_float(&temp2);
			break;
		}
		
		bn_destroy_float(&temp1);
		bn_destroy_float(&temp2);
		
		if(b>=f) {
			ret = 0;
			break;
		}
		b = bn_min_ui(2*b, f);
		bn_destroy_float(&r);
	}
	
	bn_destroy_float(&r);
	bn_destroy_float(&xf);
	bn_destroy_float(&nf);
	
	return ret;
	
}

bignum bn_root_k(bignum n, bn_float_t y, uint32_t k) {
	uint32_t f = bn_log2(n);
	uint32_t b = 4+(f/k);
	
	//printf("f = %"PRIu32"\n", f);
	
	bn_float_t r = bn_float_nrootb(y, k, b);
	
	//printf("Here\n");
	
	bignum x;
	bn_copy(&x, r.n);
	bn_bitshift(&x, r.a);
	
	//printf("x = %s\n", bn_conv_bn2str(x));
	
	if(bn_equals(x, ZERO)) {
		//printf("x = 0\n");
		bn_destroy_float(&r);
		return x;
	}
	
	bn_float_t xf;
	xf.a = 0;
	bn_copy(&xf.n, x);
	bn_float_t temp = bn_float_sub(r, xf);
	
	bn_destroy_float(&xf);
	bn_destroy_float(&r);
	
	bn_float_t quarter;
	bn_copy(&quarter.n, ONE);
	quarter.a = -2;
	if(bn_compare_float(temp, quarter)<0) {
		int8_t sign = bn_ispower_check(n, x, k);
		if(sign == 0) {
			bn_destroy_float(&quarter);
			bn_destroy_float(&temp);
			return x;
		}
	}
	
	bn_destroy_float(&quarter);
	bn_destroy_float(&temp);
	bn_destroy(&x);
	bn_copy(&x, ZERO);
	return x;
	
}

//Sometimes works sometimes doesn't. ¯\_(ツ)_/¯
int8_t bn_ispower(bignum n) {
	uint32_t f = bn_log2(n)+1;
	bignum x;
	bn_float_t nf;
	bn_copy(&nf.n, n);
	nf.a = 0;
	bn_float_t y = bn_float_nrootb(nf, 1, 4+(f/2));
	uint32_t *primes;
	uint32_t noPrimes =0;
	noPrimes = bn_int_eratosthenes(f, &primes);
	//printf("%"PRIu32"\n", noPrimes);
	for(uint32_t i = 0; i<noPrimes; i++) {
		x = bn_root_k(n, y, primes[i]);
		if(!bn_equals(x, ZERO)) {
			printf("%"PRIu32", ", primes[i]);bn_prnt_dec(x);
			bn_destroy(&x);
			bn_destroy_float(&y);
			bn_destroy_float(&nf);
			free(primes);
			return 1;
		}
		bn_destroy(&x);
	}
	printf("1, ");bn_prnt_dec(ZERO);
	free(primes);
	bn_destroy_float(&y);
	bn_destroy_float(&nf);
	return 0;
}
