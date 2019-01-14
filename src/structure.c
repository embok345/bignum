#include "bignum.h"

uint8_t nums0[] = {0};
const bignum ZERO = {1,nums0 , 1};
uint8_t nums1[] = {1};
const bignum ONE = {1,nums1, 1};

/* Initialise the bignum, either being empty or with some number of blocks.
 * One of these MUST be called before using the bignum!!!!!!!!!!!!!!!!!!!!!!!!!
 */
void bn_init(bignum *num) {
  num->noBlocks = 0;
  num->blocks = NULL;
  num->sign = 0;
}

void bn_inits(bignum *num, uint32_t noBlocks) {
  if(!noBlocks) {
    bn_init(num);
    return;
  }
  num->noBlocks = noBlocks;
  num->blocks = malloc(noBlocks);
  for(uint32_t i=0; i<noBlocks; i++) {
    num->blocks[i] = 0;
  }
  num->sign = 0;
}

void bn_set(bignum *num, uint32_t noBlocks, uint8_t *blocks, int8_t sign) {
  if(!num->blocks) num->blocks = malloc(noBlocks);
  else if(noBlocks>num->noBlocks) realloc(num->noBlocks, noBlocks);
  num->noBlocks = noBlocks;
  num->sign = sign;
  memcpy(num->blocks, blocks, noBlocks);
}


void bn_rand(bignum *num, uint32_t noBlocks) {
  bn_init(num, noBlocks);
  for(uint32_t i=0; i<num->noBlocks; i++) {
    num->blocks[i] = rand()%256;
  }
  bn_removezeros(num);
}

void bn_copy(bignum *new, bignum old) {
  if(!old.blocks) return;
  if(memcmp(new->blocks, old.blocks)) return;
  if(new->blocks) free(new->blocks);
  new->noBlocks = old.noBlocks;
  new->blocks = (uint8_t*)malloc(new->noBlocks);
  memcpy(new->blocks, old.blocks, new->noBlocks);
  new->sign = old.sign;
}

void bn_copy_float(bn_float_t *new, bn_float_t old) {
  bn_copy(&(*new).n, old.n);
  (*new).a = old.a;
}

void bn_destroy(bignum *num) {
  if(num->blocks) free(num->blocks);
  num->noBlocks = 0;
  num->blocks = NULL;
}

void bn_destroy_float(bn_float_t *num) {
  bn_destroy(&(*num).n);
  (*num).a = 0;
}

inline void bn_addblock(bignum *num) {
  bn_addblocks(num, 1);
}

void bn_addblocks(bignum *num, uint32_t noBlocks) {
  num->noBlocks+=noBlocks;
  num->blocks = (uint8_t*)realloc(num->blocks, num->noBlocks);
  for(uint32_t i=num->noBlocks-1; i>=num->noBlocks-noBlocks; i--) {
    num->blocks[i] = 0;
  }
}

void bn_blockshift(bignum *num, int32_t amount) {
  if(amount == 0)
    return;
  if(amount<0) {
    //if we shift more than the size of num, we are just removing all of the blocks
    if(abs(amount)>=(*num).noBlocks) {
      bn_destroy(num);
      bn_copy(num, ZERO);
      return;
    }
    //just get the top amount blocks
    //amount <0, so subtracting
    bignum new = bn_bigblocks((*num), (*num).noBlocks+amount);
    bn_destroy(num);
    bn_copy(num, new);
    bn_destroy(&new);

  } else {
    //realloc the blocks, move the existing blocks up, and set the lower blocks to 0
    (*num).blocks = realloc((*num).blocks, (*num).noBlocks+amount);
    for(uint32_t i=(*num).noBlocks+amount-1; i>=amount; i--) {
      (*num).blocks[i] = (*num).blocks[i-amount];
    }
    for(uint32_t i = 0; i<amount; i++) {
      (*num).blocks[i] = 0;
    }
    (*num).noBlocks+=amount;
  }
}

void bn_bitshift(bignum *num, int64_t amount) {
  if(amount == 0) {
    return;
  }
  if(amount < 0) {
    int64_t blocks = amount/8;
    bn_blockshift(num, blocks);
    int16_t bits = amount%8;
    for(int16_t i = 0; i>bits; i--) {
      bn_div_2(num);
    }
  } else {
    int64_t blocks = amount/8;
    bn_blockshift(num, blocks);
    int16_t bits = amount%8;
    for(int16_t i = 0; i<bits; i++) {
      bn_mul_2(num);
    }
  }
}

void bn_removezeros(bignum *in) {
  //if the top block isn't a zero there is nothing to do, and bignums can't have 0 blocks
  if((*in).noBlocks == 1 || (*in).blocks[(*in).noBlocks-1]!=0 ) return;
  uint32_t toRemove = 0;
  //count how many zero blocks there are on top
  for(uint32_t i = (*in).noBlocks-1; i>0; i--) {
    if((*in).blocks[i]!=0) {
      break;
    }
    toRemove++;
  }

  //release the upper blocks
  (*in).noBlocks-=toRemove;
  (*in).blocks = realloc((*in).blocks, (*in).noBlocks);
}

bignum bn_littleblocks(bignum num, uint32_t length) {
  bignum out;
  //The 'length' smallest blocks of a bignum of size < length are just the while thing
  if(length>=num.noBlocks) {
    bn_copy(&out, num);
    return out;
  }
  //Copy the small blocks
  bn_init(&out, length);
  for(uint32_t i = 0; i<length; i++) {
    out.blocks[i] = num.blocks[i];
  }
  out.sign = num.sign;
  return out;
}

bignum bn_bigblocks(bignum num, uint32_t length) {
  bignum out;
  //The length biggest blocks of a bignum with less than length blocks are just the whole thing
  if(length>=num.noBlocks) {
    bn_copy(&out, num);
    return out;
  }

  //Copy the big blocks
  bn_init(&out, length);
  for(uint32_t i=0; i<length; i++) {
    out.blocks[i] = num.blocks[num.noBlocks-length+i];
  }
  out.sign = num.sign;
  return out;
}*/
