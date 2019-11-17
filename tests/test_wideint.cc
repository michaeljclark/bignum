// See LICENSE.md

#include <cassert>
#include <cstdlib>

#include "wideint.h"

typedef wideint<48> int48_t;
typedef wideint<128> int128_t;
typedef wideint<256> int256_t;

template <typename T> void test_i256()
{
    assert(sizeof(T) == 32);
    assert(sizeof(typename T::limb_t) == sizeof(void*));
    assert(T::limb_count == sizeof(T)/sizeof(typename T::limb_t));
}

template <typename T> void print(const char *fmt, T num)
{
    const size_t hex_digits = T::limb_bits >> 2;
    const size_t len = T::limb_count * (hex_digits + 4);

    char *buf = (char*)alloca(len);
    char *p = buf, *end = buf + len;
    for (ptrdiff_t i = T::limb_count-1; i >= 0; i--) {
        if (i != T::limb_count-1) {
            p += snprintf(p, end-p, ", ");
        }
        p += snprintf(p, end-p, "0x%016lx", num.limbs[i]);
    }
    *p = 0;

    printf(fmt, buf);
}

#define STR(x) #x
#define DUMP(x) print(STR(x) " = { %s }\n", x);

void test_wideint()
{
    test_i256<int256_t>();

    int256_t b = 0;
    assert(b.limbs[3] == 0);
    assert(b.limbs[2] == 0);
    assert(b.limbs[1] == 0);
    assert(b.limbs[0] == 0);

    b = 0xff;
    b <<= 8;
    DUMP(b);
    assert(b.limbs[3] == 0);
    assert(b.limbs[2] == 0);
    assert(b.limbs[1] == 0);
    assert(b.limbs[0] == 0xff00);

    b <<= 128;
    DUMP(b);
    assert(b.limbs[3] == 0);
    assert(b.limbs[2] == 0xff00);
    assert(b.limbs[1] == 0);
    assert(b.limbs[0] == 0);

    b >>= 16;
    DUMP(b);
    assert(b.limbs[3] == 0);
    assert(b.limbs[2] == 0);
    assert(b.limbs[1] == 0xff00000000000000ull);
    assert(b.limbs[0] == 0);

    b >>= 112;
    DUMP(b);
    assert(b.limbs[3] == 0);
    assert(b.limbs[2] == 0);
    assert(b.limbs[1] == 0);
    assert(b.limbs[0] == 0xff00);

    for (size_t i = 0; i < (int256_t::num_bits >> 3); i++) {
        b <<= 8;
        b |= i;
    }
    DUMP(b);
}

int main(int argc, char const *argv[])
{
    test_wideint();
}
