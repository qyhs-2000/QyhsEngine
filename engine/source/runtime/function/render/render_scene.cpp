#include "render_scene.h"
#include <function/render/render_pass.h>
#include <function/render/render_entity.h>

namespace QYHS
{
	void RenderScene::updateVisibleObjectsOfMainCamera(std::shared_ptr<RenderResource> render_resource)
	{
		main_camera_visible_object_nodes.clear();
		for (RenderEntity & entity : m_render_entities)
		{
			if (renderEntityVisible(entity))
			{
				main_camera_visible_object_nodes.emplace_back();
				RenderMeshNode& node = main_camera_visible_object_nodes.back();
				node.model_matrix = entity.model_matrix;
				VulkanMaterial& material = render_resource->getEntityMaterial(entity);
				node.p_material = &material;
				VulkanMesh& mesh = render_resource->getEntityMesh(entity);
				node.p_mesh = &mesh;
				node.node_id = entity.m_instance_id;
				assert(entity.m_joint_matrices.size() <= s_mesh_vertex_blending_max_joint_count);
				if (!entity.m_joint_matrices.empty())
				{
					node.joint_count = entity.m_joint_matrices.size();
					node.joint_matrices = entity.m_joint_matrices.data();
					node.enable_vertex_blending = entity.enable_vertex_blending;
				}
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
		updateVisibleObjectAxisRenderNode(render_resource);
	}
	void RenderScene::setVisibleObjectNodesReference()
	{
		RenderPass::m_visible_render_meshes.p_main_camera_pass_visible_mesh_nodes = &main_camera_visible_object_nodes;
		RenderPass::m_visible_render_meshes.p_axis_node = &m_axis_render_node;
		
	}
	bool RenderScene::hasEntityInScene(GameObjectPartId& part_id)
	{
		return true;
	}

	void RenderScene::addInstancedIdToGameObjectIdMap(size_t instanced_id, size_t go_id)
	{
		m_instance_to_gameobject_map[instanced_id] = go_id;
	}

	void RenderScene::updateVisibleObjectAxisRenderNode(std::shared_ptr<RenderResource> render_resource)
	{
		if (m_axis_entity.has_value())
		{
			m_axis_render_node.model_matrix = m_axis_entity.value().model_matrix;
			m_axis_render_node.node_id = m_axis_entity.value().m_instance_id;
			//m_axis_render_node.p_material = &render_resource->getEntityMaterial(m_axis_entity.value());
			m_axis_render_node.p_mesh = &render_resource->getEntityMesh(m_axis_entity.value());
		}
	}

	void RenderScene::setVisibleAxisEntity(std::optional<RenderEntity> axis_entity)
	{
		if (axis_entity.has_value())
		{
			m_axis_entity = axis_entity;
		}
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

	size_t RenderScene::getGameObjectIDByInstance(size_t instance_id)
	{
		if(m_instance_to_gameobject_map.find(instance_id) ==m_instance_to_gameobject_map.end())
		{
			return 0;
		}
		else
		{
			return m_instance_to_gameobject_map[instance_id];
		}
	}

}