// See LICENSE.md

#include <cassert>

#include "bignum.h"

void test_bignum()
{
	/* test empty constructor */
	bignum b1;
	assert(b1.num_limbs() == 1);
	assert(b1.limb_at(0) == 0);

	/* test integral constructor */
	bignum b2(1);
	assert(b2.num_limbs() == 1);
	assert(b2.limb_at(0) == 1);

	/* test array constructor */
	bignum b3{1, 1};
	assert(b3.num_limbs() == 2);
	assert(b3.limb_at(0) == 1);
	assert(b3.limb_at(1) == 1);

	/* test left shift */
	bignum b4 = 1;
	b4 = b4 << 1;
	assert(b4.num_limbs() == 1);
	assert(b4.limb_at(0) == 2);

	/* test left shift carry overflow */
	bignum b5(std::numeric_limits<bignum::limb_t>::max());
	b5 = b5 << 10;
	assert(b5.num_limbs() == 2);
	assert(b5.limb_at(0) == std::numeric_limits<bignum::limb_t>::max() - 1023);
	assert(b5.limb_at(1) == 1023);

	/* test big left shift */
	bignum b6 = bignum{1, 1} << 2;
	assert(b6.num_limbs() == 2);
	assert(b6.limb_at(0) == 4);
	assert(b6.limb_at(1) == 4);

	/* test right shift carry under */
	assert((bignum{0,1} >> 1) == bignum(1ULL << (bignum::limb_bits - 1)));

	/* test right shift carry under limb contraction */
	assert((bignum{0,1} >> 1).num_limbs() == 1);

	/* test big right shift */
	assert((bignum{0,1} >> 17) == bignum(1ULL << (bignum::limb_bits - 17)));

	/* test logical and */
	bignum b7 = bignum{0b101, 0b101} & bignum{0b100, 0};
	assert(b7.num_limbs() == 1);
	assert(b7.limb_at(0) == 0b100);

	/* test logical or */
	bignum b8 = bignum{0b101, 0b101} | bignum{0b100, 0};
	assert(b8.num_limbs() == 2);
	assert(b8.limb_at(0) == 0b101);
	assert(b8.limb_at(1) == 0b101);

	/* test equals */
	assert((bignum{2,3} == bignum{2,3,0}));
	assert((bignum{2,3,0} == bignum{2,3}));
	assert((bignum{1,2,3} == bignum{1,2,3}));
	assert(!(bignum{1,2,3} == bignum{3,2,3}));
	assert(!(bignum{1,2,3} == 0));
	assert(!(bignum{1,2,3} == 2147483648));

	/* test not equals */
	assert(!(bignum{2,3} != bignum{2,3,0}));
	assert(!(bignum{2,3,0} != bignum{2,3}));
	assert(!(bignum{1,2,3} != bignum{1,2,3}));
	assert((bignum{1,2,3} != bignum{3,2,3}));
	assert((bignum{1,2,3} != 0));
	assert((bignum{1,2,3} != 2147483648));

	/* test less than */
	assert(!(bignum{3,2,3} < bignum{1,2,3}));
	assert(!(bignum{1,2,3} < bignum{1,2,3}));
	assert((bignum{1,2,3} < bignum{3,2,3}));
	assert((bignum{1,2,3} < bignum{3,2,3,1}));
	assert(!(bignum{3,2,3,1} < bignum{1,2,3}));
	assert(!(bignum{3,2,3,1} < 0));
	assert(!(bignum{3,2,3,1} < 2147483648));

	/* test less than equals */
	assert(!(bignum{3,2,3} <= bignum{1,2,3}));
	assert((bignum{1,2,3} <= bignum{1,2,3}));
	assert((bignum{1,2,3} <= bignum{3,2,3}));
	assert((bignum{1,2,3} <= bignum{3,2,3,1}));
	assert(!(bignum{3,2,3,1} <= bignum{1,2,3}));
	assert(!(bignum{3,2,3,1} <= 0));
	assert(!(bignum{3,2,3,1} <= 2147483648));

	/* test greater than */
	assert((bignum{3,2,3} > bignum{1,2,3}));
	assert(!(bignum{1,2,3} > bignum{1,2,3}));
	assert(!(bignum{1,2,3} > bignum{3,2,3}));
	assert(!(bignum{1,2,3} > bignum{3,2,3,1}));
	assert((bignum{3,2,3,1} > bignum{1,2,3}));
	assert((bignum{3,2,3,1} > 0));
	assert((bignum{3,2,3,1} > 2147483648));

	/* test greater than equals */
	assert((bignum{3,2,3} >= bignum{1,2,3}));
	assert((bignum{1,2,3} >= bignum{1,2,3}));
	assert(!(bignum{1,2,3} >= bignum{3,2,3}));
	assert(!(bignum{1,2,3} >= bignum{3,2,3,1}));
	assert((bignum{3,2,3,1} >= bignum{1,2,3}));
	assert((bignum{3,2,3,1} >= 0));
	assert((bignum{3,2,3,1} >= 2147483648));

	/* test not */
	assert(!bignum(0));
	assert(!!bignum(1));

	/* test addition */
	assert((bignum{1,1,1} + bignum{1,1,1} == bignum{2,2,2}));

	/* test addition with carry */
	bignum b9{0,std::numeric_limits<bignum::limb_t>::max()};
	bignum b10{0,std::numeric_limits<bignum::limb_t>::max()-1,1};
	assert(b9 + b9 == b10);

	/* test multiplication */
	bignum b12 = bignum(2147483648) * bignum(2147483648);
	assert(b12.num_limbs() == 2);
	assert(b12.limb_at(0) == 0);
	assert(b12.limb_at(1) == 1073741824);
	bignum b13 = b12 * b12;
	assert(b13.num_limbs() == 4);
	assert(b13.limb_at(0) == 0);
	assert(b13.limb_at(1) == 0);
	assert(b13.limb_at(2) == 0);
	assert(b13.limb_at(3) == 268435456);
	bignum b14 = bignum(2147483647) * bignum(2147483647);
	assert(b14.num_limbs() == 2);
	assert(b14.limb_at(0) == 1);
	assert(b14.limb_at(1) == 1073741823);
	bignum b15 = b14 * b14;
	assert(b15.num_limbs() == 4);
	assert(b15.limb_at(0) == 1);
	assert(b15.limb_at(1) == 2147483646);
	assert(b15.limb_at(2) == 2147483649);
	assert(b15.limb_at(3) == 268435455);

	/* test subtraction */
	assert((bignum{3,3,3} - bignum{1,1,1} == bignum{2,2,2}));

	/* test subtraction with borrow */
	bignum b17{1,1};
	bignum b18{std::numeric_limits<bignum::limb_t>::max()};
	assert(b17-b18 == 2);

	/* test division */
	bignum b19 = b15 / b14;
	assert(b19.num_limbs() == 2);
	assert(b19.limb_at(0) == 1);
	assert(b19.limb_at(1) == 1073741823);
	assert(b19.to_string() == "4611686014132420609");

	/* test set and test bit */
	bignum b20;
	b20.set_bit(64);
	assert(b20.test_bit(64) == 1);
	assert(b20.test_bit(63) == 0);
	b20 = b20 - 1;
	assert(b20.test_bit(64) == 0);
	assert(b20.test_bit(63) == 1);

	/* decimal string formatting */
	assert((bignum(9) * bignum(9)).to_string() == "81");
	assert((bignum(255) * bignum(255)).to_string() == "65025");
	assert((bignum(65535) * bignum(65535)).to_string() == "4294836225");
	assert((bignum(2147483648) * bignum(2147483648)).to_string() == "4611686018427387904");

	/* binary string formatting */
	assert((bignum{0b101}).to_string(2) == "0b101");
	assert((bignum{0b111100001111}).to_string(2) == "0b111100001111");
	assert((bignum{0xff00ff,0xff}).to_string(2) == "0b1111111100000000111111110000000011111111");

	/* hex string formatting */
	assert((bignum{0x1}).to_string(16) == "0x1");
	assert((bignum{0x7f}).to_string(16) == "0x7f");
	assert((bignum{0x3ff}).to_string(16) == "0x3ff");
	assert((bignum{0xffffffff,1}).to_string(16) == "0x1ffffffff");
	assert((bignum{0xffffffff,0x80}).to_string(16) == "0x80ffffffff");
	assert((bignum{0xffffffff,0x400}).to_string(16) == "0x400ffffffff");
	assert((bignum{0x80000000,0x80000000}).to_string(16) == "0x8000000080000000");

	/* pow */
	assert(bignum(71).pow(0) == 1);
	assert(bignum(71).pow(1) == 71);
	assert(bignum(71).pow(17).to_string() == "29606831241262271996845213307591");

	/* from string */
	assert(bignum("71").to_string() == "71");
	assert(bignum("29606831241262271996845213307591").to_string() == "29606831241262271996845213307591");
	assert(bignum("0xdeadbeef").to_string(16) == "0xdeadbeef");
	assert(bignum("0xdeadbeef00ff00ff00ff00ff").to_string(16) == "0xdeadbeef00ff00ff00ff00ff");
	assert(bignum("0b101").to_string(2) == "0b101");
	assert(bignum("0b11110000111100001111000011110000").to_string(2) == "0b11110000111100001111000011110000");
	assert(bignum("3249094308290873429032409832424398902348094329803249083249089802349809430822903").to_string()
		== "3249094308290873429032409832424398902348094329803249083249089802349809430822903");

	/* fixed width tests */
	assert(bignum(0xffffffff, is_unsigned(), 32) + 2 == 1);
	assert(bignum(0xffffffff, is_unsigned(), 31) == 0x7fffffff);
	assert(bignum(0x7fffffff, is_unsigned(), 31) + 2 == 1);
	assert(bignum("0xffffffff", is_unsigned(), 31) == 0x7fffffff);
	assert(bignum(100000) * bignum(100000) == (bignum{0x540be400, 0x2}));
	assert(bignum(100000, is_unsigned(), 34) * bignum(100000, is_unsigned(), 34) == (bignum{0x540be400, 0x2}));
	assert(bignum(100000, is_unsigned(), 33) * bignum(100000, is_unsigned(), 33) == bignum(0x540be400));
	assert(bignum(100000, is_unsigned(), 32) * bignum(100000, is_unsigned(), 32) == bignum(0x540be400));
	assert(bignum(100000, is_unsigned(), 20) * bignum(100000, is_unsigned(), 20) == bignum(0xbe400));
	assert(-bignum(1, is_unsigned(), 32) == bignum(-1, is_unsigned(), 32));
	assert(-bignum(1, is_unsigned(), 64) == bignum({0xffffffff, 0xffffffff}, is_unsigned(), 64));
	assert(-bignum(1, is_unsigned(), 65) == bignum({0xffffffff, 0xffffffff,1}, is_unsigned(), 65));
	assert(bignum({0xffffffff, 0x7fffffff,1}, is_unsigned(), 65) >> 1 == bignum({0xffffffff, 0xbfffffff}, is_unsigned(), 65));
	assert(-bignum(1, is_signed(), 65) >> 1 == bignum({0xffffffff, 0xffffffff,1}, is_signed(), 65));
	assert(bignum({0xffffffff, 0x7fffffff,1}, is_signed(), 65) >> 1 == bignum({0xffffffff, 0xbfffffff,1}, is_signed(), 65));
	assert(bignum({0xffffffff, 0xffffffff}, is_signed(), 65) >> 1 == bignum({0xffffffff, 0x7fffffff}, is_signed(), 65));
	assert(-bignum(1, is_unsigned(), 65) >> 1 == bignum({0xffffffff, 0xffffffff}, is_unsigned(), 65));
	assert(-bignum(1, is_unsigned(), 65) >> 2 == bignum({0xffffffff, 0x7fffffff}, is_unsigned(), 65));
	assert(-bignum(1, is_unsigned(), 65) << 1 == bignum({0xfffffffe, 0xffffffff,1}, is_unsigned(), 65));
	assert(-bignum(1, is_unsigned(), 65) << 2 == bignum({0xfffffffc, 0xffffffff,1}, is_unsigned(), 65));

	/* unsigned comparison */
	assert(bignum(-1, is_unsigned(), 32) > bignum(1, is_unsigned(), 32));
	assert(bignum(0, is_unsigned(), 32) < bignum(1, is_unsigned(), 32));
	assert(bignum(1, is_unsigned(), 32) > bignum(0, is_unsigned(), 32));
	assert(bignum(1, is_unsigned(), 32) < bignum(-1, is_unsigned(), 32));

	/* signed comparison */
	assert(bignum(-1, is_signed(), 32) < bignum(1, is_signed(), 32));
	assert(bignum(0, is_signed(), 32) < bignum(1, is_signed(), 32));
	assert(bignum(1, is_signed(), 32) > bignum(0, is_signed(), 32));
	assert(bignum(1, is_signed(), 32) > bignum(-1, is_signed(), 32));

	assert(bignum(0).num_bits() == 0);
	assert(bignum(1).num_bits() == 1);
	assert(bignum(2).num_bits() == 2);
	assert(bignum(3).num_bits() == 2);
	assert(bignum(4).num_bits() == 3);
	assert(bignum(5).num_bits() == 3);
	assert(bignum(6).num_bits() == 3);
	assert(bignum(7).num_bits() == 3);
}

