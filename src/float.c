#include "bignum.h"

const uint16_t B = 256;

//Stores the number m*256^e
struct bn_float {
  bignum *m;
  int32_t e;
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

void bnf_bn2bnf(const bignum *in, bn_float *out) {
  out->e = 0;
  bn_clone(out->m, in);
}

void bnf_prnt(const bn_float *in) {
  printf("%B * %"PRIu16"^%"PRId32"\n", in->m, B, in->e);
}
void bnf_prnt_blocks(const bn_float *in) {
  printf("%"PRIu16" ^ %"PRId32" * ", B, in->e);bn_prnt_blocks(in->m);
}

void bnf_add(const bn_float *in1, const bn_float *in2, bn_float *out) {
  if(in1->e < in2->e) {
    //printf("swapping them\n");
    bnf_add(in2, in1, out);
    return;
  } else {
  //printf("in1->e = %"PRId32"\nin2->e = %"PRId32"\n", in1->e, in2->e);
  int32_t k = in1->e - in2->e;
  bignum *new_m;
  bn_init(&new_m);
  bn_clone(new_m, in1->m);
  //bn_prnt_blocks(in1->m);
  //bn_prnt_blocks(new_m);
  //printf("k=%"PRId32"\n", k);
  bn_blockshift(new_m, k);
  //bn_prnt_blocks(new_m);
  bn_add(new_m, in2->m, new_m);
  //printf("A+B = %B\n", new_m);
  bn_nuke(&(out->m));
  out->m = new_m;
  out->e = in2->e;
  }
}

void bnf_mul(const bn_float *a, const bn_float *b, bn_float *c) {
  bn_mul(a->m, b->m, c->m);
  c->e = a->e + b->e;
}

void bnf_invert(const bignum *D, bn_float *out) {
  bn_float *x, *c1, *c2, *d, *t1, *t2, *t3, *o;
  bnf_inits(8, &x, &c1, &c2, &d, &t1, &t2, &t3, &o);
  uint32_t n = bn_trueLength(D)+2;
  c1->e = -n-1;
  c2->e = -n-1;
  uint8_t *blocks1 = malloc(n+2), *blocks2 = malloc(n+2);
  blocks1[n+1] = 2;
  blocks2[n+1] = 1;
  for(uint32_t i = 0; i<=n; i++) {
    blocks1[i] = 210;
    blocks2[i] = 225;
  }
  bn_set(c1->m, n+2, blocks1, 1);
  bn_set(c2->m, n+2, blocks2, 1);

  bnf_prnt(c1);
  bnf_prnt(c2);

  bnf_bn2bnf(&ONE, o);
  bnf_bn2bnf(D, d);
  d->e = (-n) + 2;
  bn_signSwap(d->m);
  //bnf_prnt_blocks(d);
  printf("d=");bnf_prnt(d);
  bnf_mul(c2, d, x);
  printf("c2*d=");bnf_prnt(x);
  bnf_add(c1, x, x);
  printf("c1 - c2*d=");bnf_prnt(x);
  printf("\n");

  for(int i=0; i<10; i++) {
    bnf_mul(d, x, t1);
    printf("-d*x = ");bnf_prnt(t1);
    bnf_add(o, t1, t2);
    printf("1-d*x = ");bnf_prnt(t2);
    bnf_mul(x, t2, t3);
    printf("x*(1-d*x) = ");bnf_prnt(t3);
    bnf_add(x, t3, x);
    printf("x + x*(1-d*x) = ");bnf_prnt(x);
    uint32_t newLen = bn_trueLength(x->m)+2;
    if(newLen > n+2) {
      bn_blockshift(x->m, n-newLen+2);
      x->e += newLen - n - 2;
    }
    printf("trunc x = ");bnf_prnt(x);
    printf("\n");
  }
}
