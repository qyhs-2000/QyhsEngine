#pragma once
#include <vector>
#include <memory>
#include <optional>
#include <function/render/render_structure.h>
#include <function/render/game_object_desc.h>
#include <function/render/render_resource.h>
#include <function/render/render_guid_allocator.h>
namespace QYHS
{
	typedef size_t Entity;
	class RenderScene
	{
	public:
		void updateVisibleObject(std::shared_ptr<RenderResource> render_resource);
		void setVisibleObjectNodesReference();
		bool hasEntityInScene(GameObjectPartId & part_id);
		void addInstancedIdToGameObjectIdMap(size_t instanced_id,size_t go_id);
		void updateVisibleObjectAxisRenderNode(std::shared_ptr<RenderResource> render_resource);
		void setVisibleAxisEntity(std::optional<RenderEntity> axis_entity);
		size_t getGameObjectIDByInstance(size_t instance_id);
		RenderGUIDAllocator<GameObjectPartId>& getInstanceIdAllocator();
		RenderGUIDAllocator<MeshSourceDesc>& getMeshAssetIdAllocator();
		RenderGUIDAllocator<MaterialSourceDesc>& getMaterialAssetIdAllocator();
		std::vector<RenderEntity>				m_render_entities;

	private:
		void updateVisibleObjectsOfMainCamera(std::shared_ptr<RenderResource> render_resource);
		bool renderEntityVisible(RenderEntity& entity);
	private:
		std::vector<RenderMeshNode>				main_camera_visible_object_nodes;
		RenderMeshNode							m_axis_render_node;
		RenderGUIDAllocator<GameObjectPartId>	m_instance_id_allocator;		//allocate uind for every instance in the scene
		RenderGUIDAllocator<MeshSourceDesc>     m_mesh_asset_id_allocator;
		RenderGUIDAllocator<MaterialSourceDesc> m_material_asset_id_allocator;
		std::unordered_map<size_t, GameObjectID> m_instance_to_gameobject_map;
		std::optional<RenderEntity> m_axis_entity;
	};
}