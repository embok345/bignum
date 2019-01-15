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

void bn_resize(bignum *num, uint32_t noBlocks) {
  if(num->noBlocks>=noBlocks) {
    num->noBlocks = noBlocks;
    return;
  }

  if(noBlocks == 0) {
    bn_destroy(num);
    return;
  }

  if(!num->blocks) num->blocks = malloc(noBlocks);
  else if(noBlocks>num->noBlocks) num->blocks = realloc(num->blocks, noBlocks);

  for(uint32_t i=num->noBlocks; i<noBlocks; i++) {
    num->blocks[i] = 0;
  }
  num->noBlocks = noBlocks;
}

void bn_set(bignum *num, uint32_t noBlocks, uint8_t *blocks, int8_t sign) {
  bn_resize(num, noBlocks);
  num->sign = sign;
  memcpy(num->blocks, blocks, noBlocks);
}


void bn_rand(bignum *num, uint32_t noBlocks) {
  bn_resize(num, noBlocks);
  for(uint32_t i=0; i<num->noBlocks; i++) {
    num->blocks[i] = rand()%256;
  }
  bn_removezeros(num);
}

void bn_clone(bignum *new, const bignum old) {
  if(new->blocks == old.blocks) return;
  if(!old.blocks) {
    bn_destroy(new);
    return;
  }
  bn_resize(new, old.noBlocks);
  memcpy(new->blocks, old.blocks, old.noBlocks);
  new->sign = old.sign;
}

void bn_clone_float(bn_float_t *new, const bn_float_t old) {
  bn_clone(&(new->n), old.n);
  new->a = old.a;
}

void bn_destroy(bignum *num) {
  if(num->blocks) free(num->blocks);
  num->noBlocks = 0;
  num->blocks = NULL;
}

void bn_destroy_float(bn_float_t *num) {
  bn_destroy(&num->n);
  num->a = 0;
}

inline void bn_addblock(bignum *num) {
  bn_resize(num, num->noBlocks+1);
}

inline void bn_addblocks(bignum *num, uint32_t noBlocks) {
  bn_resize(num, num->noBlocks+noBlocks);
}

void bn_blockshift(bignum *num, int32_t amount) {
  if(amount == 0)
    return;
  if(amount<0) {
    if(abs(amount) >= num->noBlocks) {
      bn_clone(num, ZERO);
      return;
    }

    num->noBlocks+=amount;
    num->blocks = memmove(num->blocks, num->blocks-amount, num->noBlocks);

  } else {
    int32_t noBlocks = num->noBlocks;
    bn_resize(num, num->noBlocks+amount);
    //I want to belive there is a nicer way of doing this.
    for(int32_t i=noBlocks-1; i>=0; i--) {
      num->blocks[amount+i] = num->blocks[i];
    }
    for(int32_t i=0; i<amount; i++) {
      num->blocks[i] = 0;
    }
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
      bn_mul_byte(*num, 2, num);
    }
  }
}

//Note it doesn't actually realloc.
void bn_removezeros(bignum *in) {
  if(in->noBlocks == 1 || in->blocks[in->noBlocks-1] !=0 ) return;

  while(in->blocks[in->noBlocks-1] == 0 && in->noBlocks>0) {
    in->noBlocks--;
  }
}


void bn_littleblocks(const bignum num, uint32_t length, bignum *out) {
  bn_clone(out, num);
  if(length<num.noBlocks)
    bn_resize(out, length);
}

void bn_bigblocks(const bignum num, uint32_t length, bignum *out) {
  bn_clone(out, num);
  if(length<num.noBlocks)
    bn_blockshift(out, num.noBlocks-length);
}

void bn_prnt_test(bignum a) {
  for(int i=0; i<a.noBlocks; i++) {
    printf("%"PRIu8",", a.blocks[i]);
  }
  printf("\n");
}

int main() {
  srand(time(NULL));
  bignum a;
  bn_init(&a);
  bn_rand(&a, 1);
  bn_prnt_test(a);
  bn_resize(&a, 20);
  bn_prnt_test(a);

  bn_removezeros(&a);
  bn_prnt_test(a);

  bn_blockshift(&a, 5);

  bn_prnt_test(a);

  bn_blockshift(&a, -7);

  bn_prnt_test(a);

  bn_mul_byte(a, 10, &a);

  bn_prnt_test(a);

  bn_destroy(&a);

  printf("Done\n");
  return 0;
}
