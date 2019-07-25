#include "bignum.h"
#include <stdlib.h>
#include <time.h>

int main() {

  srand(time(NULL));

  bn_t a, b;
  if(!bn_inits(2, &a, &b)) return 1;

  bn_rand_blocks(a, 5);
  bn_rand_blocks(b, 5);
  bn_prnt_dec(a);
  bn_prnt_dec(b);

  bn_t temp = a;
  a = b;
  b = temp;

  bn_prnt_dec(a);
  bn_prnt_dec(b);

  bn_deinits(2, &a, &b);

 /*bn_t x, y, z;
  bn_inits(3, &x, &y, &z);
  bn_rand_blocks(x, 2 + rand()%20);
  bn_rand_blocks(y, 2 + rand()%20);

  bn_prnt_dec(x);
  bn_prnt_dec(y);

  bn_add(x, y, z);
  bn_prnt_dec(z);

  bn_sub(x, y, z);
  bn_prnt_dec(z);

  bn_mul(x, y, z);
  bn_prnt_dec(z);

  bn_t q;
  bn_init(&q);

  bn_div(x, y, q, z);
  bn_prnt_dec(q);
  bn_prnt_dec(z);

  bn_nukes(4, &x, &y, &q, &z);*/

}
