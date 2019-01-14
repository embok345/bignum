#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct {
  uint32_t noBlocks;
  uint8_t* blocks;
  int8_t sign;
} bignum;


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
  else if(noBlocks>num->noBlocks) num->blocks = realloc(num->blocks, noBlocks);
  num->noBlocks = noBlocks;
  num->sign = sign;
  memcpy(num->blocks, blocks, noBlocks);
}

int main() {

  bignum b;
  bn_init(&b);
  bn_inits(&b, 2);
  for(int i=0; i<b.noBlocks; i++) {
    printf("%"PRIu8",",b.blocks[i]);
  }
  printf("\n");
  uint8_t nums[] = {1,2,3,4,5,6,7,8,9,10};
  bn_set(&b, 8, nums, 1);
  for(int i=0; i<b.noBlocks; i++) {
    printf("%"PRIu8",", b.blocks[i]);
  }
  printf("\n");

  free(b.blocks);
  return 0;
}
