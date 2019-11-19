// See LICENSE.md

#include <cassert>
#include <cstdlib>
#include <cinttypes>

#include "wideint.h"
#include "bignum.h"

typedef wideint<48>            int48_t;
typedef wideint<128>          int128_t;
typedef wideint<128,false>   uint128_t;
typedef wideint<256>          int256_t;
typedef wideint<256,false>   uint256_t;

template <typename T> void printb(const char *fmt, T num)
{
    const size_t lc = num.num_limbs();
    const size_t hex_digits = 8;
    const size_t len = lc * (hex_digits + 4);

    char *buf = (char*)alloca(len);
    char *p = buf, *end = buf + len;
    for (ptrdiff_t i = lc-1; i >= 0; i--) {
        if (i != lc-1) {
            p += snprintf(p, end-p, ", ");
        }
        p += snprintf(p, end-p, "0x%08" PRIx32, num.limbs[i]);
    }
    *p = 0;

    printf(fmt, buf);
}

template <typename T> void printw(const char *fmt, T num)
{
    const size_t hex_digits = T::limb_bits >> 2;
    const size_t len = T::limb_count * (hex_digits + 4);

    char *buf = (char*)alloca(len);
    char *p = buf, *end = buf + len;
    for (ptrdiff_t i = T::limb_count-1; i >= 0; i--) {
        if (i != T::limb_count-1) {
            p += snprintf(p, end-p, ", ");
        }
        p += snprintf(p, end-p, "0x%016" PRIx64, num.limbs[i]);
    }
    *p = 0;

    printf(fmt, buf);
}

#define STR(x) #x
#define DUMP(x) printw(STR(x) " = { %s }\n", x);
#define DUMPB(x) printb(STR(x) " = { %s }\n", x);

template <typename T> void test_i256()
{
    assert(sizeof(T) == 32);
    assert(sizeof(typename T::ulimb_t) == sizeof(void*));
    assert(T::limb_count == sizeof(T)/sizeof(typename T::ulimb_t));
}

#define ASSERT_EQ_48(v,a) \
    DUMP(v); \
    assert(v.limbs[0] == a);

#define ASSERT_EQ_256(v,a,b,c,d) \
    DUMP(v); \
    assert(v.limbs[3] == a); \
    assert(v.limbs[2] == b); \
    assert(v.limbs[1] == c); \
    assert(v.limbs[0] == d);

void test_wideint()
{
    test_i256<int256_t>();

    int256_t b = 0;
    assert(b.limbs[3] == 0);
    assert(b.limbs[2] == 0);
    assert(b.limbs[1] == 0);
    assert(b.limbs[0] == 0);

    b = 0xff00ff00ff00ff00ull;
    ASSERT_EQ_256(b,0x0000000000000000ull,0x0000000000000000ull,0x0000000000000000ull,0xff00ff00ff00ff00ull);

    /* shifts */
    b <<= 8;
    ASSERT_EQ_256(b,0x0000000000000000ull,0x0000000000000000ull,0x00000000000000ffull,0x00ff00ff00ff0000ull);
    b <<= 128;
    ASSERT_EQ_256(b,0x00000000000000ffull,0x00ff00ff00ff0000ull,0x0000000000000000ull,0x0000000000000000ull);
    b >>= 24;
    ASSERT_EQ_256(b,0x0000000000000000ull,0x0000ff00ff00ff00ull,0xff00000000000000ull,0x0000000000000000ull);
    b >>= 128;
    ASSERT_EQ_256(b,0x0000000000000000ull,0x0000000000000000ull,0x0000000000000000ull,0x0000ff00ff00ff00ull);
    b <<= 96;
    ASSERT_EQ_256(b,0x0000000000000000ull,0x000000000000ff00ull,0xff00ff0000000000ull,0x0000000000000000ull);
    b <<= 112;
    ASSERT_EQ_256(b,0xff00ff00ff000000ull,0x0000000000000000ull,0x0000000000000000ull,0x0000000000000000ull);
    b >>= 8;
    ASSERT_EQ_256(b,0xffff00ff00ff0000ull,0x0000000000000000ull,0x0000000000000000ull,0x0000000000000000ull);
    int256_t c = b >> 192;
    ASSERT_EQ_256(c,0xffffffffffffffffull,0xffffffffffffffffull,0xffffffffffffffffull,0xffff00ff00ff0000ull);
    int256_t d = b >> 208;
    ASSERT_EQ_256(d,0xffffffffffffffffull,0xffffffffffffffffull,0xffffffffffffffffull,0xffffffff00ff00ffull);
    uint256_t e = uint256_t(b) >> 208;
    ASSERT_EQ_256(e,0x0000000000000000ull,0x0000000000000000ull,0x0000000000000000ull,0x0000ffff00ff00ffull);
    uint256_t f = b ; f >>= 208;
    ASSERT_EQ_256(f,0x0000000000000000ull,0x0000000000000000ull,0x0000000000000000ull,0x0000ffff00ff00ffull);

    for (size_t i = 0; i < int256_t::num_bytes; i++) {
        b <<= 8;
        b |= (int256_t::num_bytes-i);
    }
    ASSERT_EQ_256(b,0x201f1e1d1c1b1a19ull,0x1817161514131211ull,0x100f0e0d0c0b0a09ull,0x0807060504030201ull);

    int48_t g = 0x0807060504030201ull;
    uint256_t h = g;
    ASSERT_EQ_256(h,0x0000000000000000ull,0x0000000000000000ull,0x0000000000000000ull,0x0000060504030201ull);
    g = d;
    ASSERT_EQ_48(g, 0x0000ffff00ff00ffull);
}

void test_multiply()
{
    /* bignum multiplication */
    bignum b12 = bignum(2147483648) * bignum(2147483648);
    DUMPB(b12);
    bignum b13 = b12 * b12;
    DUMPB(b13);
    bignum b14 = bignum(2147483647) * bignum(2147483647);
    DUMPB(b14);
    bignum b15 = b14 * b14;
    DUMPB(b15);
    bignum b16 = b15 * b15;
    DUMPB(b16);

    /* wideint multiplication */
    uint256_t w12 = uint256_t(2147483648) * uint256_t(2147483648);
    DUMP(w12);
    uint256_t w13 = w12 * w12;
    DUMP(w13);
    uint256_t w14 = uint256_t(2147483647) * uint256_t(2147483647);
    DUMP(w14);
    uint256_t w15 = w14 * w14;
    DUMP(w15);
    uint256_t w16 = w15 * w15;
    DUMP(w16);

    /* compare strings */
    assert(b12.to_string() == w12.to_string());
    assert(b13.to_string() == w13.to_string());
    assert(b14.to_string() == w14.to_string());
    assert(b15.to_string() == w15.to_string());
    assert(b16.to_string() == w16.to_string());

    /* print decimal strings */
    printf("b12 = %s\n", b12.to_string().c_str());
    printf("w12 = %s\n", w12.to_string().c_str());
    printf("b13 = %s\n", b13.to_string().c_str());
    printf("w13 = %s\n", w13.to_string().c_str());
    printf("b14 = %s\n", b14.to_string().c_str());
    printf("w14 = %s\n", w14.to_string().c_str());
    printf("b15 = %s\n", b15.to_string().c_str());
    printf("w15 = %s\n", w15.to_string().c_str());
    printf("b16 = %s\n", b16.to_string().c_str());
    printf("w16 = %s\n", w16.to_string().c_str());
}

int main(int argc, char const *argv[])
{
    test_wideint();
    test_multiply();
}
