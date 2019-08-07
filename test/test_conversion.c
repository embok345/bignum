#include "bignum.h"
#include <criterion/criterion.h>

TestSuite(conversion);

Test(conversion, conv_bn2str) {
    bn_t in;
    const char *number = "12345678910";
    if(!bn_init(&in)) cr_assert_fail("Could not alloc bignum");
    bn_conv_str2bn(number, in);
    cr_assert_str_eq(bn_conv_bn2str(in), number);
    bn_setnegative(in);
    cr_assert_str_eq(bn_conv_bn2str(in), "-12345678910");
    bn_conv_str2bn("00000", in);
    cr_assert_str_eq(bn_conv_bn2str(in), "0");
}

Test(conversion, conv_bn2ui) {
    bn_t in;
    if(!bn_init(&in)) cr_assert_fail("Could not alloc bignum");
    bn_conv_str2bn("123456", in);
    cr_expect_eq(bn_conv_bn2ui(in), 123456);
}
