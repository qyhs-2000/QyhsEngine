#pragma once
#include <memory>
#include <function/framework/game_object/game_object.h>
#include "core/meta/reflection/reflection.h"
namespace QYHS
{
	class GameObject;
	REFLECTION_TYPE(Component)
	CLASS(Component,WhiteListFields)
	{
	public:
		virtual void tick(double delta_time) {};
	protected:
		std::weak_ptr<GameObject>	parent_object;		//pointer to component owner
	};
}