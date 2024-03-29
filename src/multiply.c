#include "bignum.h"

static void bn_mul_long(const bn_t, const bn_t, bn_t);
static void bn_mul_karat(const bn_t, const bn_t, bn_t);
static int8_t bn_mul_tc3(const bn_t, const bn_t, bn_t);

/* Multiplies the numbers in1 and in2, and stores it in out.
 * This method doesn't actually do any of the multiplying, it simply picks out
 * the correct multiplication algorithm to use, as well as ensuring the signs
 * line up, and the inputs are sorted.
 * ---------------------------------------------------------------------------
 */
void bn_mul(const bn_t in1, const bn_t in2, bn_t out) {

  //printf("%"PRId8" * %"PRId8"\n", bn_getSign(in1), bn_getSign(in2));
  LOG(BN_LOG, LOG_LEVEL_CRIT, "test");

  //If the second input is zero, the output is zero. (The inputs will be
  //swapped if the first is zero, so this always gets called eventually)
  if(bn_iszero(in2)) {
    bn_setzero(out);
    return;
  }

  uint32_t len1 = bn_trueLength(in1);
  uint32_t len2 = bn_trueLength(in2);

  //If the first is smaller than the second, swap them over and recall
  if(len1<len2) {
    bn_mul(in2, in1, out);
    return;
  }

  //If the length of the two numbers is less than 50 blocks, use long mul.
  if(len1 <= 50 && len2 <= 50) {
    //printf("long\nin1 = %B\nin2 = %B\n", in1, in2);
    //printf("long\n");
    bn_mul_long(in1, in2, out);
    //printf("%"PRId8", %"PRId8"\n", bn_getSign(in1), bn_getSign(in2));
  } else if(len1 <= 1000 && len2 <= 1000) {
    //Otherwise, use karat mul.
    //printf("karat\nin1 = %B\nin2 = %B\n", in1, in2);
    bn_mul_karat(in1, in2, out);
  } else {
    bn_mul_tc3(in1, in2, out);
  }

  //printf("done multiplaction\n");

  //printf("%"PRId8", %"PRId8"; ", bn_getSign(in1), bn_getSign(in2));

  //If either of the inputs is negative, set the output to be negative.
  if(bn_ispositive(in1) ^ bn_ispositive(in2)) bn_setnegative(out);
  else bn_setpositive(out);

  //printf("%"PRId8"\n", bn_getSign(out));

  bn_removezeros(out);
}

/* Use long multiplication on the two inputs, and save it in out. This can
 * only be called from bn_mul, so the input lengths, and order, should be
 * correct.
 * ---------------------------------------------------------------------------
 */
static void bn_mul_long(const bn_t in1, const bn_t in2, bn_t out) {

  uint32_t len1 = bn_trueLength(in1);
  uint32_t len2 = bn_trueLength(in2);

  //printf("start long: %"PRId8"\n", bn_getSign(in2));

  //Resize the output to the sum of the lengths of the inputs.
  bn_t mul, add;
  if(!bn_resize(out, len1+len2) || !bn_inits(2, &mul, &add)) return;

  //printf("resize: %"PRId8"\n", bn_getSign(in2));

  for(uint32_t i = len2-1; i>0; i--) {
    if(bn_getBlock(in2, i) == 0) {
      bn_blockshift(add, 1);
      continue;
    }
    //Multiply in1 by the next block of in2
    bn_mul_ub(in1, bn_getBlock(in2, i), mul);
    //printf("%B * %"PRIu8" = %B\n", in1, bn_getBlock(in2, i),  mul);
    //Add this two the working number
    //printf("%B + %B = ", mul, add);
    bn_add(mul, add, add);
    //printf("%B\n", add);
    //Shift the working number by 1
    bn_blockshift(add, 1);
  }

  //printf("after main loop: %"PRId8"\n", bn_getSign(in2));

  //If the final block of in2 is not 0, do the final multiply and add,
  //and store it into out.
  if(bn_getBlock(in2, 0) != 0) {
    //printf("final multiply start: %"PRId8"\n", bn_getSign(in2));
    bn_mul_ub(in1, bn_getBlock(in2, 0), mul);
    //printf("after multiply: %"PRId8"\n", bn_getSign(in2));
    //printf("%"PRId8", %"PRId8"\n", bn_getSign(mul), bn_getSign(add));
    bn_add(mul, add, out);
    //printf("final multiply end: %"PRId8"\n", bn_getSign(in2));
  } else {
  //Otherwise just copy the working number into out.
    if(!bn_clone(out, add)) return;
  }

  bn_deinits(2, &mul, &add);

  //printf("end long: %"PRId8"\n", bn_getSign(in2));
}

