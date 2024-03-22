#include "game_object.h"

namespace QYHS
{
	void GameObject::tick(double delta_time)
	{
		for ( Component& component : m_components)
		{
			component.tick(delta_time);
		}
	}
	void GameObject::load()
	{
	}
}