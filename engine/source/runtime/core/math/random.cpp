#include "random.h"

namespace qyhs::random
{
	static RandomNumberGenerator random_number_generator(time(nullptr));
	uint32_t getRandom(uint32_t min_value, uint32_t max_value)
	{
		return random_number_generator.next_uint(min_value, max_value);
	}

	uint32_t getRandom(uint32_t max_value)
	{
		return getRandom(0, max_value);
	}
}