#pragma once

#include "core/math/vector3.h"
#include "resource/common/object.h"
namespace QYHS
{
	REFLECTION_TYPE(LevelResource)
		CLASS(LevelResource, Fields)
	{
	public:
		Vector3 m_gravity = { 0.f,0.f,-9.8f };
		std::string m_character_name;
		std::string m_test_name;
		std::string m_test_name2;
		std::vector<ObjectInstanceResource> m_objects;
	private:
	};
}