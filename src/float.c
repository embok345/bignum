#include "bignum.h"

//Stores the number m*256^e
struct bn_float {
  bignum *m;
  int64_t e;
};

void bnf_init(bn_float **a) {
  *a = malloc(sizeof(bn_float));
  bn_init(&((*a)->m));
  (*a)->e = 0;
}

void bnf_inits(int num, ...) {
  va_list list;
  va_start(list, num);
  for(int i=0; i<num; i++) {
    bnf_init(va_arg(list, bn_float**));
  }
  va_end(list);
}

void bnf_nuke(bn_float **a) {
  if(a) {
    bn_nuke(&((*a)->m));
    free(a);
  }
  a = NULL;
}

void bnf_nukes(int num,...) {
  va_list list;
  va_start(list, num);
  for(int i=0; i<num; i++) {
    bnf_nuke(va_arg(list, bn_float **));
  }
  va_end(list);
}
