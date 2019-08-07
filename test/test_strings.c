#include "bignum.h"
#include <criterion/criterion.h>

char *bn_str_mul(const char *in, int8_t k);

TestSuite(strings);

Test(strings, str_mul256) {
    const char *in = "123456";
    cr_expect_null(bn_str_mul256(NULL));
    cr_expect_str_empty(bn_str_mul256(""));
    cr_expect_str_eq(bn_str_mul256("0"), "0");
    cr_expect_str_eq(bn_str_mul256("00000"), "0");
    cr_expect_str_eq(bn_str_mul256(in), "31604736");
}

Test(strings, str_mul) {
    const char *in = "12345";
    cr_assert_null(bn_str_mul(NULL, 5));
    cr_assert_null(bn_str_mul(in, 11));

    cr_expect_str_eq(bn_str_mul(in, 1), "12345");
    cr_expect_str_eq(bn_str_mul(in, 2), "024690");
    cr_expect_str_eq(bn_str_mul(in, 3), "037035");
    cr_expect_str_eq(bn_str_mul(in, 4), "049380");
    cr_expect_str_eq(bn_str_mul(in, 5), "061725");
    cr_expect_str_eq(bn_str_mul(in, 6), "074070");
    cr_expect_str_eq(bn_str_mul(in, 7), "086415");
    cr_expect_str_eq(bn_str_mul(in, 8), "098760");
    cr_expect_str_eq(bn_str_mul(in, 9), "111105");
}
