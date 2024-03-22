#pragma once

namespace QYHS
{
	class RenderEntity
	{
	public:
		glm::mat4 model_matrix;
		size_t material_id;
		size_t mesh_id;
		size_t m_instance_id;
	};
}