/* Apply the karatsuba multiplication algorithm to the two inputs, and store
 * the result in out. This is more efficient than long multiplication for
 * medium sized numbers. This should only get called from bn_mul, so the sizes
 * should be correct.
 * ---------------------------------------------------------------------------
 */
static void bn_mul_karat(const bn_t in1, const bn_t in2, bn_t out) {

  uint32_t len1 = bn_trueLength(in1);
  uint32_t len2 = bn_trueLength(in2);

  //m = Half the length of the longer number (should be in1)
  uint32_t m = ceil((float)bn_max_ui(len1, len2)/2);

  bn_t z[3];
  bn_t up[2], down[2];
  bn_t t1, t2;

  if(!bn_inits(9,&z[0],&z[1],&z[2],&up[0],&up[1],&down[0],&down[1],&t1,&t2))
      return;

  //Store the top len1-m blocks of in1 into up[0], and the bottom m blocks
  //into down[0], and similarly for in2, though in this case there could be no
  //blocks in up[1]
  bn_upperblocks(in1, len1 - m, up[0]);
  bn_lowerblocks(in1, m, down[0]);
  bn_upperblocks(in2, bn_max_si(0, (int32_t)len2-m), up[1]);
  bn_lowerblocks(in2, m, down[1]);

  bn_removezeros(down[0]);
  bn_removezeros(down[1]);
  bn_removezeros(up[0]);
  bn_removezeros(up[1]);

  //Recursively use bn_mul to multiply the tops and bottoms of the numbers.
  //Eventually this will reach a point at which it uses long multiplication.
  bn_mul(down[0],down[1], z[0]);
  bn_mul(up[0],up[1], z[2]);

  //printf("Done up and downs\n");

  //Now compute (u0+d0)*(u1+d1) - z0 - z2 = z1
  bn_add(up[0], down[0], t1);
  bn_add(up[1], down[1], t2);
  bn_mul(t1, t2, z[1]);
  bn_sub(z[1], z[0], z[1]);
  bn_sub(z[1], z[2], z[1]);

  //printf("Done middles\n");

  //Shift z2 by 2m, z1 by m, and then add all of z2, z1, z0, for the result.
  //printf("z[2] = %B\nz[1] = %B\nz[0] = %B\n", z[2], z[1], z[0]);
  //printf("m = %"PRIu32"\n", m);
  bn_blockshift(z[2], m);
  //printf("done blockshift\n");
  //printf("z[2]<<m = %B\n", z[2]);
  //printf("doing adding\n");
  bn_add(z[2], z[1], out);
  //printf("done adding\n");
  bn_blockshift(out, m);
  bn_add(out, z[0], out);

  //printf("Done shifting\n");

  bn_deinits(9,&up[0],&up[1],&down[0],&down[1],&t1,&t2,&z[0],&z[1],&z[2]);
}

/* Performs Toom-Cook-3 multiplication on the two arguments. This method
 * should be faster than Karatsuba muliplication on sufficiently large inputs.
 * ----------------------------------------------------------------------------
 * const bn_t in1 -- The numbers to be multiplied together.
 * const bn_t in2 --                 -"-
 * bn_t out       -- The result of the multiplication.
 *
 * return         -- 1 if the mulipication was successful, 0 otherwise.
 */
