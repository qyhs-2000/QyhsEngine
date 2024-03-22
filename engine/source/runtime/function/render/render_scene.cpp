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
				Material& material = render_resource->getEntityMaterial(entity);
				node.p_material = &material;
				Mesh& mesh = render_resource->getEntityMesh(entity);
				node.p_mesh = &mesh;
				
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

	RenderGUIDAllocator<GameObjectPartId>& RenderScene::getInstanceIdAllocator()
	{
		return m_instance_id_allocator;
	}
}