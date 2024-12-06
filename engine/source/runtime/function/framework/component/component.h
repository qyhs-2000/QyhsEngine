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
		REFLECTION_BODY(Component)
	public:
		Component() = default;
		virtual void tick(double delta_time) {};
		virtual void postLoadResource(std::weak_ptr<GameObject> parent_object) { m_owner = parent_object; }
		void setParent(std::weak_ptr<GameObject> parent_object){m_owner = parent_object;}
	protected:
		std::weak_ptr<GameObject>	m_owner;		//pointer to component owner
	};
}