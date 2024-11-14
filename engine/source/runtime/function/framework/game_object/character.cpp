#include "character.h"
#include "function/framework/component/transform/transform_component.h"
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
}