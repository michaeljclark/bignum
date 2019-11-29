// See LICENSE.md

#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <array>
#include <string>
#include <algorithm>
#include <initializer_list>

#include "bits.h"
#include "hostint.h"

/*------------------.
| wideint.          |
`------------------*/

template <size_t bits, bool is_signed = true, size_t limb_bits_param = 64 >
struct wideint
{
    /*------------------.
    | type definitions. |
    `------------------*/

    /*! limb bit width and bit shift */
    enum {
        nb = bits,
        lb = limb_bits_param,
        ll2 = int_t<lb>::traits::log2,
        lsm = ((1 << ll2)-1),
        lc = (bits + lb - 1) >> ll2,

        is_pow2 = ((nb & -nb) == nb),

        num_bits = nb,
        num_bytes = nb>>3,
        limb_shift = ll2,
        limb_bits = lb,
        limbh_bits = 32,
        limb_count = lc
    };

    /*! limb types */
    typedef typename hostint<lb, false>::type ulimb_t;
    typedef typename hostint<lb, true>::type slimb_t;

    typedef uint32_t uhlimb_t;
    typedef uint64_t udhlimb_t;
    typedef int64_t sdhlimb_t;
    typedef wideint<bits,is_signed,32> hwideint;
    typedef wideint<bits*2,is_signed,32> dhwideint;

    typedef wideint<bits,is_signed,limb_bits> value_type;

    /*------------------.
    | member variables. |
    `------------------*/

    /* limbs is an array of words with the little end at offset 0 */
    std::array<ulimb_t,lc> limbs;


    /*--------------.
    | constructors. |
    `--------------*/

    /*! empty constructor */
    inline wideint() : limbs{0} {}

    /*! integral constructor */
    inline wideint(const ulimb_t n) {
        limbs[0] = n & limb_mask(0);
        for(size_t i = 1; i < lc; i++) {
            limbs[i] = 0;
        }
    }

    /*! unsigned copy constructor */
    inline wideint(const wideint<bits,false,limb_bits> &o) : limbs(o.limbs) {};

    /*! signed copy constructor */
    inline wideint(const wideint<bits,true,limb_bits> &o) : limbs(o.limbs) {};

    /*! different size copy constructor */
    template <size_t o_bits, bool o_signed, size_t o_limb_bits>
    inline wideint(const wideint<o_bits,o_signed,o_limb_bits> &o)
    {
        uint8_t *d = (uint8_t*)(void*)limbs.data();

        size_t sz_min = std::min(sizeof(limbs), sizeof(o.limbs));
        std::memcpy(d, o.limbs.data(), sz_min);

        if (sizeof(limbs) > sizeof(o.limbs)) {
            std::memset(d + sizeof(o.limbs), 0, sizeof(limbs) - sizeof(o.limbs));
        }

        limbs[lc-1] &= limb_mask(lc-1);
    }

    /*! initializer list */
    inline wideint(const std::initializer_list<ulimb_t> l)
    {
        auto li = l.begin();
        for(size_t i = 0; i < lc; i++) {
            if (li == l.end()) limbs[i] = 0;
            else limbs[i] = *li++;
        }
    }

    /*! string */
    inline wideint(std::string str)
        : limbs{0}
    {
        from_string(str.c_str(), str.size(), 0);
    }


    /*----------------------.
    | assignment operators. |
    `----------------------*/

    /*! integral copy assignment operator */
    inline wideint& operator=(const ulimb_t n) {
        limbs[0] = n & limb_mask(0);
        for(size_t i = 1; i < lc; i++) {
            limbs[i] = 0;
        }
        return *this;
    }

    /*! copy assignment operator */
    template <size_t o_bits, bool o_signed, size_t o_limb_bits>
    inline wideint& operator=(const wideint<o_bits,o_signed,o_limb_bits> &o)
    {
        uint8_t *d = (uint8_t*)(void*)limbs.data();

        size_t sz_min = std::min(sizeof(limbs), sizeof(o.limbs));
        std::memcpy(d, o.limbs.data(), sz_min);

        if (sizeof(limbs) > sizeof(o.limbs)) {
            std::memset(d + sizeof(o.limbs), 0, sizeof(limbs) - sizeof(o.limbs));
        }

        limbs[lc-1] &= limb_mask(lc-1);

        return *this;
    }

    /*-------------------------------.
    | limb and bit accessor methods. |
    `-------------------------------*/

