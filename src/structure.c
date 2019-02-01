#include "bignum.h"

struct bignum {
	uint32_t noBlocks;
	uint8_t* blocks;
	int8_t sign;
};

uint8_t nums0[] = {0};
const bignum ZERO = {1,nums0 , 1};
uint8_t nums1[] = {1};
const bignum ONE = {1,nums1, 1};

/* Initialise the bignum.
 * This MUST be called before using the bignum!!!!!!!!!!!!!!!!!!!!!!!!!
 */
void bn_init(bignum **num) {
  *num = malloc(sizeof(bignum));
  (*num)->noBlocks = 0;
  (*num)->blocks = NULL;
  (*num)->sign = 0;
}

void bn_resize(bignum *num, uint32_t noBlocks) {
  //If we want to resize to 0 blocks, just destroy it.
  if(noBlocks == 0) {
    bn_destroy(num);
    return;
  }

  num->blocks = realloc(num->blocks, noBlocks);

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
  if(noBlocks == 0) {
    bn_destroy(num);
    return;
  }
  bn_resize(num, noBlocks);
  for(uint32_t i=0; i<num->noBlocks; i++) {
    num->blocks[i] = rand()%256;
  }
  bn_removezeros(num);
  num->sign = 1;
}

void bn_clone(bignum *new, const bignum *old) {
  if(new->blocks == old->blocks) return;
  if(bn_isempty(old)) {
    bn_destroy(new);
    return;
  }
  bn_resize(new, old->noBlocks);
  memcpy(new->blocks, old->blocks, old->noBlocks);
  new->sign = old->sign;
}

void bn_destroy(bignum *num) {
  if(num->blocks) free(num->blocks);
  num->noBlocks = 0;
  num->blocks = NULL;
}

void bn_nuke(bignum **num) {
  if(num) {
    bn_destroy(*num);
    free(*num);
  }
  *num = NULL;
}

int8_t bn_isempty(const bignum *num) {
  return (num->noBlocks == 0 || !num->blocks) ? 1 : 0;
}

void inline bn_addblock(bignum *num) {
  bn_resize(num, num->noBlocks+1);
}

void inline bn_addblocks(bignum *num, uint32_t noBlocks) {
  bn_resize(num, num->noBlocks+noBlocks);
}

void bn_blockshift(bignum *num, int32_t amount) {
  if(amount == 0)
    return;
  if(num->noBlocks == 0 || bn_equals(num, &ZERO))
    return;
  if(amount<0) {
    if(abs(amount) >= num->noBlocks) {
      bn_clone(num, &ZERO);
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
      bn_mul_byte(num, 2, num);
    }
  }
}

//Note it doesn't actually realloc.
void bn_removezeros(bignum *in) {
  if(in->noBlocks < 1 || in->blocks[in->noBlocks-1] !=0 ) return;

  while( in->noBlocks>=1 && in->blocks[in->noBlocks-1] == 0) {
    in->noBlocks--;
    //printf("%"PRIu32"\n", in->noBlocks);
  }
  //printf("removed zeros\n");
}

uint32_t bn_leadingZeros(const bignum *in) {
  if(bn_isempty(in)) return 0;
  uint32_t numZeros = 0;
  uint32_t noBlocks=bn_length(in);
  if(noBlocks == 1 || bn_getBlock(in, noBlocks - 1) != 0) return 0;
  while(bn_getBlock(in, --noBlocks)==0) numZeros++;
  return numZeros;
}

void bn_littleblocks(const bignum *num, uint32_t length, bignum *out) {
  if(bn_isempty(num)) {
    bn_destroy(out);
    return;
  }
  if(length == 0) {
    bn_clone(out, &ZERO);
    return;
  }

  bn_clone(out, num);
  if(length<num->noBlocks)
    bn_resize(out, length);
}

void bn_bigblocks(const bignum *num, uint32_t length, bignum *out) {
  if(bn_isempty(num)) {
    bn_destroy(out);
    return;
  }
  if(length == 0) {
    bn_clone(out, &ZERO);
    return;
  }
  bn_clone(out, num);
  if(length<num->noBlocks)
    bn_blockshift(out, -(num->noBlocks-length));
}

uint32_t bn_length(const bignum *num) {
  return num->noBlocks;
}
uint32_t bn_trueLength(const bignum *num) {
  uint32_t len = bn_length(num) - bn_leadingZeros(num);
  //if(len==0) len++;
  return len;
}

uint8_t bn_getBlock(const bignum *num, uint32_t index) {
  if(index>=num->noBlocks) return 0; //TODO return something more appropriate.
  return num->blocks[index];
}
void bn_setBlock(bignum *num, uint32_t index, uint8_t val) {
  if(index<num->noBlocks)
    num->blocks[index] = val;
}
/*int8_t bn_getSign(const bignum *num) {
  return num->sign;
}*/
int8_t bn_isnegative(const bignum *num) {
  return (num->sign == -1) ? 1 : 0;
}
int8_t bn_ispositive(const bignum *num) {
  return (num->sign == 1) ? 1 : 0;
}
void bn_setpositive(bignum *num) {
  num->sign = 1;
}
void bn_setnegative(bignum *num) {
  num->sign = -1;
}
void bn_signSwap(bignum *num) {
  switch(num->sign) {
    case 1: num->sign = -1;
      break;
    case -1: num->sign = 1;
      break;
    default: num->sign = 0;
  }
}

/*void bn_clone_float(bn_float_t *new, const bn_float_t old) {
  bn_clone(&(new->n), old.n);
  new->a = old.a;
}*/
/*void bn_destroy_float(bn_float_t *num) {
  bn_destroy(&num->n);
  num->a = 0;
}*/
