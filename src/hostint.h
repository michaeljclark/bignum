// See LICENSE.md

#pragma once

#include <type_traits>

/*!
 * integer template deduces primitive type based on bits parameter
 */
template<size_t _size, bool _signed = true>
struct hostint {
	typedef typename std::conditional<_size == 8,
		typename std::conditional<_signed, int8_t, uint8_t>::type,
		typename std::conditional<_size == 16,
			typename std::conditional<_signed, int16_t, uint16_t>::type,
			typename std::conditional<_size == 32,
				typename std::conditional<_signed, int32_t, uint32_t>::type,
				typename std::conditional<_size == 64,
					typename std::conditional<_signed, int64_t, uint64_t>::type,
					void
				>::type
			>::type
		>::type
	>::type type;
};

/*!
 * static type information container for log2 of integer bit widths
 */
struct int8_traits { enum { log2 = 3 }; };
struct int16_traits { enum { log2 = 4 }; };
struct int32_traits { enum { log2 = 5 }; };
struct int64_traits { enum { log2 = 6 }; };
struct int128_traits { enum { log2 = 7 }; };
struct int256_traits { enum { log2 = 8 }; };

template <size_t bits, bool is_signed, size_t limb_bits_param>
struct wideint;

/*!
 * integer template deduces primitive or wideint type based on bits parameter
 */
template<size_t _size, bool _signed = true>
struct int_t {
	typedef typename std::conditional<
		_size == 8,
		typename std::conditional<_signed, int8_t, uint8_t>::type,
		typename std::conditional<_size == 16,
			typename std::conditional<_signed, int16_t, uint16_t>::type,
			typename std::conditional<_size == 32,
				typename std::conditional<_signed, int32_t, uint32_t>::type,
				typename std::conditional<_size == 64,
					typename std::conditional<_signed, int64_t, uint64_t>::type,
					wideint<_size,_signed,64>
				>::type
			>::type
		>::type
	>::type type;
	typedef typename std::conditional<
		_size <= 8, int8_traits, typename std::conditional<
			_size <= 16, int16_traits, typename std::conditional<
				_size <= 32, int32_traits, typename std::conditional<
					_size <= 64, int64_traits, typename std::conditional<
						_size <= 128, int128_traits, typename std::conditional<
							_size <= 256, int256_traits, void
						>::type
					>::type
				>::type
			>::type
		>::type
	>::type traits;
};
