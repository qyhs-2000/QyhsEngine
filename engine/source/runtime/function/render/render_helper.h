#pragma once
#include <cstdint>
namespace qyhs
{
	static inline uint32_t roundUp(uint32_t value,uint32_t alignment)
	{
		uint32_t temp = value + alignment - static_cast<uint32_t>(1);
		return (temp - temp % alignment);
	}
}