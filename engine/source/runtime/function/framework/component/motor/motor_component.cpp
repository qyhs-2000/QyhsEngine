#include "motor_component.h"
#include "function/framework/component/transform/transform_component.h"
#include "function/global/global_context.h"
namespace qyhs
{
	void MotorComponent::postLoadResource(std::weak_ptr<GameObject> parent_object)
	{
		Component::postLoadResource(parent_object);
		MotorComponent transform;
		m_is_dirty = true;

	}

	void MotorComponent::tick(double delta_time)
	{
		Component::tick(delta_time);
		TransformComponent* transform_component = m_owner.lock()->TryGetComponent(TransformComponent);
		if (!transform_component) return;
		Vector3 m_current_position = transform_component->getPosition();
		m_target_position = m_current_position;
		 
		calculateTargetMoveDirection(transform_component->getRotation());
		calculateTargetPosition(m_current_position,delta_time);
		transform_component->setPosition(m_target_position);
	}

	void MotorComponent::calculateTargetPosition(Vector3 m_current_position,double delta_time)
	{		
		m_target_position = m_current_position + m_diresed_horizontal_direction * m_motor_res.move_speed * delta_time + m_diresed_vertical_direction;
		m_is_moving = (m_target_position - m_current_position).squaredLength() > 0.f;
	}

	void MotorComponent::calculateTargetMoveDirection(const Quaternion & object_rotation)
	{
		Vector3 forward_direction = object_rotation * Vector3::UNIT_Y;
		forward_direction *= -1;
		Vector3 left_direction = object_rotation * Vector3::UNIT_X;
		m_diresed_horizontal_direction = Vector3::ZERO;
		unsigned int command = g_runtime_global_context.m_input_system->getGameCommand();
		if (command > 0)
		{
			m_diresed_horizontal_direction = Vector3::ZERO;
		}
		if (command & (unsigned int)GameCommand::foward)
		{
			m_diresed_horizontal_direction += forward_direction;
		}
		if (command & (unsigned int)GameCommand::back)
		{
			m_diresed_horizontal_direction -= forward_direction;
		}
		if (command & (unsigned int)GameCommand::left)
		{
			m_diresed_horizontal_direction += left_direction;
		}
		if (command & (unsigned int)GameCommand::right)
		{
			m_diresed_horizontal_direction -= left_direction;
		}
		
	}
}


