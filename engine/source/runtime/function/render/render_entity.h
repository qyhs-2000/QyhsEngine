#pragma once
#include "core/math/matrix4.h"
namespace QYHS
{
	class RenderEntity
	{
	public:
		Matrix4x4 model_matrix;
		size_t material_asset_id;
		size_t mesh_asset_id;
		size_t m_instance_id;
	};
}