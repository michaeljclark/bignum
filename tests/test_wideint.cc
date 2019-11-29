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

template <typename T> void test_i48()
{
    assert(sizeof(T) == 8);
    assert(sizeof(typename T::ulimb_t) == sizeof(void*));
    assert(T::limb_count == sizeof(T)/sizeof(typename T::ulimb_t));
}

template <typename T> void test_i256()
{
    assert(sizeof(T) == 32);
    assert(sizeof(typename T::ulimb_t) == sizeof(void*));
    assert(T::limb_count == sizeof(T)/sizeof(typename T::ulimb_t));
}

#define ASSERT_EQ_48(v,a) \
    assert(v.limbs[0] == a);

#define ASSERT_EQ_256(v,a,b,c,d) \
    assert(v.limbs[3] == a); \
    assert(v.limbs[2] == b); \
    assert(v.limbs[1] == c); \
    assert(v.limbs[0] == d);

void test_mask_48()
{
    int48_t g = 0x0807060504030201ull;
    uint256_t h = g;
    ASSERT_EQ_256(h,0x0000000000000000ull,0x0000000000000000ull,0x0000000000000000ull,0x0000060504030201ull);
    g = uint256_t{0xffffffffffffffffull,0xffffffffffffffffull,0xffffffffffffffffull,0xffffffffffffffffull};
    ASSERT_EQ_48(g, 0x0000ffffffffffffull);
}

void test_shift_256()
{
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
}

void test_multiply()
{
    /* bignum multiplication */
    bignum b12 = bignum(2147483648) * bignum(2147483648);
    assert(b12.to_string() == "4611686018427387904");
    bignum b13 = b12 * b12;
    assert(b13.to_string() == "21267647932558653966460912964485513216");
    bignum b14 = bignum(2147483647) * bignum(2147483647);
    assert(b14.to_string() == "4611686014132420609");
    bignum b15 = b14 * b14;
    assert(b15.to_string() == "21267647892944572736998860269687930881");
    bignum b16 = b13 * b15;
    assert(b16.to_string() == "452312847740768055613344954116656535546982460631814466461259655922420023296");

    /* wideint multiplication */
    uint256_t w12 = uint256_t(2147483648) * uint256_t(2147483648);
    assert(w12.to_string() == "4611686018427387904");
    uint256_t w13 = w12 * w12;
    assert(w13.to_string() == "21267647932558653966460912964485513216");
    uint256_t w14 = uint256_t(2147483647) * uint256_t(2147483647);
    assert(w14.to_string() == "4611686014132420609");
    uint256_t w15 = w14 * w14;
    assert(w15.to_string() == "21267647892944572736998860269687930881");
    uint256_t w16 = w13 * w15;
    assert(w16.to_string() == "452312847740768055613344954116656535546982460631814466461259655922420023296");
}

void test_clz()
{
    assert(int256_t(0).count_leading_zeros() == 256);
    assert(int256_t(1).count_leading_zeros() == 255);
    assert(int256_t(2).count_leading_zeros() == 254);
    assert((int256_t{0,0,0,0xffffffffffffffffull}).count_leading_zeros() == 0);
    assert((int256_t{0,0,0,0x00ffffffffffffffull}).count_leading_zeros() == 8);
    assert(int256_t(0xffffffffffffffull).count_leading_zeros() == 200);
    assert((int256_t{0,0xff}).count_leading_zeros() == 184);

    /* const reference */
    int256_t a{0,0xff00};
    assert(clz(a) == 176);

    /* r-value reference */
    assert(clz(int256_t{0,0xff}) == 184);
}

void test_ctz()
{
    assert(int256_t(0).count_trailing_zeros() == 256);
    assert(int256_t(1).count_trailing_zeros() == 0);
    assert(int256_t(2).count_trailing_zeros() == 1);
    assert((int256_t{0,0,0,0xffffffffffffffffull}).count_trailing_zeros() == 192);
    assert((int256_t{0,0,0,0xffffffffffffff00ull}).count_trailing_zeros() == 200);
    assert(int256_t(0xffffffffffffffull).count_trailing_zeros() == 0);
    assert((int256_t{0,0xff}).count_trailing_zeros() == 64);

    /* const reference */
    int256_t a{0,0xff00};
    assert(ctz(a) == 72);

    /* r-value reference */
    assert(ctz(int256_t{0,0xff}) == 64);
}

int main(int argc, char const *argv[])
{
    test_i48<int48_t>();
    test_i256<int256_t>();
    test_mask_48();
    test_shift_256();
    test_multiply();
    test_clz();
    test_ctz();
}
