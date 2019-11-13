# bignum

C++ bignum with support for arbitrary precision integer arithmetic.

- bit vector with bignum support using 32-bit limbs.
- supports static or dynamic width.
- supports arbitrary precision signed and unsigned arithmetic.
- supports operator overloads for C++ math, logical and bitwise operators.
- multiplication and division algorithm from Hacker's Delight.
- divide and conquer algorithm for radix 10 conversion to string.

### Example code for C++ bignum arithmetic:

```
#include <iostream>

#include <bignum.h>

int main()
{
	bignum x = 2;
	bignum y = x.pow(4096);
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

## bignum API

bignum implements the following operators and methods:

- bignum(unsigned int n)
- bignum(std::string str, size_t radix)
- bignum& operator+=(const bignum &operand)
- bignum& operator-=(const bignum &operand)
- bignum& operator<<=(int shamt)
- bignum& operator>>=(int shamt)
- bignum& operator&=(const bignum &operand)
- bignum& operator|=(const bignum &operand)
- bignum operator+(const bignum &operand) const
- bignum operator-(const bignum &operand) const
- bignum operator<<(int shamt) const
- bignum operator>>(int shamt) const
- bignum operator&(const bignum &operand) const
- bignum operator|(const bignum &operand) const
- bool operator==(const bignum &operand) const
- bool operator<(const bignum &operand) const
- bool operator!=(const bignum &operand) const
- bool operator<=(const bignum &operand) const
- bool operator>(const bignum &operand) const
- bool operator>=(const bignum &operand) const
- bool operator!() const
- bignum operator*=(const bignum &operand)
- bignum operator/=(const bignum &divisor)
- bignum operator%=(const bignum &divisor)
- bignum operator*(const bignum &operand) const
- bignum operator/(const bignum &divisor) const
- bignum operator%(const bignum &divisor) const
- void from_string(std::string, size_t radix = 0 /*autodetect*/)
- std::string to_string(size_t radix = 10) const
