#pragma once
#include <vector>
#include "game_object_id_allocator.h"
#include <function/framework/component/component.h>
namespace QYHS
{
	class Component;
	class GameObject
	{
	public:
		void tick(double delta_time);
		GameObjectID getObjectId() { return m_id; }
		void load();
	private:
		std::vector<Component> m_components;
		GameObjectID	m_id;
	};
}