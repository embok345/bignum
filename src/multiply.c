#include "bignum.h"


/*bignum bn_mul(bignum in1, bignum in2) {
// TODO:figure out which algorithms to use
	bignum a, b;
	bn_copy(&a, in1);
	bn_copy(&b, in2);
	bn_removezeros(&a);
	bn_removezeros(&b);
	bignum in[2] = {a, b};
	bignum out;
	
	if(bn_min_ui(a.noBlocks, b.noBlocks) <= 50) {
		out = bn_mul_long(in);
	} else {
		out = bn_mul_karat(in);
	}
	out.sign = a.sign*b.sign;
	
	bn_destroy(&a);
	bn_destroy(&b);
	
	return out;
}

bignum bn_pow_i(bignum x, int32_t exponent) {
	bignum out, temp;
	
	if(exponent == 0) {
		bn_copy(&out, ONE);
		return out;
	}
	if(exponent == 1) {
		bn_copy(&out, x);
		return out;
	}
	if(exponent%2 == 0) {
		temp = bn_pow_i(x, exponent/2);
		out = bn_mul(temp, temp);
		bn_destroy(&temp);
		return out;
	} else {
		temp = bn_pow_i(x, exponent-1);
		out = bn_mul(temp, x);
		bn_destroy(&temp);
		return out;
	}
	
}

bignum bn_powmod_bn(bignum x, bignum exponent, bignum modulus) {
	bignum in, e, mod, out, temp;
	bn_copy(&in, x);
	bn_copy(&e, exponent);
	bn_copy(&mod, modulus);
	
	if(bn_equals(mod, ONE) || bn_equals(mod, ZERO)) {
		bn_copy(&out, ZERO);
		bn_destroy(&in);
		bn_destroy(&e);
		bn_destroy(&mod);
		return out;
	}
	if(bn_equals(e, ZERO)) {
		bn_copy(&out, ONE);
		bn_destroy(&in);
		bn_destroy(&e);
		bn_destroy(&mod);
		return out;
	}
	if(bn_equals(e, ONE)) {
		out = bn_div_r(in, mod);
		bn_destroy(&e);
		bn_destroy(&mod);
		bn_destroy(&in);
		return out;
		
	}
	
	temp = bn_div_r(in, mod);
	bn_destroy(&in);
	bn_copy(&in, temp);
	bn_destroy(&temp);
	bn_removezeros(&in);
	bn_copy(&out, ONE);
	
	while(!bn_equals(e, ZERO)) {
		if(!bn_iseven(e)) {
			temp = bn_mul(out, in);
			bn_destroy(&out);
			out = bn_div_r(temp, mod);
			bn_destroy(&temp);
		}
		bn_div_2(&e);
		temp = bn_mul(in, in);
		bn_destroy(&in);
		in = bn_div_r(temp, mod);
		
		bn_destroy(&temp);
	}
	
	bn_destroy(&in);
	bn_destroy(&e);
	bn_destroy(&mod);
	
	return out;
}

bignum bn_powmod_i(bignum x, int32_t exponent, bignum modulus) {
	
	bignum in, mod, out, temp;
	bn_copy(&in, x);
	bn_removezeros(&in);
	bn_copy(&mod, modulus);
	
	if(bn_equals(mod, ONE) || bn_equals(mod, ZERO)) {
		bn_copy(&out, ZERO);
		bn_destroy(&in);
		bn_destroy(&mod);
		return out;
	}
	
	if(exponent == 0) {
		bn_copy(&out, ONE);
		bn_destroy(&in);
		bn_destroy(&mod);
		return out;
	}
	
	temp = bn_div_r(in, modulus);
	bn_destroy(&in);
	bn_copy(&in, temp);
	bn_destroy(&temp);
	bn_removezeros(&in);
	bn_copy(&out, ONE);
	
	while(exponent>0) {
		if(exponent%2 == 1) {
			//printf("Here\n");
			temp = bn_mul(out, in);
			bn_destroy(&out);
			out = bn_div_r(temp, mod);
			bn_destroy(&temp);
		}
		//printf("a\n");
		exponent >>= 1;
		temp = bn_mul(in, in);
		bn_destroy(&in);
		in = bn_div_r(temp, mod);
		
		bn_destroy(&temp);
	}
	
	bn_destroy(&in);
	bn_destroy(&mod);
	return out;
}

bignum bn_mul_long(bignum in[]) {
	uint32_t productLength = in[0].noBlocks+in[1].noBlocks;
	bignum out;
	bn_init(&out, productLength);
	
	uint8_t smaller;
	
	if(in[0].noBlocks<in[1].noBlocks) {
		smaller = 0;
	} else {
		smaller = 1;
	}
	
	bignum temp;
	
	for(uint32_t i = 0; i<in[smaller].noBlocks; i++) {
		temp = bn_mul_byte(in[1-smaller], in[smaller].blocks[i]);
		if(i>0) 
			bn_blockshift(&temp, i);
		bignum temp2;
		bn_copy(&temp2, out);
		bn_destroy(&out);
		bn_removezeros(&temp);
		bn_removezeros(&temp2);
		out = bn_add(temp, temp2); 
		bn_destroy(&temp);
		bn_destroy(&temp2);
	}
	
	bn_removezeros(&out);
	
	return out;
}

bignum bn_mul_karat(bignum in[]) {
	//printf("karat called\n\n\n");
	
	bignum a, b;
	bn_copy(&a, in[0]);
	bn_copy(&b, in[1]);
	a.sign = 1;
	b.sign = 1;
	
	uint32_t m = ceil((float)bn_max_ui(a.noBlocks, b.noBlocks)/2);
	
	bignum up[2], down[2];
	
	if(a.noBlocks <= m) {
		
		bn_copy(&up[0], ZERO);
		bn_copy(&down[0], a);
		
		up[1] = bn_bigblocks(b, b.noBlocks-m);
		down[1] = bn_littleblocks(b, m);

	} else if(b.noBlocks <= m) {
		bn_copy(&up[1], ZERO);
		bn_copy(&down[1], b);
		
		up[0] = bn_bigblocks(a, a.noBlocks-m);
		down[0] = bn_littleblocks(a, m);
	} else {
		
		up[0] = bn_bigblocks(a, a.noBlocks-m);
		down[0] = bn_littleblocks(a, m);
		
		up[1] = bn_bigblocks(b, b.noBlocks-m);
		down[1] = bn_littleblocks(b, m);
	}
	
	bignum z[3];
	
	bn_removezeros(&down[0]);
	bn_removezeros(&down[1]);
	bn_removezeros(&up[1]);
	bn_removezeros(&up[0]);
	
	z[0] = bn_mul(down[0],down[1]);
	
	z[2] = bn_mul(up[0],up[1]);
	
	bignum temp1 = bn_subtract(down[0],up[0]);
	bignum temp2 = bn_subtract(up[1],down[1]);
	
	bn_removezeros(&temp1);
	bn_removezeros(&temp2);

	bn_destroy(&up[0]);
	bn_destroy(&up[1]);
	bn_destroy(&down[0]);
	bn_destroy(&down[1]);
	
	
	bignum temp = bn_mul(temp1, temp2);

	bn_destroy(&temp1);
	bn_destroy(&temp2);
	
	bn_removezeros(&z[0]);
	bn_removezeros(&z[2]);
	
	temp2 = bn_add(z[0],z[2]);
	
	bn_removezeros(&temp);
	bn_removezeros(&temp2);
	
	z[1] = bn_add(temp, temp2);

	bn_destroy(&temp);
	bn_destroy(&temp2);
	
	bn_blockshift(&z[2], 2*m);
	bn_blockshift(&z[1], m);
	
	bignum out;
	
	bn_removezeros(&z[1]);
	bn_removezeros(&z[2]);
	
	temp = bn_add(z[1],z[2]);
	
	bn_removezeros(&temp);
	bn_removezeros(&z[0]);

	out = bn_add(z[0],temp);
	
	bn_destroy(&temp);
	bn_destroy(&z[0]);
	bn_destroy(&z[1]);
	bn_destroy(&z[2]);
	bn_destroy(&a);
	bn_destroy(&b);
	
	bn_removezeros(&out);

	/*printf("karat done:\n");
	printf("a = ");bn_prnt_dec(in[0]);printf("\n");
	printf("b = ");bn_prnt_dec(in[1]);printf("\n");
	printf("out = ");bn_prnt_dec(out);printf("\n");
	printf("---------------------------------\n\n");
	
	return out;
}


bignum bn_mul_tc3(bignum in[]) {
//TODO
	bignum a, b;
	bn_copy(&a, in[0]);
	bn_copy(&b, in[1]);
	bn_removezeros(&a);
	bn_removezeros(&b);
	uint32_t i = bn_max_ui(a.noBlocks/3, b.noBlocks/3) + 1;
	
	if(2*i >= a.noBlocks || 2*i>=b.noBlocks) {
		printf("one is shorter than the other\n");
		exit(1);
	}
	if(a.noBlocks > 3*i || b.noBlocks > 3*i) {
		printf("ONe is too long\n");
		exit(1);
	}
	
	bignum p, p0, p1, pm1, pm2, pinf, q, q0, q1, qm1, qm2, qinf, temp;
	
	bignum middle[2];
	
	p0 = bn_littleblocks(a, i);
	q0 = bn_littleblocks(b, i);
	pinf = bn_bigblocks(a, a.noBlocks-2*i);
	qinf = bn_bigblocks(b, b.noBlocks-2*i);
	
	bn_init(&middle[0], i);
	bn_init(&middle[1], i);
	
	for(uint32_t j = 0; j<i; j++) {
		middle[0].blocks[j] = a.blocks[j+i];
		middle[1].blocks[j] = b.blocks[j+i];
	}
	
	printf("a blocks = ");bn_prnt_blocks(a);
	printf("a top blocks = ");bn_prnt_blocks(pinf);
	printf("a middle blocks = ");bn_prnt_blocks(middle[0]);
	printf("a bottom blocks = ");bn_prnt_blocks(p0);
	
	printf("b blocks = ");bn_prnt_blocks(b);
	printf("b top blocks = ");bn_prnt_blocks(qinf);
	printf("b middle blocks = ");bn_prnt_blocks(middle[1]);
	printf("b bottom blocks = ");bn_prnt_blocks(q0);
	
	bn_destroy(&a);
	bn_destroy(&b);
	
	printf("\np0 = ");bn_prnt_dec(p0);
	printf("pinf = ");bn_prnt_dec(pinf);
	printf("pmid = ");bn_prnt_dec(middle[0]);
	
	p = bn_add(p0, pinf);
	printf("p = p0+pinf = ");bn_prnt_dec(p);
	p1 = bn_add(p, middle[0]);
	printf("p1 = p+pmid = ");bn_prnt_dec(p1);
	pm1 = bn_subtract(p, middle[0]);
	printf("pm1 = p-pmid = ");bn_prnt_dec(pm1);
	temp = bn_add(pm1, pinf);
	bn_mul_2(&temp);
	pm2 = bn_subtract(temp, p0);
	printf("pm2 = 2(pm1 + pinf) - p0 = ");bn_prnt_dec(pm2);
	bn_destroy(&temp);
	bn_destroy(&p);
	
	printf("\nq0 = ");bn_prnt_dec(q0);
	printf("qinf = ");bn_prnt_dec(qinf);
	printf("qmid = ");bn_prnt_dec(middle[1]);
	q = bn_add(q0, qinf);
	printf("q = q0+qinf = ");bn_prnt_dec(q); // it's here where the problem occurs
	exit(1);
	q1 = bn_add(q, middle[1]);
	printf("q1 = q+qmid = ");bn_prnt_dec(q1);
	qm1 = bn_subtract(q, middle[1]);
	printf("qm1 = q-qmid = ");bn_prnt_dec(qm1);
	temp = bn_add(qm1, qinf);
	bn_mul_2(&temp);
	qm2 = bn_subtract(temp, q0);
	printf("qm2 = 2(qm1 + qinf) - q0 = ");bn_prnt_dec(qm2);
	bn_destroy(&temp);
	bn_destroy(&q);
	
	bn_destroy(&middle[0]);
	bn_destroy(&middle[1]);
	
	bignum r1, rm1, rm2, r[5];
	
	printf("\np0 = ");bn_prnt_dec(p0);
	printf("q0 = ");bn_prnt_dec(q0);
	printf("p1 = ");bn_prnt_dec(p1);
	printf("q1 = ");bn_prnt_dec(q1);
	printf("pm1 = ");bn_prnt_dec(pm1);
	printf("qm1 = ");bn_prnt_dec(qm1);
	printf("pm2 = ");bn_prnt_dec(pm2);
	printf("qm2 = ");bn_prnt_dec(qm2);
	printf("pinf = ");bn_prnt_dec(pinf);
	printf("qinf = ");bn_prnt_dec(qinf);
	
	r[0] = bn_mul(p0, q0);
	r1 = bn_mul(p1, q1);
	rm1 = bn_mul(pm1, qm1);
	rm2 = bn_mul(pm2, qm2);
	r[4] = bn_mul(pinf, qinf);
	
	printf("\nr0 = ");bn_prnt_dec(r[0]);
	printf("r1 = ");bn_prnt_dec(r1);
	printf("rm1 = ");bn_prnt_dec(rm1);
	printf("rm2 = ");bn_prnt_dec(rm2);
	printf("rinf = ");bn_prnt_dec(r[4]);
	
	bn_destroy(&p0);
	bn_destroy(&p1);
	bn_destroy(&pm1);
	bn_destroy(&pm2);
	bn_destroy(&pinf);
	
	bn_destroy(&q0);
	bn_destroy(&q1);
	bn_destroy(&qm1);
	bn_destroy(&qm2);
	bn_destroy(&qinf);
	
	//printf("r0 = ");bn_prnt_dec(r[0]);
	//printf("r1 = ");bn_prnt_dec(r1);
	//printf("rm1 = ");bn_prnt_dec(rm1);
	//printf("rm2 = ");bn_prnt_dec(rm2);
	//printf("rinf = ");bn_prnt_dec(r[4]);
	
	temp = bn_subtract(rm2, r1);
	printf("\nrm2 - r1 = ");bn_prnt_dec(temp);
	r[3] = bn_div_byteq(temp, 3);
	printf("r[3] = (rm2 - r1)/3 = ");bn_prnt_dec(r[3]);
	bn_destroy(&temp);
	r[1] = bn_subtract(r1, rm1);
	printf("r1 - rm1 = ");bn_prnt_dec(r[1]);
	bn_div_2(&r[1]);
	printf("r[1] = (r1-rm1)/2 = ");bn_prnt_dec(r[1]);
	r[2] = bn_subtract(rm1, r[0]);
	printf("r[2] = rm1-r0 = ");bn_prnt_dec(r[2]);
	temp = bn_subtract(r[2], r[3]);
	printf("r[2]-r[3] = ");bn_prnt_dec(temp);
	bn_div_2(&temp);
	bn_destroy(&r[3]);
	bn_mul_2(&r[4]);
	r[3] = bn_add(temp, r[4]);
	printf("r[3] = (r[2]-r[3])/2 + 2rinf = ");bn_prnt_dec(r[3]);
	bn_div_2(&r[4]);
	bn_destroy(&temp);
	temp = bn_add(r[2], r[1]);
	bn_destroy(&r[2]);
	r[2] = bn_subtract(temp, r[4]);
	printf("r[2] = r[2]+r[1]-r[4] = ");bn_prnt_dec(r[2]);
	bn_destroy(&temp);
	temp = bn_subtract(r[1], r[3]);
	bn_destroy(&r[1]);
	bn_copy(&r[1], temp);
	printf("r[1] = r[1]-r[3] = ");bn_prnt_dec(r[1]);
	
	bn_destroy(&temp);
	bn_destroy(&r1);
	bn_destroy(&rm1);
	bn_destroy(&rm2);
	
	bignum out;
	bn_copy(&out, r[4]);
	bn_destroy(&r[4]);
	for(uint32_t j = 0; j<4; j++) {
		bn_blockshift(&out, i);
		temp = bn_add(r[3-j], out);
		bn_destroy(&out);
		bn_copy(&out, temp);
		bn_destroy(&temp);
		bn_destroy(&r[3-j]);
	}

	return out;
}
*/

