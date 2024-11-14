#include "camera_component.h"
#include "function/global/global_context.h"
#include "function/render/render_system.h"
#include "function/framework/world/world_manager.h"

#include <core/base/macro.h>
namespace QYHS
{
	void CameraComponent::tick(double delta_time)
	{
		Component::tick(delta_time);
		if (!m_parent_object.lock())
			return;

		std::shared_ptr<Level> current_level = g_runtime_global_context.m_world_manager->getCurrentActiveLevel().lock();
		std::shared_ptr<Character> current_character = current_level->getCurrentActiveCharacter().lock();
		if (current_character == nullptr)
			return;

		if (current_character->getObjectID() != m_parent_object.lock()->getObjectId())
			return;
		switch (m_camera_mode)
		{
		case CameraMode::ThirdPersonMode:
			tickThirdPersonCamera(delta_time);
			break;
		default:
			break;
		}
	}
	void CameraComponent::tickThirdPersonCamera(double delta_time)
	{
		std::shared_ptr<Level> m_current_level = g_runtime_global_context.m_world_manager->getCurrentActiveLevel().lock();
		if (!m_current_level) return;
		std::shared_ptr<Character> character = m_current_level->getCurrentActiveCharacter().lock();
		if (!character) return;
		Quaternion q_yaw, q_pitch;
		q_yaw.fromAngleAxis(-g_runtime_global_context.m_input_system->m_cursor_delta_yaw, Vector3::UNIT_Z);
		q_pitch.fromAngleAxis(g_runtime_global_context.m_input_system->m_cursor_delta_pitch, Vector3::UNIT_X);
		auto param = static_cast<ThirdPersonCameraParameter*>(m_camera_res.m_parameter);
		param->m_cursor_pitch = q_pitch * param->m_cursor_pitch;
		param->m_cursor_yaw = q_yaw * param->m_cursor_yaw;
		Vector3 offset = Vector3(0, param->m_horizontal_offset, param->m_vertical_offset);
		Vector3 center_pos = character->getPosition() + Vector3::UNIT_Z * param->m_vertical_offset;
		m_position = character->getPosition() + character->getRotation() * param->m_cursor_pitch * offset;
		m_forward = center_pos - m_position;
		m_up = character->getRotation() * param->m_cursor_pitch * Vector3::UNIT_Z;
		m_left = m_up.crossProduct(m_forward);

		character->setRotation(q_yaw * character->getRotation());
		CameraSwapData camera_swap_data;
		camera_swap_data.m_view_matrix = Math::makeLookAtMatrix(m_position, m_position + m_forward, m_up);
		camera_swap_data.m_camera_type = RenderCameraType::Motor;
		RenderSwapContext& swap_context = g_runtime_global_context.m_render_system->getSwapContext();
		SwapData& logical_swap_data = swap_context.getLogicSwapData();
		logical_swap_data.m_camera_swap_data = camera_swap_data;
	}

	void CameraComponent::postLoadResource(std::weak_ptr<GameObject> parent_object)
	{
		Component::postLoadResource(parent_object);
		const std::string camera_type_name = m_camera_res.m_parameter.getTypeName();
		if (camera_type_name == "FirstPersonCameraParameter")
		{
			m_camera_mode = CameraMode::FirstPersonMode;
		}
		else if (camera_type_name == "ThirdPersonCameraParameter")
		{
			m_camera_mode = CameraMode::ThirdPersonMode;
		}
		else
		{
			LOG_ERROR("invalid camera type");
		}
		RenderSwapContext& swap_context = g_runtime_global_context.m_render_system->getSwapContext();
		CameraSwapData     camera_swap_data;
		camera_swap_data.m_fov_x = m_camera_res.m_parameter->m_fov;
		swap_context.getLogicSwapData().m_camera_swap_data = camera_swap_data;
	}
}

