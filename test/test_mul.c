#include "bignum.h"
#include <criterion/criterion.h>
#include <time.h>

void initialize(void) {
    register_printf_function('B', bn_printf, bn_printf_info);
}

TestSuite(mul, .init=initialize);

int8_t bn_mul_tc3(const bn_t, const bn_t, bn_t);

Test(mul, tc3) {
    bn_t m, n;
    bn_t out1, out2;
    if(!bn_inits(4, &m, &n, &out1, &out2)) cr_assert_fail("Couldn't init bignums");

    bn_conv_str2bn("1962292663755342276878171591292032076502196173846134242413565716035956767500435103731481187353303643729818866433534826397", m);
    bn_conv_str2bn("81041710035431057549062374202255990942966636616311048947557649738586134741125264206007813130048114594442343960451120587", n);

    bn_mul(m, n, out1);
    bn_mul_tc3(m, n, out2);

    cr_assert(bn_equals(out1, out2) == 1, "Expected %B, but got %B", out1, out2);
}
