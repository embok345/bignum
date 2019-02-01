#include "bignum.h"

char *bn_conv_bn2str(const bignum *num) {
  char *out, *x, *temp, *workStr = (char *)malloc(3);
  workStr[0]='\0';
  if(bn_isempty(num)) return workStr;

  uint32_t numBlocks = bn_length(num);
  for(uint32_t i = numBlocks-1; i>0; i--) {
    //Convert the current block to a string
    x = (char*)malloc(4);
    sprintf(x,"%"PRIu8, bn_getBlock(num, i));
    x[3]='\0';
    //Add new block to the end of the string
    temp = (char *)malloc(strlen(workStr)+1);
    strcpy(temp, workStr);
    workStr = (char *)realloc(workStr, strlen(workStr)+strlen(x)+2);
    bn_str_add(temp, x, workStr);
    free(temp);
    free(x);

    //Multiply string by 256
    temp = (char *)malloc(strlen(workStr)+4);
    bn_str_mul256(workStr, temp);
    workStr = (char *)realloc(workStr, strlen(workStr)+4);
    strcpy(workStr, temp);
    free(temp);

  }

  //Add the final block to the string
  x = (char*)malloc(4);
  sprintf(x,"%"PRIu8,bn_getBlock(num, 0));
  temp = (char *)malloc(strlen(workStr)+1);
  strcpy(temp, workStr);
  workStr = (char *)realloc(workStr, strlen(workStr)+strlen(x)+2);
  bn_str_add(temp, x, workStr);
  free(x);
  free(temp);

  out = (char *)malloc(strlen(workStr)+1);
  strcpy(out, workStr);
  free(workStr);
  return out;
}

void bn_conv_str2bn(const char* str, bignum *num) {
  uint32_t len = strlen(str);

  bn_clone(num, &ZERO);

  if(!isdigit_str(str)) {
    //TODO return something better than this.
    return;
  }

  for(uint32_t i=0; i<len-1; i++) {
    if(str[i] == '-') {
      bn_setnegative(num);
      continue;
    }
    //Add the next digit to num
    bn_add_byte(num, str[i]-48, num);
    //Multiply out by 10
    bn_mul_byte(num, 10, num);
  }
  bn_add_byte(num, str[len-1]-48, num);
}

void bn_conv_int2bn(uint32_t in, bignum *out) {
  uint8_t blocks[4];
  blocks[0] = in%256;
  blocks[1] = (in>>8)%256;
  blocks[2] = (in>>16)%256;
  blocks[3] = (in>>24)%256;
  bn_set(out, 4, blocks, 1);
  bn_removezeros(out);
}

void bn_conv_byte2bn(uint8_t in, bignum *out) {
  uint8_t blocks[1] = {in};
  bn_set(out, 1, blocks, 1);
}
