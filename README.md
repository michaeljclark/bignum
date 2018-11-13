# bitvector

C++ bitvector with support for arbitrary precision integer arithmetic.

- bit vector with bignum support using 32-bit limbs.
- supports static or dynamic width.
- supports arbitrary precision signed and unsigned arithmetic.
- supports operator overloads for C++ math, logical and bitwise operators.
- multiplication and division algorithm from Hacker's Delight.
- divide and conquer algorithm for radix 10 conversion to string.

### Example code for C++ bitvector bignum arithmetic:

```
#include <iostream>

#include <bitvector.h>

int main()
{
	bitvector x = 2;
	bitvector y = x.pow(4096);
	std::cout << y.to_string(10) << std::endl;
}
```

### Equivalent code using GMP (GNU Multiple Precision Arithmetic Library):

```
#include <iostream>

#include <gmp.h>

int main()
{
	mpz_t result, base;

	mpz_init(base);
	mpz_init(result);
	mpz_set_ui(base, 2);
	mpz_pow_ui(result, base, 4096);

	std::cout << mpz_get_str(NULL, 10, result) << std::endl;

	mpz_clear(base);
	mpz_clear(result);
}
```

## bitvector API

bitvector implements the following operators and methods:

- bitvector(unsigned int n)
- bitvector(std::string str, size_t radix)
- bitvector& operator+=(const bitvector &operand)
- bitvector& operator-=(const bitvector &operand)
- bitvector& operator<<=(int shamt)
- bitvector& operator>>=(int shamt)
- bitvector& operator&=(const bitvector &operand)
- bitvector& operator|=(const bitvector &operand)
- bitvector operator+(const bitvector &operand) const
- bitvector operator-(const bitvector &operand) const
- bitvector operator<<(int shamt) const
- bitvector operator>>(int shamt) const
- bitvector operator&(const bitvector &operand) const
- bitvector operator|(const bitvector &operand) const
- bool operator==(const bitvector &operand) const
- bool operator<(const bitvector &operand) const
- bool operator!=(const bitvector &operand) const
- bool operator<=(const bitvector &operand) const
- bool operator>(const bitvector &operand) const
- bool operator>=(const bitvector &operand) const
- bool operator!() const
- bitvector operator*=(const bitvector &operand)
- bitvector operator/=(const bitvector &divisor)
- bitvector operator%=(const bitvector &divisor)
- bitvector operator*(const bitvector &operand) const
- bitvector operator/(const bitvector &divisor) const
- bitvector operator%(const bitvector &divisor) const
- void from_string(std::string, size_t radix = 0 /*autodetect*/)
- std::string to_string(size_t radix = 10) const
