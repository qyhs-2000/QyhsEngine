#pragma once
#include "core/math/matrix4.h"
#include <vector>
namespace qyhs
{
	class RenderEntity
	{
	public:
		Matrix4x4 model_matrix;
		std::vector<Matrix4x4> m_joint_matrices;
		bool enable_vertex_blending{ false };
		size_t material_asset_id;
		size_t mesh_asset_id;
		size_t m_instance_id;
	};
}