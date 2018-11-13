// See LICENSE.md

#pragma once

#include <cstddef>

#if defined (_MSC_VER)
#include <intrin.h>
#endif

/*! clz */
template <typename T>
inline size_t clz(T val)
{
	const int bits = sizeof(T) << 3;
	unsigned count = 0, found = 0;
	for (size_t i = bits - 1; i >= 0; --i) {
		count += !(found |= val & T(1)<<i ? 1 : 0);
	}
	return count;
}

/* clz specializations */
#if defined (__GNUC__)
template<> inline size_t clz(unsigned val) { return __builtin_clz(val); }
template<> inline size_t clz(unsigned long long val) { return __builtin_clzll(val); }
#elif defined (_MSC_VER)
template<> inline size_t clz(unsigned val)
{
	unsigned long count;
	return _BitScanReverse(&count, val) ^ 31;
}
template<> inline size_t clz(unsigned long long val)
{
	unsigned long count;
	return _BitScanReverse(&count, val) ^ 31;
}
#endif

/*! ctz */
template <typename T>
inline size_t ctz(T val)
{
	const int bits = sizeof(T) << 3;
	unsigned count = 0, found = 0;
	for (int i = 0; i < bits; ++i) {
		count += !(found |= val & T(1)<<i ? 1 : 0);
	}
	return count;
}

/* ctz specializations */
#if defined (__GNUC__)
template<> inline size_t ctz(unsigned val) { return __builtin_ctz(val); }
template<> inline size_t ctz(unsigned long long val) { return __builtin_ctzll(val); }
#elif defined (_MSC_VER)
template<> inline size_t ctz(unsigned val)
{
	unsigned long count;
	return _BitScanForward(&count, val);
}
template<> inline size_t ctz(unsigned long long val)
{
	unsigned long count;
	return _BitScanForward64(&count, val);
}
#endif