static int8_t bn_mul_tc3(const bn_t in1, const bn_t in2, bn_t out) {

    const int8_t k = 3;
    const bn_t in[2] = {in1, in2};
    const size_t len[2] = {bn_length(in[0]), bn_length(in[1])};

    /* Get the block size of the multiplication. in1 must be larger, as this is
     * ensured by bn_mul. */
    int32_t block_size = 1 + (len[0] / k);

    int32_t block_sizes[2][3];
    /* The first two blocks from in1 are of full length, the third is the
     * remaining length (which must be +ve by definition). */
    block_sizes[0][0] = block_size;
    block_sizes[0][1] = block_size;
    block_sizes[0][2] = len[0] - (2 * block_size);

    if(len[1] <= block_size) {
        /* If the length of in2 is less than the block size, only the first
         * block is non-zero. */
        block_sizes[1][0] = len[1];
        block_sizes[1][1] = 0;
        block_sizes[1][2] = 0;
    } else if(len[1] <= 2 * block_size) {
        /* If the lenth of in2 is less than twice the block size, only the
         * first two blocks are non-zero. */
        block_sizes[1][0] = block_size;
        block_sizes[1][1] = len[1] - block_size;
        block_sizes[1][2] = 0;
    } else {
        /* Otherwise, all of the blocks are non-zero. */
        block_sizes[1][0] = block_size;
        block_sizes[1][1] = block_size;
        block_sizes[1][2] = len[1] - (2 * block_size);
    }

    /* Get all of the blocks. */
    bn_t in_blocks[2][3];
    for(int i = 0; i < 2; i++) {
        for(int j = 0; j < 3; j++) {
            if(!bn_init(&in_blocks[i][j])) return 0;
            bn_innerblocks(in[i], block_sizes[0][j], j * block_size,
                in_blocks[i][j]);
        }
    }

    /* Generate the points on the polynomial. */
    bn_t p0, q0, p[5], q[5];
    if(!bn_inits(2, &p0, &q0)) return 0;
    for(int i = 1; i < 4; i++) {
        if(!bn_inits(2, &p[i], &q[i])) return 0;
    }

    /* p0 = in[0][0] + in[0][2] */
    bn_add(in_blocks[0][0], in_blocks[0][2], p0);
    /* p(0) = in[0][0] */
    p[0] = in_blocks[0][0];
    /* p(1) = p0 + in[0][1] */
    bn_add(p0, in_blocks[0][1], p[1]);
    /* p(-1) = p0 - in[0][1] */
    bn_sub(p0, in_blocks[0][1], p[2]);
    /* p(-2) = (p(-1) + in[0][2]) * 2 - in[0][0] */
    bn_add(p[2], in_blocks[0][2], p[3]);
    bn_mul_ub(p[3], 2, p[3]);
    bn_sub(p[3], in_blocks[0][0], p[3]);
    /* p(inf) = in[0][2] */
    p[4] = in_blocks[0][2];

    /* q0 = in[1][0] + in[1][2] */
    bn_add(in_blocks[1][0], in_blocks[1][2], q0);
    /* q(0) = in[1][0] */
    q[0] = in_blocks[1][0];
    /* q(1) = q0 + in[1][1] */
    bn_add(q0, in_blocks[1][1], q[1]);
    /* q(-1) = q0 - in[1][1] */
    bn_sub(q0, in_blocks[1][1], q[2]);
    /* q(-2) = (q(-1) + in[1][2]) * 2 - in[1][0] */
    bn_add(q[2], in_blocks[1][2], q[3]);
    bn_mul_ub(q[3], 2, q[3]);
    bn_sub(q[3], in_blocks[1][0], q[3]);
    /* q(inf) = in[1][2] */
    q[4] = in_blocks[1][2];

    /* Multiply the points. */
    bn_t r[5];
    for(int i = 0; i < 5; i++) {
        if(!bn_init(&r[i])) return 0;
        bn_mul(p[i], q[i], r[i]);
    }

    /* Interpolate the points. */
    bn_t out_blocks[5];
    out_blocks[0] = r[0];
    out_blocks[4] = r[4];
    for(int i = 1; i < 4; i++) {
        if(!bn_init(&out_blocks[i])) return 0;
    }
    /* out[3] = (r(-2) - r(1)) / 3 */
    bn_sub(r[3], r[1], out_blocks[3]);
    bn_div_ub(out_blocks[3], 3, out_blocks[3]);
    /* out[1] = (r(1) - r(-1)) / 2 */
    bn_sub(r[1], r[2], out_blocks[1]);
    bn_div_ub(out_blocks[1], 2, out_blocks[1]);
    /* out[2] = r(-1) - r(0) */
    bn_sub(r[2], r[0], out_blocks[2]);
    /* out[3] = ((out[2] - out[3]) / 2) + 2 * r(inf) */
    bn_sub(out_blocks[2], out_blocks[3], out_blocks[3]);
    bn_div_ub(out_blocks[3], 2, out_blocks[3]);
    bn_add(out_blocks[3], r[4], out_blocks[3]);
    bn_add(out_blocks[3], r[4], out_blocks[3]);
    /* out[2] = out[2] + out[1] - out[4] */
    bn_add(out_blocks[2], out_blocks[1], out_blocks[2]);
    bn_sub(out_blocks[2], out_blocks[4], out_blocks[2]);
    /* out[1] = out[1] - out[3] */
    bn_sub(out_blocks[1], out_blocks[3], out_blocks[1]);

    /* Combine the out blocks. */
    if(!bn_clone(out, out_blocks[4])) return 0;

    for(int i = 0; i < 4; i++) {
        /* Shift the current output, then add the next block. */
        bn_blockshift(out, block_size);
        bn_add(out, out_blocks[3-i], out);
    }
    bn_removezeros(out);

    /* Cleanup */
    bn_deinits(2, &p0, &q0);
    for(int i = 0; i < 2; i++) {
        for(int j = 0; j < 3; j++) {
            bn_deinit(&in_blocks[i][j]);
        }
    }
    for(int i = 1; i < 4; i++)
        bn_deinits(3, &p[i], &q[i], &out_blocks[i]);
    for(int i = 0; i < 5; i++)
        bn_deinit(&r[i]);

    return 1;
}