    /*! limb_mask at limb offset */
    constexpr ulimb_t limb_mask(size_t n) const
    {
        if ((bits & ((1<<ll2)-1)) == 0) return -1;
        if (n < lc-1) return -1;
        if (n > lc-1) return 0;
        return (ulimb_t(1) << (bits & ((1<<ll2)-1))) - 1;
    }

    /*! test bit at bit offset */
    int test_bit(size_t n) const
    {
        size_t word = n >> ll2;
        return (limbs[word] >> (n & lsm)) & 1;
    }

    /*! set bit at bit offset */
    void set_bit(size_t n)
    {
        size_t word = n >> ll2;
        limbs[word] |= (1ULL << (n & lsm)) & limb_mask(word);
    }

    /*! test sign */
    bool sign_bit() const
    {
        return is_signed && bits > 0 ? test_bit(bits - 1) : 0;
    }


    /*---------------------------------------------.
    | add, subtract, shifts and logical operators. |
    `---------------------------------------------*/

    /*! add with carry equals */
    wideint& op_add(const wideint &operand)
    {
        ulimb_t carry = 0;
        for (size_t i = 0; i < lc; i++) {
            ulimb_t old_val = limbs[i];
            ulimb_t new_val = old_val + operand.limbs[i] + carry;
            limbs[i] = new_val & limb_mask(i);
            carry = new_val < old_val;
        }
        return *this;
    }

    /*! subtract with borrow equals */
    wideint& op_sub(const wideint &operand)
    {
        ulimb_t borrow = 0;
        for (size_t i = 0; i < lc; i++) {
            ulimb_t old_val = limbs[i];
            ulimb_t new_val = old_val - operand.limbs[i] - borrow;
            limbs[i] = new_val & limb_mask(i);
            borrow = new_val > old_val;
        }
        return *this;
    }

    /*! left shift equals */
    wideint& op_shl(int shamt)
    {
        shamt = is_pow2 ? shamt & (nb-1) /* -shift -> mod width */
                        : shamt < 0 ? nb - shamt : shamt % nb;
        ptrdiff_t ls = shamt >> ll2;
        shamt -= ls << ll2;
        if (shamt == 0) {
            for (ptrdiff_t i = lc-1; i >= ls; i--) {
                limbs[i] = limbs[i-ls];
            }
        } else {
            ptrdiff_t i = lc-1;
            ulimb_t n = limbs[i-ls];
            for (; i > ls; i--) {
                ulimb_t m = limbs[i-ls-1];
                limbs[i] = (n << shamt | m >> (-shamt & lsm));
                n = m;
            }
            limbs[i] = (n << shamt);
        }
        for (ptrdiff_t i = ls-1; i >= 0; i--) {
            limbs[i] = 0;
        }
        limbs[lc-1] &= limb_mask(lc-1);
        return *this;
    }

    /*! right shift equals */
    wideint& op_shr(int shamt)
    {
        shamt = is_pow2 ? shamt & (nb-1) /* -shift -> mod width */
                        : shamt < 0 ? nb - shamt : shamt % nb;
        size_t ls = shamt >> ll2;
        shamt -= ls << ll2;
        ulimb_t s = is_signed && sign_bit();
        if (shamt == 0) {
            for (size_t i = 0; i < lc-ls; i++) {
                limbs[i] = limbs[i+ls];
            }
        } else {
            size_t i = 0;
            ulimb_t n = limbs[i+ls];
            for (; i < lc-ls-1; i++) {
                ulimb_t m = limbs[i+ls+1];
                limbs[i] = (n >> shamt | m << (-shamt & lsm));
                n = m;
            }
            /* arithmetic shift if signed and this word is the last word */
            ulimb_t lw = s && (i == lc-1) ? slimb_t(n) >> shamt : n >> shamt;
            limbs[i] = lw | -s << ((nb-shamt-1) & lsm);
        }
        for (size_t i = lc-ls; i < lc; i++) {
            limbs[i] = -s;
        }
        limbs[lc-1] &= limb_mask(lc-1);
        return *this;
    }

    /*! bitwise and equals */
    wideint& op_and(const wideint &operand)
    {
        for (size_t i = 0; i < lc; i++) {
            limbs[i] = (operand.limbs[i] & limbs[i]) & limb_mask(i);
        }
        return *this;
    }

