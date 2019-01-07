#include "bignum.h"

/* Trial divide in by all prime powers for primes less than limit
 * TODO: figure out the best return values etc. Return just the list of prime 
 * power factors?
 */
bignum bn_factor_trial(bignum in1, uint32_t limit) {
	
	uint32_t *primes, noPrimes, p, mod;
	bignum in, temp;
	
	bn_copy(&in, in1);
	bn_removezeros(&in);
	
	//Generate primes less than limit
	noPrimes = bn_int_eratosthenes(limit, &primes);
	
	//Go through all of the generated primes
	for(uint32_t i = 0; i<noPrimes; i++) {
		p = primes[i];
		mod = bn_div_intr(in, p);
		if(mod == 0) {
			//If p is a factor of in, increase the power of p until it is no longer a factor of in
			uint32_t count = 0;
			while(mod == 0) {
				count++;
				p*=primes[i];
				mod = bn_div_intr(in, p);
			}
			//divide in by the highest power of p which divides it, and print out that prime power
			temp = bn_div_intq(in, pow(primes[i], count));
			bn_destroy(&in);
			bn_copy(&in, temp);
			bn_destroy(&temp);
			bn_removezeros(&in);
			printf("%"PRIu32", %"PRIu32"\n", primes[i], count);
		}
	}
	free(primes);
	return in;
}

/* Perform Pollard's rho factoring algorithm on n
 * See https://en.wikipedia.org/wiki/Pollard%27s_rho_algorithm for the algorithm
 * TODO: perhaps add possibility to randomise starting values, or vary the 
 * polynomial g which is used
 */
bignum bn_factor_prho(bignum n) {
	printf("prho called\n");
	bignum x, y, d, z, out, temp;
	
	uint32_t j = 0, i=0;
	
	x = bn_conv_byte2bn(2);
	y = bn_conv_byte2bn(2);
	bn_copy(&d, ONE);
	bn_copy(&out, n);
	bn_copy(&z, ONE);
	//bn_copy(&j, ZERO);
	
	bn_removezeros(&out);
	
	while(bn_equals(d, ONE)) {
		for(i=0; i<100; i++) {
			pthread_t th1, th2;
			
			struct thread_data {
				bignum num;
				bignum n;
				uint32_t id;
			} xDat;
			xDat.num = x;
			xDat.n = out;
			xDat.id = 2;
			struct thread_data yDat;
			yDat.num = y;
			yDat.n = out;
			yDat.id = 1;	
			
			//printf("Starting threads\n");
			//printf("x = ");bn_prnt_dec(x);
			//printf("y = ");bn_prnt_dec(y);
			
			pthread_create(&th1, NULL, prhoThrd, &yDat);
			pthread_create(&th2, NULL, prhoThrd, &xDat);
			
			//printf("Started threads\n");
			
			/*//x = (x^2+1) mod n
			temp = bn_mul(x, x);
			bn_add_1(&temp);
			bn_destroy(&x);
			x = bn_div_r(temp, out);
			bn_destroy(&temp);
		
			//y = (((y^2+1) mod n)^2 + 1) mod n
			temp = bn_mul(y, y);
			bn_add_1(&temp);
			bn_destroy(&y);
			y = bn_div_r(temp, out);
			bn_destroy(&temp);
			temp = bn_mul(y, y);
			bn_add_1(&temp);
			bn_destroy(&y);
			y = bn_div_r(temp, out);
			bn_destroy(&temp);*/
			
			pthread_join(th1, NULL);
			pthread_join(th2, NULL);
			
			x = xDat.num;
			y = yDat.num;
			
			//printf("Finished threads\n");
			//printf("x = ");bn_prnt_dec(x);
			//printf("y = ");bn_prnt_dec(y);
			
			//d = gcd(|x-y|, n)
			temp = bn_subtract(x, y);
			bn_abs(&temp);
			bn_removezeros(&temp);
			d = bn_mul(z, temp);
			bn_destroy(&z);
			z = bn_div_r(d, out);
			bn_removezeros(&z);
			if(bn_equals(z, ZERO)) {
				break;
			}
			bn_destroy(&temp);
			bn_destroy(&d);
			
			//exit(-1);
		
		//temp = bn_subtract(x, y);
		//bn_abs(&temp);
		//bn_removezeros(&temp);
		//bn_destroy(&d);
		//d = bn_gcd(temp, out);
		//bn_removezeros(&d);
		//bn_destroy(&temp);
		
		j++;
		}
		d = bn_gcd2(z, out);
		
	}

	printf("j = %"PRIu32"\n", j);
	
	bn_removezeros(&d);
	
	bn_destroy(&x);
	bn_destroy(&y);
	
	if(bn_equals(d, out)) {
		printf("Failed to find a factor\n");
		bn_destroy(&d);
		bn_destroy(&out);
		bn_copy(&out, ONE);
		return out;
	}
	
	bn_destroy(&out);
	
	return d;
}

bignum bn_factor_prho2(bignum in) {
	bignum n, x, x_fixed, cycle_size, d, count, temp;
	bn_copy(&n, in);
	x = bn_conv_int2bn(2);
	x_fixed = bn_conv_int2bn(2);
	cycle_size = bn_conv_int2bn(2);
	d = bn_conv_int2bn(1);
	
	while(bn_equals(d, ONE)) {
		count = bn_conv_int2bn(1);
		while(bn_compare(count, cycle_size) <= 0 && (bn_equals(d, ONE) || bn_equals(d, ZERO))) {	
			//printf("x start = ");bn_prnt_dec(x);
			//x = (x*x+1) % n;
			temp = bn_mul(x, x);
			bn_add_1(&temp);
			bn_destroy(&x); 
			x = bn_div_r(temp, n);
			//printf("x^2 + 1 mod n = ");bn_prnt_dec(x);
			bn_destroy(&temp);
			temp = bn_subtract(x, x_fixed);
			bn_abs(&temp);
			bn_destroy(&d);
			d = bn_gcd(temp, n);
			bn_destroy(&temp);
			bn_add_1(&count);
		}
		//printf("this\n");
		bn_mul_2(&cycle_size);
		bn_destroy(&x_fixed);
		bn_copy(&x_fixed, x);
		bn_destroy(&count);
		//bn_destroy(&x);
	}
	
	bn_destroy(&n);
	bn_destroy(&cycle_size);
	bn_destroy(&x_fixed);
	
	return d;
}

void *prhoThrd(void *data) {
	struct thread_data {
		bignum num;
		bignum n;
		uint32_t id;
	} *dat  = data;
	
	if(dat->id == 1) {
		//y
		bignum temp = bn_mul(dat->num, dat->num);
		bn_add_1(&temp);
		bn_destroy(&(dat->num));
		dat->num = bn_div_r(temp, dat->n);
		bn_destroy(&temp);
		temp = bn_mul(dat->num, dat->num);
		bn_add_1(&temp);
		bn_destroy(&(dat->num));
		dat->num = bn_div_r(temp, dat->n);
		bn_destroy(&temp);
		//printf("Finishing thread\n");
		//printf("y = ");bn_prnt_dec(dat->num);
	} else {
		//x
		bignum temp = bn_mul(dat->num, dat->num);
		bn_add_1(&temp);
		bn_destroy(&(dat->num));
		dat->num = bn_div_r(temp, dat->n);
		bn_destroy(&temp);
		//printf("Finishing thread\n");
		//printf("x = ");bn_prnt_dec(dat->num);
	}	
	pthread_exit(NULL);
	return data;
}