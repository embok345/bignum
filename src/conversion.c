#include "bignum.h"

int8_t isdigit_str(const char *);
void bn_str_mul256(char *, char *);
void bn_str_add(char *, char *, char *);

/* Converts a bignum into a string. The string can be freed later, in the
 * calling function. This function is probably woefully inefficient.
 * ----------------------------------------------------------------------------
 * const bn_t num -- The number to convert to a string.
 */
char *bn_conv_bn2str(const bn_t num) {

  char *out, *x, *temp, *workStr = (char *)malloc(3);
  workStr[0]='\0';
  //If the bignum is empty, return an empty string. (is this best?)
  if(bn_isempty(num)) return workStr;

  uint32_t numBlocks = bn_trueLength(num);
  for(uint32_t i = numBlocks-1; i>0; i--) {
    //Convert the current block to a string
    //An 8 bit number takes at most 3 digits
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

  //If the number is negative, add a minus sign to the front
  if(bn_isnegative(num)) {
    char temp2[strlen(out)+1];
    temp2[0] = '-';
    temp2[1] = '\0';
    strcat(temp2, out);
    out = realloc(out, strlen(out)+2);
    strcpy(out, temp2);
  }

  return out;
}

void bn_conv_str2bn(const char* str, bn_t num) {
  uint32_t len = strlen(str);

  bn_setzero(num);

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
    bn_add_ub(num, str[i]-48, num);
    //Multiply out by 10
    bn_mul_ub(num, 10, num);
    //bn_prnt_blocks(num);
  }
  //bn_prnt_blocks(num);
  //printf("%"PRIu8"\n", str[len-1]-48);
  bn_add_ub(num, str[len-1]-48, num);
  //bn_prnt_blocks(num);
}

uint32_t bn_conv_bn2ui(const bn_t in) {
  uint32_t out = 0;
  for(int i=0; i<bn_min_ui(4,bn_length(in)); i++) {
    out>>=8;
    out+=bn_getBlock(in, i);
  }
  return out;
}

void bn_conv_ui2bn(uint32_t in, bn_t out) {
  bn_resize(out, 4);

  bn_setBlock(out, 0, in%256);
  bn_setBlock(out, 1, (in>>8)%256);
  bn_setBlock(out, 2, (in>>16)%256);
  bn_setBlock(out, 3, (in>>24)%256);

  bn_removezeros(out);
}

void bn_conv_ub2bn(uint8_t in, bn_t out) {
  bn_resize(out, 1);
  bn_setBlock(out, 0, in);
}
