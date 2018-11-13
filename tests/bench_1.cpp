// See LICENSE.md

#include <cstdio>
#include <chrono>

#include <bitvector.h>
#include <gmp.h>

void test_bit(int a, int b)
{
	bitvector c = a;
	auto t1 = std::chrono::system_clock::now();
	bitvector d = c.pow(b);
	auto t2 = std::chrono::system_clock::now();
	std::string s = d.to_string(10);
	auto t3 = std::chrono::system_clock::now();

	int tp = std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();
	int ts = std::chrono::duration_cast<std::chrono::microseconds>(t3-t2).count();

	printf("bit: pow=%12d(ms), to_string=%12d(ms) digits=%ld\n", tp, ts, s.size());
}

void test_gmp(int a, int b)
{
	mpz_t result, base;

	mpz_init(base);
	mpz_init(result);

	mpz_set_ui(base, a);
	auto t1 = std::chrono::system_clock::now();
	mpz_pow_ui(result, base, b);
	auto t2 = std::chrono::system_clock::now();
	const char *s = mpz_get_str(NULL, 10, result);
	auto t3 = std::chrono::system_clock::now();

	int tp = std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();
	int ts = std::chrono::duration_cast<std::chrono::microseconds>(t3-t2).count();

	printf("gmp: pow=%12d(ms), to_string=%12d(ms) digits=%ld\n", tp, ts, strlen(s));

	mpz_clear(base);
	mpz_clear(result);
}

int main(int argc, char const *argv[])
{
	test_bit(71, 717);
	test_gmp(71, 717);
	test_bit(71, 7117);
	test_gmp(71, 7117);
	test_bit(71, 71117);
	test_gmp(71, 71117);
}
