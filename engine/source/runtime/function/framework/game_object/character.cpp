#include "character.h"
#include "function/framework/component/transform/transform_component.h"
#include "function/framework/component/motor/motor_component.h"
namespace QYHS
{
	Character::Character(std::shared_ptr<GameObject> gobject)
	{
		setGameObject(gobject);
	}

	void Character::setGameObject(std::shared_ptr<GameObject> gobject)
	{
		m_character_object = gobject;
		if (m_character_object)
		{
			TransformComponent* transform_component = gobject->TryGetComponent(TransformComponent);
			if (transform_component)
			{
				const Transform& transform = transform_component->getTransformConst();
				m_position = transform.m_position;
				m_rotation = transform.m_rotation;
			}
		}
		else
		{
			m_position = Vector3::ZERO;
			m_rotation = Quaternion::IDENTITY;
		}
	}

	void Character::tick(double delta_time)
	{
		if (!m_character_object) return;
		MotorComponent* motor_component = m_character_object->TryGetComponent(MotorComponent);
		if (!motor_component) return;
		TransformComponent* transform_component = m_character_object->TryGetComponent(TransformComponent);
		if (!transform_component) return;
		if (m_rotation_dirty)
		{
			transform_component->setRotation(m_rotation_buffer);
			m_rotation_dirty = false;
		}
		if (motor_component->isMoving())
		{
			m_rotation_buffer = m_rotation;
			m_rotation_dirty = true;
		}
		m_position = motor_component->getTargetPosition();

	}

}