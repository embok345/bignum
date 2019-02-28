#include "bignum.h"

void bn_prnt_blocks(const bignum *num) {
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

void bn_prnt_dec(const bignum *num) {
  if(bn_isempty(num)) {
    printf("Bignum is empty\n");
    return;
  }
  char *out;
  out = bn_conv_bn2str(num);
  printf("%s\n", out);
  free(out);
}

int bn_printf(FILE *stream,
              const struct printf_info *info,
              const void *const *args) {
  const bignum *num;
  char *buffer;
  int len;

  num = *((const bignum **) (args[0]));
  buffer = bn_conv_bn2str(num);
  len = strlen(buffer);

  len = fprintf(stream, "%*s", (info->left ? -info->width : info->width),
                buffer);

  free(buffer);
  return len;
}

int bn_printf_info(const struct printf_info *info, size_t n, int *argtypes, int *sz) {
  if(n>0)
    argtypes[0] = PA_POINTER;
  return 1;
}
