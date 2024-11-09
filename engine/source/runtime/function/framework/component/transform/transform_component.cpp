#include "transform_component.h"
#include <iostream>
namespace QYHS
{
	void TransformComponent::postLoadResource(std::weak_ptr<GameObject> parent_object)
	{
		Component::postLoadResource(parent_object);
		TransformComponent transform;
		m_is_dirty = true;

	}

	void TransformComponent::setPosition(Vector3 position)
	{
		m_transform.m_position = position; 
		std::cout << "set position" << std::endl;
		m_is_dirty = true; 
	}
}