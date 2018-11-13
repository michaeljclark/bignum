// See LICENSE.md

#include <cassert>
#include <iostream>

#include "bitvector.h"

void test_uint8()
{
	bitvector b1 = Uint8(0);
	bitvector b2 = Uint8(0);
	bitvector b3 = Uint8(0);

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
	bitvector b1 = Uint16(0);
	bitvector b2 = Uint16(0);
	bitvector b3 = Uint16(0);

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
	bitvector b1 = Uint32(0);
	bitvector b2 = Uint32(0);
	bitvector b3 = Uint32(0);

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
	bitvector b1 = Uint64(0x0102030405060708);
	bitvector b2 = Uint64(0x8070605040302010);
	bitvector b3 = Uint64(0);

	b3 = b1 + b2;

	printf("%s\n", __func__);
	printf("b1=%s (%s)\n", b1.to_string(10).c_str(), b1.to_string(16).c_str());
	printf("b2=%s (%s)\n", b2.to_string(10).c_str(), b2.to_string(16).c_str());
	printf("b3=%s (%s)\n", b3.to_string(10).c_str(), b3.to_string(16).c_str());
}

int main(int argc, char const *argv[])
{
	test_uint8();
	test_uint16();
	test_uint32();
	test_uint64();
}
