#pragma once
#include <vector>
#include <memory>
#include <function/render/render_structure.h>
#include <function/render/game_object_desc.h>
#include <function/render/render_resource.h>
#include <function/render/render_guid_allocator.h>
namespace QYHS
{
	class Entity;
	class RenderScene
	{
	public:
		void updateVisibleObject(std::shared_ptr<RenderResource> render_resource);
		void setVisibleObjectNodesReference();
		bool hasEntityInScene(GameObjectPartId & part_id);
		void addInstancedIdToGameObjectIdMap(size_t instanced_id,size_t go_id);
		RenderGUIDAllocator<GameObjectPartId>& getInstanceIdAllocator();
		RenderGUIDAllocator<MeshSourceDesc>& getMeshAssetIdAllocator();
		RenderGUIDAllocator<MaterialSourceDesc>& getMaterialAssetIdAllocator();
		std::vector<RenderEntity>				m_render_entities;

	private:
		void updateVisibleObjectsOfMainCamera(std::shared_ptr<RenderResource> render_resource);
		bool renderEntityVisible(RenderEntity& entity);
	private:
		std::vector<RenderMeshNode>				main_camera_visible_object_node;
		RenderGUIDAllocator<GameObjectPartId>	m_instance_id_allocator;		//allocate uind for every instance in the scene
		RenderGUIDAllocator<MeshSourceDesc>     m_mesh_asset_id_allocator;
		RenderGUIDAllocator<MaterialSourceDesc> m_material_asset_id_allocator;
		std::unordered_map<size_t, GameObjectID> m_instance_to_gameobject_map;
	};
}