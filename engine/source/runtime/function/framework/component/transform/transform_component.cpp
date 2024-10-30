#include "transform_component.h"

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
		m_is_dirty = true; 
	}
}