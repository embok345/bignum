#include "bignum.h"
#include <criterion/criterion.h>

TestSuite(structure);

Test(structure, init) {
    bn_t a;
    cr_assert_eq(bn_init(&a), 1);
    bn_deinit(&a);
    cr_assert_null(a);
}

Test(structure, innerblocks) {
    bn_t a, b;
    cr_assert_eq(bn_inits(2, &a, &b), 1);
    cr_assert_eq(bn_resize(a, 5), 1);
    for(int i = 0; i < 5; i++) {
        bn_setBlock(a, i, i);
    }

    bn_deinits(2, &a, &b);
}