    /*! bitwise or equals */
    wideint& op_or(const wideint &operand)
    {
        for (size_t i = 0; i < lc; i++) {
            limbs[i] = (operand.limbs[i] | limbs[i]) & limb_mask(i);
        }
        return *this;
    }

    /*! bitwise xor equals */
    wideint& op_xor(const wideint &operand)
    {
        for (size_t i = 0; i < lc; i++) {
            limbs[i] = (operand.limbs[i] ^ limbs[i]) & limb_mask(i);
        }
        return *this;
    }

    /*! bitwise not */
    wideint& op_not() const
    {
        for (size_t i = 0; i < lc; i++) {
            limbs[i] = ~limbs[i] & limb_mask(i);
        }
        return *this;
    }

    /*! negate */
    wideint& op_neg() const
    {
        for (size_t i = 0; i < lc; i++) {
            limbs[i] = ~limbs[i] & limb_mask(i);
        }
        *this += 1;
        return *this;
    }

    /*! base 2^limb_bits multiply */
    static void op_mult(const wideint &multiplicand, const wideint multiplier, wideint &result)
    {
        /* This routine is derived from Hacker's Delight,
         * and possibly originates from Knuth */

        const uhlimb_t *a = (uhlimb_t*)(void*)multiplier.limbs.data();
        const uhlimb_t *b = (uhlimb_t*)(void*)multiplicand.limbs.data();
        dhwideint tmp;

        // skip zeros in the big end limbs
        size_t m = hwideint::lc, n = hwideint::lc, k = dhwideint::lc;
        for (;m > 1 && b[m-1] == 0; m--);
        for (;n > 1 && a[n-1] == 0; n--);

        uhlimb_t carry = 0;
        udhlimb_t mj = a[0];
        for (size_t i = 0; i < m && i < k; i++) {
            udhlimb_t t = udhlimb_t(b[i]) * mj + carry;
            tmp.limbs[i] = uhlimb_t(t);
            carry = t >> limbh_bits;
        }
        if (m < k) {
            tmp.limbs[m] = carry;
        }
        for (size_t j = 1; j < n; j++) {
            carry = 0;
            mj = a[j];
            for (size_t i = 0; i < m && i + j < k; i++) {
                udhlimb_t t = udhlimb_t(b[i]) * mj + udhlimb_t(tmp.limbs[i + j]) + carry;
                tmp.limbs[i + j] = uhlimb_t(t);
                carry = t >> limbh_bits;
            }
            if (j + m < k) {
                tmp.limbs[j + m] = carry;
            }
        }

        result = tmp;
    }

