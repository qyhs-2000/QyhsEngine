#pragma once
#include "function/framework/component/component.h"
namespace qyhs
{
	REFLECTION_TYPE(HierarchyComponent)
	CLASS(HierarchyComponent:public Component,WhiteListFields)
	{
	public:
		GameObjectID parent_id;
	private:
	};
}