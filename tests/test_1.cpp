// See LICENSE.md

#include <cassert>

#include "bitvector.h"

int main(int argc, char const *argv[])
{
	/* test empty constructor */
	bitvector b1;
	assert(b1.num_limbs() == 1);
	assert(b1.limb_at(0) == 0);

	/* test integral constructor */
	bitvector b2(1);
	assert(b2.num_limbs() == 1);
	assert(b2.limb_at(0) == 1);

	/* test array constructor */
	bitvector b3{1, 1};
	assert(b3.num_limbs() == 2);
	assert(b3.limb_at(0) == 1);
	assert(b3.limb_at(1) == 1);

	/* test left shift */
	bitvector b4 = 1;
	b4 = b4 << 1;
	assert(b4.num_limbs() == 1);
	assert(b4.limb_at(0) == 2);

	/* test left shift carry overflow */
	bitvector b5(std::numeric_limits<bitvector::limb_t>::max());
	b5 = b5 << 10;
	assert(b5.num_limbs() == 2);
	assert(b5.limb_at(0) == std::numeric_limits<bitvector::limb_t>::max() - 1023);
	assert(b5.limb_at(1) == 1023);

	/* test big left shift */
	bitvector b6 = bitvector{1, 1} << 2;
	assert(b6.num_limbs() == 2);
	assert(b6.limb_at(0) == 4);
	assert(b6.limb_at(1) == 4);

	/* test right shift carry under */
	assert((bitvector{0,1} >> 1) == bitvector(1ULL << (bitvector::limb_bits - 1)));

	/* test right shift carry under limb contraction */
	assert((bitvector{0,1} >> 1).num_limbs() == 1);

	/* test big right shift */
	assert((bitvector{0,1} >> 17) == bitvector(1ULL << (bitvector::limb_bits - 17)));

	/* test logical and */
	bitvector b7 = bitvector{0b101, 0b101} & bitvector{0b100, 0};
	assert(b7.num_limbs() == 1);
	assert(b7.limb_at(0) == 0b100);

	/* test logical or */
	bitvector b8 = bitvector{0b101, 0b101} | bitvector{0b100, 0};
	assert(b8.num_limbs() == 2);
	assert(b8.limb_at(0) == 0b101);
	assert(b8.limb_at(1) == 0b101);

	/* test equals */
	assert((bitvector{2,3} == bitvector{2,3,0}));
	assert((bitvector{2,3,0} == bitvector{2,3}));
	assert((bitvector{1,2,3} == bitvector{1,2,3}));
	assert(!(bitvector{1,2,3} == bitvector{3,2,3}));
	assert(!(bitvector{1,2,3} == 0));
	assert(!(bitvector{1,2,3} == 2147483648));

	/* test not equals */
	assert(!(bitvector{2,3} != bitvector{2,3,0}));
	assert(!(bitvector{2,3,0} != bitvector{2,3}));
	assert(!(bitvector{1,2,3} != bitvector{1,2,3}));
	assert((bitvector{1,2,3} != bitvector{3,2,3}));
	assert((bitvector{1,2,3} != 0));
	assert((bitvector{1,2,3} != 2147483648));

	/* test less than */
	assert(!(bitvector{3,2,3} < bitvector{1,2,3}));
	assert(!(bitvector{1,2,3} < bitvector{1,2,3}));
	assert((bitvector{1,2,3} < bitvector{3,2,3}));
	assert((bitvector{1,2,3} < bitvector{3,2,3,1}));
	assert(!(bitvector{3,2,3,1} < bitvector{1,2,3}));
	assert(!(bitvector{3,2,3,1} < 0));
	assert(!(bitvector{3,2,3,1} < 2147483648));

	/* test less than equals */
	assert(!(bitvector{3,2,3} <= bitvector{1,2,3}));
	assert((bitvector{1,2,3} <= bitvector{1,2,3}));
	assert((bitvector{1,2,3} <= bitvector{3,2,3}));
	assert((bitvector{1,2,3} <= bitvector{3,2,3,1}));
	assert(!(bitvector{3,2,3,1} <= bitvector{1,2,3}));
	assert(!(bitvector{3,2,3,1} <= 0));
	assert(!(bitvector{3,2,3,1} <= 2147483648));

	/* test greater than */
	assert((bitvector{3,2,3} > bitvector{1,2,3}));
	assert(!(bitvector{1,2,3} > bitvector{1,2,3}));
	assert(!(bitvector{1,2,3} > bitvector{3,2,3}));
	assert(!(bitvector{1,2,3} > bitvector{3,2,3,1}));
	assert((bitvector{3,2,3,1} > bitvector{1,2,3}));
	assert((bitvector{3,2,3,1} > 0));
	assert((bitvector{3,2,3,1} > 2147483648));

	/* test greater than equals */
	assert((bitvector{3,2,3} >= bitvector{1,2,3}));
	assert((bitvector{1,2,3} >= bitvector{1,2,3}));
	assert(!(bitvector{1,2,3} >= bitvector{3,2,3}));
	assert(!(bitvector{1,2,3} >= bitvector{3,2,3,1}));
	assert((bitvector{3,2,3,1} >= bitvector{1,2,3}));
	assert((bitvector{3,2,3,1} >= 0));
	assert((bitvector{3,2,3,1} >= 2147483648));

	/* test not */
	assert(!bitvector(0));
	assert(!!bitvector(1));

	/* test addition */
	assert((bitvector{1,1,1} + bitvector{1,1,1} == bitvector{2,2,2}));

	/* test addition with carry */
	bitvector b9{0,std::numeric_limits<bitvector::limb_t>::max()};
	bitvector b10{0,std::numeric_limits<bitvector::limb_t>::max()-1,1};
	assert(b9 + b9 == b10);

	/* test multiplication */
	bitvector b12 = bitvector(2147483648) * bitvector(2147483648);
	assert(b12.num_limbs() == 2);
	assert(b12.limb_at(0) == 0);
	assert(b12.limb_at(1) == 1073741824);
	bitvector b13 = b12 * b12;
	assert(b13.num_limbs() == 4);
	assert(b13.limb_at(0) == 0);
	assert(b13.limb_at(1) == 0);
	assert(b13.limb_at(2) == 0);
	assert(b13.limb_at(3) == 268435456);
	bitvector b14 = bitvector(2147483647) * bitvector(2147483647);
	assert(b14.num_limbs() == 2);
	assert(b14.limb_at(0) == 1);
	assert(b14.limb_at(1) == 1073741823);
	bitvector b15 = b14 * b14;
	assert(b15.num_limbs() == 4);
	assert(b15.limb_at(0) == 1);
	assert(b15.limb_at(1) == 2147483646);
	assert(b15.limb_at(2) == 2147483649);
	assert(b15.limb_at(3) == 268435455);

	/* test subtraction */
	assert((bitvector{3,3,3} - bitvector{1,1,1} == bitvector{2,2,2}));

	/* test subtraction with borrow */
	bitvector b17{1,1};
	bitvector b18{std::numeric_limits<bitvector::limb_t>::max()};
	assert(b17-b18 == 2);

	/* test division */
	bitvector b19 = b15 / b14;
	assert(b19.num_limbs() == 2);
	assert(b19.limb_at(0) == 1);
	assert(b19.limb_at(1) == 1073741823);
	assert(b19.to_string() == "4611686014132420609");

	/* test set and test bit */
	bitvector b20;
	b20.set_bit(64);
	assert(b20.test_bit(64) == 1);
	assert(b20.test_bit(63) == 0);
	b20 = b20 - 1;
	assert(b20.test_bit(64) == 0);
	assert(b20.test_bit(63) == 1);

	/* decimal string formatting */
	assert((bitvector(9) * bitvector(9)).to_string() == "81");
	assert((bitvector(255) * bitvector(255)).to_string() == "65025");
	assert((bitvector(65535) * bitvector(65535)).to_string() == "4294836225");
	assert((bitvector(2147483648) * bitvector(2147483648)).to_string() == "4611686018427387904");

	/* binary string formatting */
	assert((bitvector{0b101}).to_string(2) == "0b101");
	assert((bitvector{0b111100001111}).to_string(2) == "0b111100001111");
	assert((bitvector{0xff00ff,0xff}).to_string(2) == "0b1111111100000000111111110000000011111111");

	/* hex string formatting */
	assert((bitvector{0x1}).to_string(16) == "0x1");
	assert((bitvector{0x7f}).to_string(16) == "0x7f");
	assert((bitvector{0x3ff}).to_string(16) == "0x3ff");
	assert((bitvector{0xffffffff,1}).to_string(16) == "0x1ffffffff");
	assert((bitvector{0xffffffff,0x80}).to_string(16) == "0x80ffffffff");
	assert((bitvector{0xffffffff,0x400}).to_string(16) == "0x400ffffffff");
	assert((bitvector{0x80000000,0x80000000}).to_string(16) == "0x8000000080000000");

	/* pow */
	assert(bitvector(71).pow(0) == 1);
	assert(bitvector(71).pow(1) == 71);
	assert(bitvector(71).pow(17).to_string() == "29606831241262271996845213307591");

	/* from string */
	assert(bitvector("71").to_string() == "71");
	assert(bitvector("29606831241262271996845213307591").to_string() == "29606831241262271996845213307591");
	assert(bitvector("0xdeadbeef").to_string(16) == "0xdeadbeef");
	assert(bitvector("0xdeadbeef00ff00ff00ff00ff").to_string(16) == "0xdeadbeef00ff00ff00ff00ff");
	assert(bitvector("0b101").to_string(2) == "0b101");
	assert(bitvector("0b11110000111100001111000011110000").to_string(2) == "0b11110000111100001111000011110000");
	assert(bitvector("3249094308290873429032409832424398902348094329803249083249089802349809430822903").to_string()
		== "3249094308290873429032409832424398902348094329803249083249089802349809430822903");

	/* fixed width tests */
	assert(bitvector(0xffffffff, is_unsigned(), 32) + 2 == 1);
	assert(bitvector(0xffffffff, is_unsigned(), 31) == 0x7fffffff);
	assert(bitvector(0x7fffffff, is_unsigned(), 31) + 2 == 1);
	assert(bitvector("0xffffffff", is_unsigned(), 31) == 0x7fffffff);
	assert(bitvector(100000) * bitvector(100000) == (bitvector{0x540be400, 0x2}));
	assert(bitvector(100000, is_unsigned(), 34) * bitvector(100000, is_unsigned(), 34) == (bitvector{0x540be400, 0x2}));
	assert(bitvector(100000, is_unsigned(), 33) * bitvector(100000, is_unsigned(), 33) == bitvector(0x540be400));
	assert(bitvector(100000, is_unsigned(), 32) * bitvector(100000, is_unsigned(), 32) == bitvector(0x540be400));
	assert(bitvector(100000, is_unsigned(), 20) * bitvector(100000, is_unsigned(), 20) == bitvector(0xbe400));
	assert(-bitvector(1, is_unsigned(), 32) == bitvector(-1, is_unsigned(), 32));
	assert(-bitvector(1, is_unsigned(), 64) == bitvector({0xffffffff, 0xffffffff}, is_unsigned(), 64));
	assert(-bitvector(1, is_unsigned(), 65) == bitvector({0xffffffff, 0xffffffff,1}, is_unsigned(), 65));
	assert(bitvector({0xffffffff, 0x7fffffff,1}, is_unsigned(), 65) >> 1 == bitvector({0xffffffff, 0xbfffffff}, is_unsigned(), 65));
	assert(-bitvector(1, is_signed(), 65) >> 1 == bitvector({0xffffffff, 0xffffffff,1}, is_signed(), 65));
	assert(bitvector({0xffffffff, 0x7fffffff,1}, is_signed(), 65) >> 1 == bitvector({0xffffffff, 0xbfffffff,1}, is_signed(), 65));
	assert(bitvector({0xffffffff, 0xffffffff}, is_signed(), 65) >> 1 == bitvector({0xffffffff, 0x7fffffff}, is_signed(), 65));
	assert(-bitvector(1, is_unsigned(), 65) >> 1 == bitvector({0xffffffff, 0xffffffff}, is_unsigned(), 65));
	assert(-bitvector(1, is_unsigned(), 65) >> 2 == bitvector({0xffffffff, 0x7fffffff}, is_unsigned(), 65));
	assert(-bitvector(1, is_unsigned(), 65) << 1 == bitvector({0xfffffffe, 0xffffffff,1}, is_unsigned(), 65));
	assert(-bitvector(1, is_unsigned(), 65) << 2 == bitvector({0xfffffffc, 0xffffffff,1}, is_unsigned(), 65));

	/* unsigned comparison */
	assert(bitvector(-1, is_unsigned(), 32) > bitvector(1, is_unsigned(), 32));
	assert(bitvector(0, is_unsigned(), 32) < bitvector(1, is_unsigned(), 32));
	assert(bitvector(1, is_unsigned(), 32) > bitvector(0, is_unsigned(), 32));
	assert(bitvector(1, is_unsigned(), 32) < bitvector(-1, is_unsigned(), 32));

	/* signed comparison */
	assert(bitvector(-1, is_signed(), 32) < bitvector(1, is_signed(), 32));
	assert(bitvector(0, is_signed(), 32) < bitvector(1, is_signed(), 32));
	assert(bitvector(1, is_signed(), 32) > bitvector(0, is_signed(), 32));
	assert(bitvector(1, is_signed(), 32) > bitvector(-1, is_signed(), 32));

	assert(bitvector(0).num_bits() == 0);
	assert(bitvector(1).num_bits() == 1);
	assert(bitvector(2).num_bits() == 2);
	assert(bitvector(3).num_bits() == 2);
	assert(bitvector(4).num_bits() == 3);
	assert(bitvector(5).num_bits() == 3);
	assert(bitvector(6).num_bits() == 3);
	assert(bitvector(7).num_bits() == 3);

	return 0;
}
