#include "render_resource.h"
#include <function/render/render_entity.h>

namespace QYHS
{
	Material& RenderResource::getEntityMaterial(RenderEntity& entity)
	{
		size_t asset_id = entity.material_id;
		if (m_materials.find(asset_id) != m_materials.end())
		{
			return m_materials[asset_id];
		}
		else
		{
			throw std::runtime_error("falied to get entity material!");
		}
	}
	Mesh& RenderResource::getEntityMesh(RenderEntity& entity)
	{
		size_t asset_id = entity.mesh_id;
		if (m_meshes.find(asset_id) != m_meshes.end())
		{
			return m_meshes[asset_id];
		}
		else
		{
			throw std::runtime_error("falied to get entity mesh!");
		}
	}
	void RenderResource::uploadGameObjectRenderResource()
	{
	}
}