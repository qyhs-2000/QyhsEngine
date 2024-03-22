#pragma once
#include "render_structure.h"
#include "render_entity.h"
#include <unordered_map>
#include <stdexcept>
namespace QYHS
{
	class RenderResource
	{
	public:
		Material& getEntityMaterial(RenderEntity& entity);
		Mesh& getEntityMesh(RenderEntity& entity);
		void uploadGameObjectRenderResource();
	private:
		std::unordered_map<size_t, Material> m_materials;
		std::unordered_map<size_t, Mesh>		 m_meshes;
	};
}