    /*! base 2^limb_bits division */
    static void op_divrem(const wideint &dividend, const wideint &divisor,
        wideint &res_quotient, wideint &res_remainder)
    {
        /* This routine is derived from Hacker's Delight,
         * and possibly originates from Knuth */

        dhwideint quotient = 0;
        dhwideint remainder = 0;

        /*
         * Note: we access the operand limbs as 32-bit words because there
         * is no standard 128-bit result op :uint64 * :uint64 -> :uint128
         * Also, the results for the wide
         */
        uhlimb_t *q = quotient.limbs.data(), *r = remainder.limbs.data();
        const uhlimb_t *u = (uhlimb_t*)(void*)dividend.limbs.data();
        const uhlimb_t *v = (uhlimb_t*)(void*)divisor.limbs.data();

        // skip zeros in the big end limbs
        ptrdiff_t m = hwideint::lc, n = hwideint::lc;
        for (;m > 1 && u[m-1] == 0; m--);
        for (;n > 1 && v[n-1] == 0; n--);

        const udhlimb_t b = (1ULL << limbh_bits); // Number base
        uhlimb_t *un, *vn;                        // Normalized form of u, v.
        udhlimb_t qhat;                           // Estimated quotient digit.
        udhlimb_t rhat;                           // A remainder.

        if (m < n || n <= 0 || v[n-1] == 0) {
            res_quotient = 0;
            res_remainder = dividend;
            return;
        }

        // Single digit divisor
        if (n == 1) {
            udhlimb_t k = 0;
            for (ptrdiff_t j = m - 1; j >= 0; j--) {
                q[j] = uhlimb_t((k*b + u[j]) / v[0]);
                k = (k*b + u[j]) - q[j]*v[0];
            }
            r[0] = uhlimb_t(k);
            res_quotient = quotient;
            res_remainder = remainder;
            return;
        }

        // Normalize by shifting v left just enough so that
        // its high-order bit is on, and shift u left the
        // same amount. We may have to append a high-order
        // digit on the dividend; we do that unconditionally.

        int s = clz(v[n-1]); // 0 <= s <= limb_bits.
        vn = (uhlimb_t *)alloca(sizeof(uhlimb_t) * n);
        for (ptrdiff_t i = n - 1; i > 0; i--) {
            vn[i] = (v[i] << s) | (v[i-1] >> (limbh_bits-s));
        }
        vn[0] = v[0] << s;

        un = (uhlimb_t *)alloca(sizeof(uhlimb_t) * (m + 1));
        un[m] = u[m-1] >> (limbh_bits-s);
        for (ptrdiff_t i = m - 1; i > 0; i--) {
            un[i] = (u[i] << s) | (u[i-1] >> (limbh_bits-s));
        }
        un[0] = u[0] << s;
        for (ptrdiff_t j = m - n; j >= 0; j--) { // Main loop.
            // Compute estimate qhat of q[j].
            qhat = (un[j+n]*b + un[j+n-1]) / vn[n-1];
            rhat = (un[j+n]*b + un[j+n-1]) - qhat * vn[n-1];
        again:
            if (qhat >= b || qhat*vn[n-2] > b*rhat + un[j+n-2]) {
                qhat = qhat - 1;
                rhat = rhat + vn[n-1];
                if (rhat < b) goto again;
            }
            // Multiply and subtract.
            udhlimb_t k = 0;
            sdhlimb_t t = 0;
            for (ptrdiff_t i = 0; i < n; i++) {
                unsigned long long p = qhat*vn[i];
                t = un[i+j] - k - (p & ((1ULL<<limbh_bits)-1));
                un[i+j] = uhlimb_t(t);
                k = (p >> limbh_bits) - (t >> limbh_bits);
            }
            t = un[j+n] - k;
            un[j+n] = uhlimb_t(t);

            q[j] = uhlimb_t(qhat); // Store quotient digit.
            if (t < 0) {          // If we subtracted too
                q[j] = q[j] - 1;  // much, add back.
                k = 0;
                for (ptrdiff_t i = 0; i < n; i++) {
                    t = un[i+j] + vn[i] + k;
                    un[i+j] = uhlimb_t(t);
                    k = t >> limbh_bits;
                }
                un[j+n] = uhlimb_t(un[j+n] + k);
            }
        }

        // normalize remainder
        for (ptrdiff_t i = 0; i < n; i++) {
            r[i] = (un[i] >> s) | (un[i + 1] << (limbh_bits - s));
        }

        res_quotient = quotient;
        res_remainder = remainder;
    }

    /*----------------------.
    | comparison operators. |
    `----------------------*/

    /*! equals */
    bool op_eq(const wideint &operand) const
    {
        for (size_t i = 0; i < lc; i++) {
            if (limbs[i] != operand.limbs[i]) return false;
        }
        return true;
    }

    /*! less than */
    bool op_lt(const wideint &operand) const
    {
        /* handle signed comparison if both operands are signed */
        if (is_signed) {
            bool sign = sign_bit();
            if (sign ^ operand.sign_bit()) {
                return sign;
            } else if (sign) {
                return operand < *this;
            }
        }

        /* unsigned comparison */
        for (ptrdiff_t i = lc-1; i >= 0; i--) {
            if (limbs[i] > operand.limbs[i]) return false;
            else if (limbs[i] < operand.limbs[i]) return true;
        }
        return false;
    }

    /*! not equal, less than equals, greater than, greater than equals, not equal zero */
    bool op_neq(const wideint &operand) const { return !(*this == operand); }
    bool op_lte(const wideint &operand) const { return *this < operand || *this == operand; }
    bool op_gt(const wideint &operand) const { return !(*this <= operand); }
    bool op_gte(const wideint &operand) const { return !(*this < operand) || *this == operand; }
    bool op_nez() const { return *this == 0; }

    /*--------------------.
    | power via squaring. |
    `--------------------*/

    /*! raise to the power */
    wideint pow(size_t exp) const
    {
        if (exp == 0) return 1;
        wideint x = *this, y = 1;
        while (exp > 1) {
            if ((exp & 1) == 0) {
                exp >>= 1;
            } else {
                y *= x;
                exp = (exp - 1) >> 1;
            }
            x *= x;
        }
        return x * y;
    }

