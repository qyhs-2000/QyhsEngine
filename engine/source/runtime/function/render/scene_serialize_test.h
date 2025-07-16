#pragma once
#include "function/framework/component/component.h"

namespace qyhs
{
	REFLECTION_TYPE(AnimComponentTest)
		CLASS(AnimComponentTest:public Component, WhiteListFields)
	{
		REFLECTION_BODY(AnimComponentTest)
	public:
		META(Enable)
			int test_int;
	};
}