/* Multiply the bignum in1 by the byte in2, and store the result in out */
void bn_mul_ub(const bn_t in1, uint8_t in2, bn_t out) {
  //If either input is zero, the output is zero
  if(in2 == 0 || bn_iszero(in1)) {
    bn_setzero(out);
    return;
  }

  uint32_t len = bn_length(in1);
  if(!bn_resize(out, len)) return;
  uint16_t temp;
  uint8_t remainder = 0;
  //Go through the blocks multiplying each by the byte.
  for(uint32_t i=0; i<len; i++) {
    temp = (uint16_t)bn_getBlock(in1, i)*(uint16_t)in2 + remainder;
    bn_setBlock(out, i, temp%256);
    remainder = temp>>8;
  }

  //If we have a remainder, add it to the top of the number
  if(remainder!=0) {
    bn_addblock(out);
    bn_setBlock(out, len, remainder);
  }
}


/* Shifts the number by the given number of bits. That is multiplies or divides
 * the number by some power of 2. If `amount' is positive, the number will be
 * left shifted, i.e. multiplied by a power of 2, and if `amount' is negative
 * it will be right shifted, i.e. (integer) divided by a power of 2.
 * ----------------------------------------------------------------------------
 * bn_t num       -- The number to be bit shifted.
 * int64_t amount -- The distance to bit shift the number, left if positive,
 *                   right if negative.
 *
 * return         -- 1 if bit shifted successfully, 0 otherwise.
 */
int8_t bn_bitshift(bn_t num, int64_t amount) {
    /* If amount is 0, there is nothing to shift. */
    if(amount == 0) {
        return 1;
    }

    /* Block shift the number by the number of bits divided by 8. */
    int64_t blocks = amount/8;
    if(!bn_blockshift(num, blocks)) {
        /* If we couldn't blockshift, return failure. */
        return 0;
    }

    /* The number of bits we then need to shift is the total mod 8. */
    int16_t bits = amount%8;

    if(amount < 0) {
        /* If right shifting, half the number the given number of times. */
        //TODO is there a better way of doing this??
        for(int16_t i = 0; i>bits; i--) {
            bn_half(num);
        }
    } else {
        /* If left shifting, multiply by 2**bits. */
        bn_mul_ub(num, 1<<bits, num);
    }

    return 1;
}