    /*----------------------.
    | count leading zeros.  |
    `----------------------*/

    size_t count_leading_zeros() const
    {
        for (ptrdiff_t i = lc-1; i >= 0; i--) {
            if (limbs[i] != 0) {
                return clz(limbs[i]) + ((lc-1-i) << limb_shift);
            }
        }
        return lc << limb_shift;
    }

    /*----------------------.
    | count trailing zeros. |
    `----------------------*/

    size_t count_trailing_zeros() const
    {
        for (size_t i = 0; i < lc; i++) {
            if (limbs[i] != 0) {
                return ctz(limbs[i]) + (i << limb_shift);
            }
        }
        return lc << limb_shift;
    }

    /*----------------------.
    | operator overloads.   |
    `----------------------*/

    wideint& operator+=(const wideint &operand) { return this->op_add(operand); }
    wideint& operator-=(const wideint &operand) { return this->op_sub(operand); }
    wideint& operator<<=(int shamt) { return this->op_shl(shamt); }
    wideint& operator>>=(int shamt) { return this->op_shr(shamt); }
    wideint& operator&=(const wideint &operand) { return this->op_and(operand); }
    wideint& operator|=(const wideint &operand) { return this->op_or(operand); }
    wideint& operator^=(const wideint &operand) { return this->op_xor(operand); }
    wideint operator+(const wideint &operand) const { return value_type(*this).op_add(operand); }
    wideint operator-(const wideint &operand) const { return value_type(*this).op_sub(operand); }
    wideint operator<<(int shamt) const { return value_type(*this).op_shl(shamt); }
    wideint operator>>(int shamt) const { return value_type(*this).op_shr(shamt); }
    wideint operator&(const wideint &operand) const { return value_type(*this).op_and(operand); }
    wideint operator|(const wideint &operand) const { return value_type(*this).op_or(operand); }
    wideint operator^(const wideint &operand) const { return value_type(*this).op_xor(operand); }
    wideint operator~() const { return value_type(*this).op_not(); }
    wideint operator-() const { return value_type(*this).op_neg(); }
    bool operator==(const wideint &operand) const { return this->op_eq(operand); }
    bool operator<(const wideint &operand) const { return this->op_lt(operand); }
    bool operator!=(const wideint &operand) const { return this->op_neq(operand); }
    bool operator<=(const wideint &operand) const { return this->op_lte(operand); }
    bool operator>(const wideint &operand) const { return this->op_gt(operand); }
    bool operator>=(const wideint &operand) const { return this->op_gte(operand); }
    bool operator!() const { return this->op_nez(); }

    wideint operator*(const wideint &operand) const
    {
        wideint result;
        op_mult(*this, operand, result);
        return result;
    }

    wideint operator/(const wideint &operand) const
    {
        wideint quotient, remainder;
        op_divrem(*this, operand, quotient, remainder);
        return quotient;
    }

    wideint operator%(const wideint &operand) const
    {
        wideint quotient, remainder;
        op_divrem(*this, operand, quotient, remainder);
        return remainder;
    }

    wideint& operator*=(const wideint &operand)
    {
        wideint product;
        op_mult(*this, operand, product);
        return (*this = product);
    }

    wideint& operator/=(const wideint &operand)
    {
        wideint quotient, remainder;
        op_divrem(*this, operand, quotient, remainder);
        return (*this = quotient);
    }

    wideint& operator%=(const wideint &operand)
    {
        wideint quotient, remainder;
        op_divrem(*this, operand, quotient, remainder);
        return (*this = remainder);
    }

    /*-------------------.
    | string conversion. |
    `-------------------*/

