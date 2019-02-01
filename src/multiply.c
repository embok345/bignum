#include "bignum.h"

void bn_mul_long(const bignum *, const bignum *, bignum *);
void bn_mul_karat(const bignum *, const bignum *, bignum *);
void bn_mul_tc3(const bignum *, const bignum *, bignum *);

void bn_mul(const bignum *in1, const bignum *in2, bignum *out) {
  uint32_t len1 = bn_trueLength(in1);
  uint32_t len2 = bn_trueLength(in2);

  if(len2 == 0) {
    bn_clone(out, &ZERO);
    return;
  }
  if(len1<len2) {
    bn_mul(in2, in1, out);
    return;
  }

  if(len1 <= 50 && len2 <= 50) {
    bn_mul_long(in1, in2, out);
  } else {
    printf("k\n");
    bn_mul_karat(in1, in2, out);
  }

  if(bn_ispositive(in1) ^ bn_ispositive(in2)) bn_setnegative(out);
  else bn_setpositive(out);

  bn_removezeros(out);
}

void bn_mul_long(const bignum *in1, const bignum *in2, bignum *out) {

  uint32_t len1 = bn_trueLength(in1);
  uint32_t len2 = bn_trueLength(in2);

  bn_resize(out, len1+len2);

  bignum *mul, *add;
  bn_init(&mul);
  bn_init(&add);

  for(uint32_t i = len2-1; i>0; i--) {
    //multiply large to small blocks of in2 by the whole of in1
    if(bn_getBlock(in2, i) == 0) {
      bn_blockshift(add, 1);
      continue;
    }
    bn_mul_byte(in1, bn_getBlock(in2, i), mul);
    bn_add(mul, add, add);
    bn_blockshift(add, 1);
  }

  if(bn_getBlock(in2, 0) != 0) {
    bn_mul_byte(in1, bn_getBlock(in2, 0), mul);
    bn_add(mul, add, out);
  } else {
    bn_clone(out, add);
  }

  bn_nuke(&mul);
  bn_nuke(&add);
}

void bn_mul_karat(const bignum *in1, const bignum *in2, bignum *out) {

  uint32_t len1 = bn_trueLength(in1);
  uint32_t len2 = bn_trueLength(in2);

  bn_resize(out, len1+len2);

  uint32_t m = ceil((float)bn_max_ui(len1, len2)/2);

  bignum *z[3];
  bignum *up[2], *down[2];
  bignum *t1, *t2;

  bn_init(&z[0]);
  bn_init(&z[1]);
  bn_init(&z[2]);
  bn_init(&up[0]);
  bn_init(&up[1]);
  bn_init(&down[0]);
  bn_init(&down[1]);
  bn_init(&t1);
  bn_init(&t2);

  bn_bigblocks(in1, len1 - m, up[0]);
  bn_littleblocks(in1, m, down[0]);
  bn_bigblocks(in2, bn_max_si(0, (int32_t)len2-m), up[1]);
  bn_littleblocks(in2, m, down[1]);

  bn_mul(down[0],down[1], z[0]);
  bn_mul(up[0],up[1], z[2]);

  bn_add(up[0], down[0], t1);
  bn_add(up[1], down[1], t2);
  bn_mul(t1, t2, z[1]);
  bn_subtract(z[1], z[0], z[1]);
  bn_subtract(z[1], z[2], z[1]);

  bn_blockshift(z[2], m);
  bn_add(z[2], z[1], out);
  bn_blockshift(out, m);
  bn_add(out, z[0], out);

  bn_nuke(&up[0]);
  bn_nuke(&up[1]);
  bn_nuke(&down[0]);
  bn_nuke(&down[1]);
  bn_nuke(&t1);
  bn_nuke(&t2);
  bn_nuke(&z[0]);
  bn_nuke(&z[1]);
  bn_nuke(&z[2]);
}

void bn_mul_byte(const bignum *in1, uint8_t in2, bignum *out) {
  uint32_t len = bn_length(in1);
  bn_resize(out, len);
  uint16_t temp;
  uint8_t remainder = 0;
  for(uint32_t i=0; i<len; i++) {
    //temp = (uint16_t)in1.blocks[i]*(uint16_t)in2 + remainder;
    temp = (uint16_t)bn_getBlock(in1, i)*(uint16_t)in2 + remainder;
    //out->blocks[i] = temp%256;
    bn_setBlock(out, i, temp%256);
    remainder = temp>>8;
  }
  if(remainder!=0) {
    bn_addblock(out);
    //out->blocks[out->noBlocks-1] = remainder;
    bn_setBlock(out, len, remainder);
  }
}
