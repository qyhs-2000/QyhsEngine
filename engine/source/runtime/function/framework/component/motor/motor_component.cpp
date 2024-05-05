#include "motor_component.h"

namespace QYHS
{
	void MotorComponent::postLoadResource(std::weak_ptr<GameObject> parent_object)
	{
		Component::postLoadResource(parent_object);
		MotorComponent transform;
		m_is_dirty = true;

	}
}