    /*! convert from wideint to string */
    std::string to_string(size_t radix = 10) const
    {
        static const char* hexdigits = "0123456789abcdef";

        switch (radix) {
            case 10: {
                if (*this == 0) return "0";

                std::string s;
                wideint v = *this, t = 10, q, r;
                do {
                    op_divrem(v, t, q, r);
                    s.push_back('0' + char(r.limbs[0]));
                    v = q;
                } while (v != 0);

                return std::string(s.rbegin(), s.rend());;
            }
            case 2: {
                if (*this == 0) return "0b0";

                std::string s("0b");
                ulimb_t l1 = limbs.back();
                size_t n = limb_bits - clz(l1);
                size_t t = n + ((wideint::limb_count - 1) << wideint::limb_shift);
                s.resize(t + 2);
                auto i = s.begin() + 2;
                for (ptrdiff_t k = n - 1; k >= 0; k--) {
                    *(i++) = '0' + ((l1 >> k) & 1);
                }
                for (ptrdiff_t j = wideint::limb_count - 2; j >= 0; j--) {
                    ulimb_t l = limbs[j];
                    for (ptrdiff_t k = limb_bits - 1; k >= 0; k--) {
                        *(i++) = '0' + ((l >> k) & 1);
                    }
                }
                return s;
            }
            case 16: {
                if (*this == 0) return "0x0";

                std::string s("0x");
                ulimb_t l1 = limbs.back();
                size_t n = ((limb_bits >> 2) - (clz(l1) >> 2));
                size_t t = n + ((wideint::limb_count - 1) << (wideint::limb_shift - 2));
                s.resize(t + 2);
                auto i = s.begin() + 2;
                for (ptrdiff_t k = n - 1; k >= 0; k--) {
                    *(i++) = hexdigits[(l1 >> (k << 2)) & 0xf];
                }
                for (ptrdiff_t j = wideint::limb_count - 2; j >= 0; j--) {
                    ulimb_t l = limbs[j];
                    for (ptrdiff_t k = (limb_bits >> 2) - 1; k >= 0; k--) {
                        *(i++) = hexdigits[(l >> (k << 2)) & 0xf];
                    }
                }
                return s;
            }
            default: {
                return std::string();
            }
        }
    }

    /*! convert to wideint from string */
    void from_string(const char *str, size_t len, size_t radix)
    {
        static const wideint tenp18{0xde0b6b3a7640000ull};
        static const wideint twop64{0,1};
        if (len > 2) {
            if (strncmp(str, "0b", 2) == 0) {
                radix = 2;
                str += 2;
                len -= 2;
            } else if (strncmp(str, "0x", 2) == 0) {
                radix = 16;
                str += 2;
                len -= 2;
            }
        }
        if (radix == 0) {
            radix = 10;
        }
        switch (radix) {
            case 10: {
                for (size_t i = 0; i < len; i += 18) {
                    size_t chunklen = i + 18 < len ? 18 : len - i;
                    std::string chunk(str + i, chunklen);
                    udhlimb_t num = strtoull(chunk.c_str(), nullptr, 10);
                    if (chunklen == 18) {
                        *this *= tenp18;
                    } else {
                        *this *= wideint(10).pow(chunklen);
                    }
                    *this += wideint{ulimb_t(num)};
                }
                break;
            }
            case 2: {
                for (size_t i = 0; i < len; i += 64) {
                    size_t chunklen = i + 64 < len ? 64 : len - i;
                    std::string chunk(str + i, chunklen);
                    udhlimb_t num = strtoull(chunk.c_str(), nullptr, 2);
                    if (chunklen == 64) {
                        *this *= twop64;
                    } else {
                        *this *= wideint(2).pow(chunklen);
                    }
                    *this += wideint{ulimb_t(num)};
                }
                break;
            }
            case 16: {
                for (size_t i = 0; i < len; i += 16) {
                    size_t chunklen = i + 16 < len ? 16 : len - i;
                    std::string chunk(str + i, chunklen);
                    udhlimb_t num = strtoull(chunk.c_str(), nullptr, 16);
                    if (chunklen == 16) {
                        *this *= twop64;
                    } else {
                        *this *= wideint(16).pow(chunklen);
                    }
                    *this += wideint{ulimb_t(num)};
                }
                break;
            }
            default: {
                limbs = { 0 };
            }
        }
    }
};

template <size_t bits, bool is_signed = true, size_t limb_bits_param = 64 >
inline int clz(const wideint<bits,is_signed,limb_bits_param> &val) { return val.count_leading_zeros(); }

template <size_t bits, bool is_signed = true, size_t limb_bits_param = 64 >
inline int clz(wideint<bits,is_signed,limb_bits_param> &&val) { return val.count_leading_zeros(); }

template <size_t bits, bool is_signed = true, size_t limb_bits_param = 64 >
inline int ctz(const wideint<bits,is_signed,limb_bits_param> &val) { return val.count_trailing_zeros(); }

template <size_t bits, bool is_signed = true, size_t limb_bits_param = 64 >
inline int ctz(wideint<bits,is_signed,limb_bits_param> &&val) { return val.count_trailing_zeros(); }