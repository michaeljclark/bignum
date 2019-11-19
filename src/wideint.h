#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <array>

#include "bits.h"
#include "hostint.h"

/*------------------.
| wideint.          |
`------------------*/

template <size_t bits, bool is_signed = true>
struct wideint
{
    /*------------------.
    | type definitions. |
    `------------------*/

    /*! limb bit width and bit shift */
    enum {
        nb = bits,
        lb = sizeof(void*)<<3,
        ll2 = int_t<lb>::traits::log2,
        lsm = ((1 << ll2)-1),
        lc = (bits + lb - 1) >> ll2,

        is_pow2 = ((nb & -nb) == nb),

        num_bits = nb,
        num_bytes = nb>>3,
        limb_bits = lb,
        limb_count = lc
    };

    /*! limb types */
    typedef typename hostint<lb, false>::type ulimb_t;
    typedef typename hostint<lb, true>::type slimb_t;
    typedef wideint<bits,is_signed> value_type;

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

    /*! copy constructors */
    inline wideint(const wideint<bits,false> &o) : limbs(o.limbs) {};
    inline wideint(const wideint<bits,true> &o) : limbs(o.limbs) {};


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

    /*! wideint copy assignment operator */
    inline wideint& operator=(const wideint<bits,false> &o) { std::memcpy(limbs,o.limbs,sizeof(limbs)); }
    inline wideint& operator=(const wideint<bits,true> &o) { std::memcpy(limbs,o.limbs,sizeof(limbs)); }


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

};
