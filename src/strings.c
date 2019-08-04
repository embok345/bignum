#include "bignum.h"

int8_t is_digit(const char c) {
    return c == '0' || c == '1' || c == '2' || c == '3' || c == '4' ||
      c == '5' || c == '6' || c == '7' || c == '8' || c == '9';
}

int8_t isdigit_str(const char* str) {
    if(strlen(str) == 0) return 0;
    if(*str == '-') {
        if(strlen(str) == 1) return 0;
        str++;
    }
    while(*str!='\0') {
        if(!is_digit(*str)) return 0;
        str++;
    }
    return 1;
}

void bn_str_add(char *in1, char *in2, char *out) {
  uint8_t remainder, temp;
  int addLength, strLength, len1, len2, i;
  char *tempStr;

  remainder = 0;
  temp = 0;
  len1 = strlen(in1);
  len2 = strlen(in2);

  if(len1>len2) {
    addLength = len2;
    strLength = len1;
  } else {
    addLength = len1;
    strLength = len2;
  }

  tempStr = malloc(strLength+1);
  tempStr[strLength] = '\0';

  for(i=1; i<=addLength; i++) {
    temp = in1[len1-i] + in2[len2-i] + remainder - 96;
    tempStr[strLength-i] = (temp%10)+48;
    remainder = (temp/10);
  }

  for(i=addLength+1; i<=strLength; i++) {
    temp = ((len1>len2) ? in1[len1-i] : in2[len2-i]) + remainder - 48;
    tempStr[strLength-i] = (temp%10)+48;
    remainder = (temp/10);
  }

  if(remainder!=0) {
    strLength++;
    tempStr = realloc(tempStr, strLength+1);
    char outTemp[strLength+1];
    outTemp[0] = (char)(remainder+48);
    outTemp[1] = '\0';
    strcat(outTemp, tempStr);
    strcpy(tempStr, outTemp);
    tempStr[strLength] = '\0';
  }

  strcpy(out, tempStr);
  out[strLength] = '\0';
  free(tempStr);
}

void bn_str_mul6(char *in, char *out) {

    size_t len;
    uint8_t remainder, temp;
    char *tempStr;

    len = strlen(in);
    remainder = 0;
    tempStr = malloc(len+2);

    tempStr[len + 1] = '\0';

    for(int i = len; i >= 1; i--) {
        temp = ((in[i - 1]-48)*6)+remainder;
        tempStr[i] = (char)((temp%10)+48);
        remainder = temp/10;
    }

    if(remainder!=0) {
        tempStr[0] = (char)(remainder + 48);
        strcpy(out, tempStr);
    } else {
        strcpy(out, tempStr + 1);
    }

    free(tempStr);
}

void bn_str_mul50(char *in, char *out) {
  int len;
  uint8_t remainder, temp;
  char *tempStr;

  len = strlen(in);
  remainder = 0;
  tempStr = malloc(len+1);

  for(int i=len-1;i>=0; i--) {
    temp = ((in[i]-48)*5)+remainder;
    tempStr[i] = (char)((temp%10)+48);
    remainder = temp/10;
  }

  tempStr[len] = '\0';

  if(remainder!=0) {
    len++;
    tempStr = realloc(tempStr, len+1);
    char outTemp[len];
    outTemp[0] = (char)(remainder+48);
    outTemp[1] = '\0';
    strcat(outTemp, tempStr);
    strcpy(tempStr, outTemp);
  }

  char outTemp[len+1];
  strcpy(outTemp, tempStr);
  strcat(outTemp, "0");
  strcpy(out, outTemp);
  free(tempStr);
  out[len+1] = '\0';
}

void bn_str_mul200(char *in, char *out) {
  int len;
  uint8_t remainder;
  char *tempStr;

  len = strlen(in);
  remainder = 0;
  tempStr = malloc(len+1);

  for(int i=len-1;i>=0; i--) {
    uint8_t temp = ((in[i]-48)*2)+remainder;
    tempStr[i] = (char)((temp%10)+48);
    remainder = temp/10;
  }

  tempStr[len] = '\0';

  if(remainder!=0) {
    len++;
    tempStr = realloc(tempStr, len+1);
    char outTemp[len];
    outTemp[0] = (char)(remainder+48);
    outTemp[1] = '\0';
    strcat(outTemp, tempStr);
    strcpy(tempStr, outTemp);
  }

  char outTemp[len+2];
  strcpy(outTemp, tempStr);
  strcat(outTemp, "00");
  strcpy(out, outTemp);
  out[len+2] = '\0';
  free(tempStr);
}


void bn_str_mul256(char *in, char *out) {
  char *str6, *str50, *str200, *str250, *str256;

  str6 = malloc(strlen(in)+2);
  str50 = malloc(strlen(in)+3);
  str200 = malloc(strlen(in)+4);
  str250 = malloc(strlen(in)+5);
  str256 = malloc(strlen(in)+6);

  bn_str_mul6(in, str6);
  bn_str_mul50(in, str50);
  bn_str_mul200(in, str200);

  bn_str_add(str50, str200, str250);
  bn_str_add(str6, str250, str256);

  strcpy(out, str256);

  free(str6);
  free(str50);
  free(str200);
  free(str250);
  free(str256);
}
