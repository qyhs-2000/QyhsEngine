#pragma once
#include "function/framework/component/component.h"
#include "function/framework/component/ecs.h"
namespace qyhs
{
	REFLECTION_TYPE(HierarchyComponent)
	CLASS(HierarchyComponent:public Component,WhiteListFields)
	{
	public:
		GameObjectID parent_id;
		void serialize(Archive & archive, ecs::EntitySerializer & seri) {};
	private:
	};
}