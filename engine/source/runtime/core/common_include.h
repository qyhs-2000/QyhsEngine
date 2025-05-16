#pragma once
#include <cstdint>
#include <type_traits>
#ifdef _WIN32
#include <intrin.h>

#endif // _WIN32
#define arraysize(a) sizeof(a)/sizeof(a[0])

inline long atomOr(volatile long* ptr, long mask)
{
	return _InterlockedOr(ptr, mask);
}

inline long atomLoad(const volatile long* ptr)
{
	return atomOr((volatile long*)ptr, 0);
}

inline long atomAdd(volatile long* ptr, long val)
{
	return _InterlockedExchangeAdd(ptr, val);
}

// Enable enum flags:
//	https://www.justsoftwaresolutions.co.uk/cplusplus/using-enum-classes-as-bitfields.html
template<typename E>
struct enable_bitmask_operators {
	static constexpr bool enable = false;
};

template<typename E>
constexpr typename std::enable_if<enable_bitmask_operators<E>::enable, E&>::type operator |=(E& lhs, E rhs)
{
	typedef typename std::underlying_type<E>::type underlying;
	lhs = static_cast<E>(static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
	return lhs;
}

template<typename E>
constexpr typename std::enable_if<enable_bitmask_operators<E>::enable, E>::type operator &(E lhs, E rhs)
{
	typedef typename std::underlying_type<E>::type underlying;
	return static_cast<E>(static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
}

template<typename E>
constexpr typename std::enable_if<enable_bitmask_operators<E>::enable, E>::type operator |(E lhs, E rhs)
{
	typedef typename std::underlying_type<E>::type underlying;
	lhs = static_cast<E>(static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
	return lhs;
}

template<typename T>
T clamp(const T& val, T a, T b)
{
	T min = a < b ? a : b;
	T max = a > b ? a : b;
	return val < min ? min : (val > max ? max : val);
}

template<typename T>
constexpr bool has_flag(T lhs, T rhs)
{
	return (lhs & rhs) == rhs;
}