void test_uint8()
{
	bignum b1 = Uint8(0);
	bignum b2 = Uint8(0);
	bignum b3 = Uint8(0);

	b1 = 127;
	b2 = -1;
	b3 = b1 + b2;

	printf("%s\n", __func__);
	printf("b1=%s (%s)\n", b1.to_string(10).c_str(), b1.to_string(16).c_str());
	printf("b2=%s (%s)\n", b2.to_string(10).c_str(), b2.to_string(16).c_str());
	printf("b3=%s (%s)\n", b3.to_string(10).c_str(), b3.to_string(16).c_str());
}


void test_uint16()
{
	bignum b1 = Uint16(0);
	bignum b2 = Uint16(0);
	bignum b3 = Uint16(0);

	b1 = 0x7fff;
	b2 = -1;

	b3 = b1 + b2;

	printf("%s\n", __func__);
	printf("b1=%s (%s)\n", b1.to_string(10).c_str(), b1.to_string(16).c_str());
	printf("b2=%s (%s)\n", b2.to_string(10).c_str(), b2.to_string(16).c_str());
	printf("b3=%s (%s)\n", b3.to_string(10).c_str(), b3.to_string(16).c_str());
}

void test_uint32()
{
	bignum b1 = Uint32(0);
	bignum b2 = Uint32(0);
	bignum b3 = Uint32(0);

	b1 = 0x7fffffff;
	b2 = 0xffffffff;

	b3 = b1 + b2;

	printf("%s\n", __func__);
	printf("b1=%s (%s)\n", b1.to_string(10).c_str(), b1.to_string(16).c_str());
	printf("b2=%s (%s)\n", b2.to_string(10).c_str(), b2.to_string(16).c_str());
	printf("b3=%s (%s)\n", b3.to_string(10).c_str(), b3.to_string(16).c_str());
}

void test_uint64()
{
	bignum b1 = Uint64(0x0102030405060708);
	bignum b2 = Uint64(0x8070605040302010);
	bignum b3 = Uint64(0);

	b3 = b1 + b2;

	printf("%s\n", __func__);
	printf("b1=%s (%s)\n", b1.to_string(10).c_str(), b1.to_string(16).c_str());
	printf("b2=%s (%s)\n", b2.to_string(10).c_str(), b2.to_string(16).c_str());
	printf("b3=%s (%s)\n", b3.to_string(10).c_str(), b3.to_string(16).c_str());
}

int main(int argc, char const *argv[])
{
	test_bignum();
	test_uint8();
	test_uint16();
	test_uint32();
	test_uint64();
}
