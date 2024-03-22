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
		RenderGUIDAllocator<GameObjectPartId>& getInstanceIdAllocator();
		std::vector<RenderEntity>				m_render_entities;

	private:
		void updateVisibleObjectsOfMainCamera(std::shared_ptr<RenderResource> render_resource);
		bool renderEntityVisible(RenderEntity& entity);
	private:
		std::vector<RenderMeshNode>				main_camera_visible_object_node;
		RenderGUIDAllocator<GameObjectPartId>	m_instance_id_allocator;		//allocate uind for every instance in the scene
		
	};
}