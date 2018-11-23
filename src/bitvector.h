// See LICENSE.md

#pragma once

#include <cstdlib>
#include <cstddef>
#include <cstring>
#include <vector>
#include <string>
#include <limits>
#include <iostream>
#include <algorithm>
#include <initializer_list> 

/*------------------.
| signedness.       |
`------------------*/

struct is_signed {};
struct is_unsigned {};

struct signedness
{
	bool is_signed;

	signedness() = delete;
	signedness(struct is_signed) : is_signed(true) {}
	signedness(struct is_unsigned) : is_signed(false) {}
	signedness(const signedness &o) = default;
	signedness& operator = (const signedness &) = default; 
};

/*------------------.
| bitwidth.         |
`------------------*/

struct bitwidth
{
	unsigned width;

	bitwidth() = delete;
	bitwidth(unsigned bits) : width(bits) {}
	bitwidth& operator = (const bitwidth &) = default;
	operator unsigned () const { return width; }
};

/*------------------.
| integer.          |
`------------------*/

template<size_t _size, bool _signed = true>
struct integer {
	typedef typename std::conditional<
		_size == 8,
		typename std::conditional<_signed, int8_t, uint8_t>::type,
		typename std::conditional<
			_size == 16,
			typename std::conditional<_signed, int16_t, uint16_t>::type,
			typename std::conditional<
				_size == 32,
				typename std::conditional<_signed, int32_t, uint32_t>::type,
				typename std::conditional<
					_size == 64,
					typename std::conditional<_signed, int64_t, uint64_t>::type,
					void
				>::type
			>::type
		>::type
	>::type type;
};

/*------------------.
| bitvector.        |
`------------------*/

struct bitvector
{
	/*------------------.
	| type definitions. |
	`------------------*/

	/*! limb bit width and bit shift */
	enum {
		limb_bits = 32,
		limb_shift = 5,
	};

	/*! limb type */
	typedef integer<limb_bits, false>::type limb_t;
	typedef integer<limb_bits*2, false>::type limb2_t;

	/*------------------.
	| member variables. |
	`------------------*/

	/* limbs is a vector of words with the little end at offset 0 */
	std::vector<limb_t> limbs;

	/*! flags indicating unsigned or signed two's complement */
	signedness s;

	/*! contains the width of the bit vector in bits (variable width = 0) */
	bitwidth bits;


	/*--------------.
	| constructors. |
	`--------------*/

	/*! empty constructor */
	bitvector(const signedness s = is_unsigned(), const bitwidth bits = 0);

	/*! integral constructor */
	bitvector(const limb_t n, const signedness s = is_unsigned(), const bitwidth bits = 0);

	/*! array constructor */
	bitvector(const std::initializer_list<limb_t> l, const signedness s =  is_unsigned(), const bitwidth bits = 0);

	/*! string constructor */
	bitvector(std::string str, const signedness s =  is_unsigned(), const bitwidth bits = 0);

	/*! string constructor with radix */
	bitvector(std::string str, const size_t radix, const signedness s =  is_unsigned(), const bitwidth bits = 0);

	/*! copy constructor */
	bitvector(const bitvector &operand);

	/*! move constructor */
	bitvector(const bitvector&& operand) noexcept;


	/*----------------------.
	| assignment operators. |
	`----------------------*/

	/*! integral copy assignment operator */
	bitvector& operator=(const limb_t l);

	/*! bitvector copy assignment operator */
	bitvector& operator=(const bitvector &operand);

	/*! bitvector move assignment operator */
	bitvector& operator=(bitvector &&operand);


	/*------------------.
	| internal methods. |
	`------------------*/

	/*! expand limbs to match operand */
	void _expand(const bitvector &operand);

	/*! contract zero big end limbs */
	void _contract();

	/*! resize number of limbs */
	void _resize(size_t n);


	/*-------------------------------.
	| limb and bit accessor methods. |
	`-------------------------------*/

	/*! return number of limbs */
	size_t num_limbs() const;

	/*! return maximum number of limbs */
	size_t max_limbs() const;

	/*! access word at limb offset */
	limb_t limb_at(size_t n) const;

	/*! limb_mask at limb offset */
	limb_t limb_mask(size_t n) const;

	/*! test bit at bit offset */
	int test_bit(size_t n) const;

	/*! set bit at bit offset */
	void set_bit(size_t n);

	/*! return number of bits */
	size_t num_bits() const;

	/*! test sign */
	bool sign_bit() const;


	/*---------------------------------------------.
	| add, subtract, shifts and logical operators. |
	`---------------------------------------------*/

