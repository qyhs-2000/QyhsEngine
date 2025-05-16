#pragma once

#include "core/meta/reflection/reflection.h"

namespace qyhs
{
	REFLECTION_TYPE(MaterialRes)
		CLASS(MaterialRes, Fields)
	{
	public:
		std::string m_base_colour_texture_file;
	private:
	};
}