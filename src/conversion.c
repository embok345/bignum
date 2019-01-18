#include "bignum.h"

char *bn_conv_bn2str(bignum *num) {
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
