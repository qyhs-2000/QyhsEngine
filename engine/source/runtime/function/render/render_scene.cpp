#include "render_scene.h"
#include <function/render/render_pass.h>
#include <function/render/render_entity.h>

namespace QYHS
{
	void RenderScene::updateVisibleObjectsOfMainCamera(std::shared_ptr<RenderResource> render_resource)
	{
		main_camera_visible_object_node.clear();
		for (RenderEntity & entity : m_render_entities)
		{
			if (renderEntityVisible(entity))
			{
				main_camera_visible_object_node.emplace_back();
				RenderMeshNode& node = main_camera_visible_object_node.back();
				node.model_matrix = entity.model_matrix;
				VulkanMaterial& material = render_resource->getEntityMaterial(entity);
				node.p_material = &material;
				VulkanMesh& mesh = render_resource->getEntityMesh(entity);
				node.p_mesh = &mesh;
				node.node_id = entity.m_instance_id;
			}
		}
	}

	bool RenderScene::renderEntityVisible(RenderEntity & entity)
	{
		return true;
	}

	void RenderScene::updateVisibleObject(std::shared_ptr<RenderResource> render_resource)
	{
		updateVisibleObjectsOfMainCamera(render_resource);
	}
	void RenderScene::setVisibleObjectNodesReference()
	{
		RenderPass::m_visible_render_meshes.p_main_camera_pass_visible_mesh_nodes = &main_camera_visible_object_node;
		
	}
	bool RenderScene::hasEntityInScene(GameObjectPartId& part_id)
	{
		return true;
	}

	void RenderScene::addInstancedIdToGameObjectIdMap(size_t instanced_id, size_t go_id)
	{
		m_instance_to_gameobject_map[instanced_id] = go_id;
	}

	RenderGUIDAllocator<GameObjectPartId>& RenderScene::getInstanceIdAllocator()
	{
		return m_instance_id_allocator;
	}
	RenderGUIDAllocator<MeshSourceDesc>& RenderScene::getMeshAssetIdAllocator()
	{
		return m_mesh_asset_id_allocator;
	}
	RenderGUIDAllocator<MaterialSourceDesc>& RenderScene::getMaterialAssetIdAllocator()
	{
		return m_material_asset_id_allocator;
		// TODO: insert return statement here
	}
}