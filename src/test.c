#include "bignum.h"

int main() {
  srand(time(NULL));
  bignum *a, *b, *c;
  bn_init(&a);
  bn_init(&b);
  int j=0;
  bn_rand(b, rand()%10);
  while(j++<10) {
    bn_rand(a, rand()%10);

    bn_prnt_dec(a);
    printf("+");bn_prnt_dec(b);
    bn_add(a, b, b);
    printf("=");bn_prnt_dec(b);
  }
  bn_nuke(&a);
  bn_nuke(&b);
}
