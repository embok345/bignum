#include "bignum.h"

void bn_prnt_blocks(bignum *num) {
  if(bn_isempty(num)) {
    printf("Bignum is empty\n");
    return;
  }
  uint32_t len = bn_length(num);
  for(uint32_t i = len-1; i>0; i--) {
    printf("%"PRIu8", ", bn_getBlock(num, i));
  }
  printf("%"PRIu8"\n", bn_getBlock(num, 0));
}

void bn_prnt_dec(bignum *num) {
  if(bn_isempty(num)) {
    printf("Bignum is empty\n");
    return;
  }
  char *out;
  out = bn_conv_bn2str(num);
  if(bn_isnegative(num)) {
    char temp[strlen(out)+1];
    temp[0] = '-';
    temp[1] = '\0';
    strcat(temp, out);
    out = realloc(out, strlen(out)+2);
    strcpy(out, temp);
  }
  printf("%s\n", out);
  free(out);
}