void bn_mul_byte(bignum in1, uint8_t in2, bignum *out) {
  bn_resize(out, in1.noBlocks);
  uint16_t temp;
  uint8_t remainder = 0;
  for(uint32_t i=0; i<in1.noBlocks; i++) {
    temp = (uint16_t)in1.blocks[i]*(uint16_t)in2 + remainder;
    out->blocks[i] = temp%256;
    remainder = temp>>8;
  }
  if(remainder!=0) {
    bn_addblock(out);
    out->blocks[out->noBlocks-1] = remainder;
  }
}

void bn_mul_10(bignum *num) {
  uint8_t remainder = 0;
  for(uint32_t i=0; i<num->noBlocks; i++) {
    uint16_t temp = 10*(num->blocks[i])+remainder;
    num->blocks[i] = temp%256;
    remainder = temp>>8;
  }
  if(remainder!=0) {
    bn_addblock(num);
    num->blocks[num->noBlocks-1] = remainder;
  }
}

void bn_mul_2(bignum *num) {
  uint8_t remainder = 0;
  for(uint32_t i=0; i<(*num).noBlocks; i++) {
    uint16_t temp = (((uint16_t)((*num).blocks[i]))<<1) + remainder;
    (*num).blocks[i] = temp%256;
    remainder = temp>>8;
  }
  if(remainder!=0) {
    bn_addblock(num);
    num->blocks[(*num).noBlocks-1] = remainder;
  }
}