	/*! add with carry equals */
	bitvector& operator+=(const bitvector &operand);

	/*! subtract with borrow equals */
	bitvector& operator-=(const bitvector &operand);

	/*! left shift equals */
	bitvector& operator<<=(size_t shamt);

	/*! right shift equals */
	bitvector& operator>>=(size_t shamt);

	/*! bitwise and equals */
	bitvector& operator&=(const bitvector &operand);

	/*! bitwise or equals */
	bitvector& operator|=(const bitvector &operand);

	/*! bitwise xor equals */
	bitvector& operator^=(const bitvector &operand);

	/*! add with carry */
	bitvector operator+(const bitvector &operand) const;

	/*! subtract with borrow */
	bitvector operator-(const bitvector &operand) const;

	/*! left shift */
	bitvector operator<<(size_t shamt) const;

	/*! right shift */
	bitvector operator>>(size_t shamt) const;

	/*! bitwise and */
	bitvector operator&(const bitvector &operand) const;

	/*! bitwise or */
	bitvector operator|(const bitvector &operand) const;

	/*! bitwise xor */
	bitvector operator^(const bitvector &operand) const;

	/*! bitwise not */
	bitvector operator~() const;

	/*! negate */
	bitvector operator-() const;


	/*----------------------.
	| comparison operators. |
	`----------------------*/

	/*! equals */
	bool operator==(const bitvector &operand) const;

	/*! less than */
	bool operator<(const bitvector &operand) const;

	/*! not equals */
	bool operator!=(const bitvector &operand) const;

	/*! less than or equal*/
	bool operator<=(const bitvector &operand) const;

	/*! greater than */
	bool operator>(const bitvector &operand) const;

	/*! less than or equal*/
	bool operator>=(const bitvector &operand) const;

	/*! not */
	bool operator!() const;


	/*-------------------------.
	| multply, divide and pow. |
	`-------------------------*/

	/*! base 2^limb_bits multiply */
	static void mult(const bitvector &multiplicand, const bitvector multiplier, bitvector &result);

	/*! base 2^limb_bits division */
	static void divrem(const bitvector &dividend, const bitvector &divisor, bitvector &quotient, bitvector &remainder);

	/*! multiply */
	bitvector operator*(const bitvector &operand) const;

	/*! division quotient */
	bitvector operator/(const bitvector &divisor) const;

	/*! division remainder */
	bitvector operator%(const bitvector &divisor) const;

	/*! multiply equals */
	bitvector& operator*=(const bitvector &operand);

	/*! divide equals */
	bitvector& operator/=(const bitvector &operand);

	/*! modulus equals */
	bitvector& operator%=(const bitvector &operand);

	/*! raise to the power */
	bitvector pow(size_t exp) const;


	/*-------------------.
	| string conversion. |
	`-------------------*/

	/*! convert bitvector to string */
	std::string to_string(size_t radix = 10) const;

	/*! convert bitvector from string */
	void from_string(const char *str, size_t len, size_t radix);

};

/*------------------.
| inttype.          |
`------------------*/

struct inttype
{
	bitvector v;
	inttype() = delete;
	inttype(const bitvector::limb_t n, const signedness s, const bitwidth b) : v(n, s, b) {}
	inttype(const std::initializer_list<bitvector::limb_t> l, const signedness s, const bitwidth b) : v(l, s, b) {}
	operator const bitvector& () const { return v; }
	inttype& operator = (const inttype &) = default; 
};

struct Uint8 : inttype {
	Uint8(uint8_t v) : inttype(v, is_unsigned(), 8) {}
};
struct Sint8 : inttype {
	Sint8(int8_t v) : inttype(v, is_signed(), 8) {}
};
struct Uint16 : inttype {
	Uint16(uint16_t v) : inttype(v, is_unsigned(), 16) {}
};
struct Sint16 : inttype {
	Sint16(int16_t v) : inttype(v, is_signed(), 16) {}
};
struct Uint32 : inttype {
	Uint32(uint32_t v) : inttype(v, is_unsigned(), 32) {}
};
struct Sint32 : inttype {
	Sint32(int32_t v) : inttype(v, is_signed(), 32) {}
};
struct Uint64 : inttype {
	Uint64(uint64_t v) : inttype({ uint32_t(v), uint32_t(v >> 32) }, is_unsigned(), 64) {}
};
struct Sint64 : inttype {
	Sint64(int64_t v) : inttype({ uint32_t(v), uint32_t(v >> 32) }, is_signed(), 64) {}
};
