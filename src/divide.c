#include "bignum.h"

static uint8_t bn_div_close(const bn_t, const bn_t, bn_t);

int8_t bn_div(const bn_t in1, const bn_t in2,
              bn_t q, bn_t r) {
  int8_t c;

  if(bn_iszero(in2)) {
    //printf("%sbn/bnDivision by 0!%s\n", RED, NORMAL);
    //TODO log and set errno
    return 0;
  }
  c = bn_compare(in1, in2);

  if(c == -1) { //if a<b, return (0, a)
    bn_setzero(q);
    return bn_clone(r, in1);
  }
  if(c == 0) { //if a==b, return (1, 0)
    if(!bn_clone(q, BN_ONE)) return 0;
    bn_setzero(r);
    return 1;
  }

  if(bn_isnegative(in1)) {
    bn_t n, rTemp;
    if(!bn_inits(2, &n, &rTemp)) return 0;
    if(!bn_clone(n, in1)) return 0;
    bn_setpositive(n);
    bn_div(n, in2, q, rTemp);
    bn_increment(q);
    bn_setnegative(q);
    bn_sub(in2, rTemp, r);
    bn_deinits(2, &n, &rTemp);
    return 1;
  }

  uint32_t len1 = bn_trueLength(in1);
  uint32_t len2 = bn_trueLength(in2);

  bn_t quot, rem;
  if(!bn_inits(2,&quot, &rem)) return 0;

  if(!bn_resize(quot, len1-len2+1)) return 0;
  bn_upperblocks(in1, len2, rem);
  bn_removezeros(rem);

  c = bn_compare(rem, in2);

  if(c<0) { // if the top b.noBlocks blocks of a are less than b, the first quotient is 0 
    bn_setBlock(quot, len1-len2, 0);
  } else if(c==0) { // if they are equal, the first quotient is 1, with 0 remainder.
    bn_setBlock(quot, len1-len2, 1);
    bn_setzero(rem);
  } else { // otherwise, we divide them
    bn_setBlock(quot, len1-len2, bn_div_close(rem, in2, rem));
  }

  for(uint32_t i = 1; i<=len1-len2; i++) {
    //Get the next smaller block from
    bn_blockshift(rem, 1);
    bn_setBlock(rem, 0, bn_getBlock(in1, len1-len2-i));
    c = bn_compare(rem, in2);

    if(c<0) { // if the current blocks are less than b, the quotient is 0
      bn_setBlock(quot, len1-len2-i, 0);
    } else if(c==0) { // if the current blocks are equal to b, the quotient is 1, and remainder is 0
      bn_setBlock(quot, len1-len2-i, 1);
      bn_setzero(rem);
    } else {
      //Otherwise, we divide them
      bn_setBlock(quot, len1-len2-i, bn_div_close(rem, in2, rem));
    }

  }
  if(!bn_clone(r, rem) || !bn_clone(q, quot)) return 0;

  bn_deinits(2, &rem, &quot);

  return 1;
}

uint8_t bn_div_close(const bn_t in1, const bn_t in2, bn_t remainder) {
  int8_t c = 0;
  uint16_t q = 0;

  uint32_t len1 = bn_trueLength(in1);
  uint32_t len2 = bn_trueLength(in2);

  c = bn_compare(in1, in2);
  if(c==-1) {
    if(!bn_clone(remainder, in1)) {
        errno = ENOMEM;
        return 0;
    }
    return 0;
  }

  if(len1 > len2+1 ||
     (len1 == len2+1 && bn_getBlock(in1, len1-1) > bn_getBlock(in2, len2-1))) {
    //TODO what should we do here?
    //printf("%s This aint right %s \n", RED, NORMAL);
    exit(1);
    return 0;
  }
  q = bn_getBlock(in1, len1-1);

  if(len1>len2) {
    q*=256;
    q+=bn_getBlock(in1, len1-1);
  }

  bn_t r;
  if(!bn_init(&r)) {
      errno = ENOMEM;
      return 0;
  }

  q/=bn_getBlock(in2, len2-1);
  //Compute r = a-qb
  bn_mul_ub(in2, q, r);
  bn_sub(in1, r, r);

  //If r<0, q is an over estimate for the quotient, so add copies of b to r until r>=0, and decrement q
  while(bn_isnegative(r)) {
    bn_removezeros(r);
    bn_add(in2, r, r);
    q--;
  }

  bn_removezeros(r);

  //If r>b, we need to reduce q until r<b, by subtracting copies of b
  c = bn_compare(r,in2);
  while(c>=0) {
    bn_sub(r, in2, r);
    q++;
    bn_removezeros(r);
    c = bn_compare(r, in2);
  }
  if(!bn_clone(remainder, r)) {
      errno = ENOMEM;
      q = 0;
  }
  bn_deinit(&r);
  return q;
}

void bn_div_rem(const bn_t in1, const bn_t in2, bn_t remainder) {
  bn_t q;
  if(!bn_init(&q)) return;
  bn_div(in1, in2, q, remainder);
  bn_deinit(&q);
}
void bn_div_quot(const bn_t in1, const bn_t in2, bn_t quotient) {
  bn_t r;
  if(!bn_init(&r)) return;
  bn_div(in1, in2, quotient, r);
  bn_deinit(&r);
}

void bn_half(bn_t num) {
  uint8_t remainder = 0;
  uint16_t temp;
  uint32_t noBlocks = bn_length(num);
  for(uint32_t i = 1; i<=noBlocks; i++) {
    temp = (remainder<<7) + (bn_getBlock(num, noBlocks-i) >> 1);
    //remainder = num->blocks[num->noBlocks-i]%2;
    remainder = bn_getBlock(num, noBlocks-i);
    //num->blocks[num->noBlocks-i] = temp%256;
    bn_setBlock(num, noBlocks-i, temp%256);
  }
  bn_removezeros(num);
}

uint32_t bn_oddPart(bn_t num) {
  uint32_t no2s = 0;
  uint32_t noZeroBlocks = 0;
  uint32_t len = bn_trueLength(num);
  for(uint32_t i = 0; i<len; i++) {
    if(bn_getBlock(num, i) == 0) {
      noZeroBlocks++;
    } else {
      break;
    }
  }
  bn_blockshift(num, -noZeroBlocks);
  no2s = noZeroBlocks*8;
  while(bn_iseven(num)) {
    bn_half(num);
    no2s++;
  }
  return no2s;
}

void bn_div_ub(const bn_t in1, uint8_t in2, bn_t out) {
    size_t len = bn_length(in1);
    if(!bn_clone(out, in1)) return;

    uint16_t numerator = 0;
    uint16_t remainder = 0;
    uint8_t next_block = 0;

    for(int i = 0; i < len; i++) {
        numerator = (uint16_t)bn_getBlock(in1, len - i - 1) + remainder;
        next_block = (numerator / (uint16_t)in2)%256;
        remainder = (numerator % in2) << 8;
        bn_setBlock(out, len - i - 1, next_block);
    }
}
