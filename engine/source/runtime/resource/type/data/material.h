#pragma once

#include "core/meta/reflection/reflection.h"

namespace QYHS
{
	REFLECTION_TYPE(MaterialRes)
		CLASS(MaterialRes, Fields)
	{
	public:
		std::string m_base_colour_texture_file;
	private:
	};
}