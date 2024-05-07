#include "render_system.h"
#include <function/render/render_pass.h>
#include "function/global/global_context.h"
#include <stack>
#include "render_type.h"
#include "passes/main_camera_render_pass.h"
//#include "function/framework/component/transform/transform_component.h"
#include "function/framework/component/motor/motor_component.h"
namespace QYHS
{

	void RenderSystem::tick()
	{
		MotorComponent motor;
		//TransformComponent transform;
		processSwapData();

		m_render_scene->updateVisibleObject(m_render_resource);
		m_render_resource->updatePerframeBuffer(m_render_camera);
		m_render_pipeline->render();

	}

	void RenderSystem::processRenderEntity()
	{
		m_render_scene->m_render_entities.resize(0);
		RenderEntity entity;
		
		
		m_render_scene->m_render_entities.push_back(entity);
	}

	void RenderSystem::processSwapData()
	{
		SwapData& swap_data = m_swap_context.getRenderSwapData();
		//update game object if needed
		if (swap_data.m_game_object_resource.has_value())
		{
			while (!swap_data.m_game_object_resource->isEmpty())
			{
				GameObjectDesc gobject = swap_data.m_game_object_resource->getGameObject();

				for (size_t part_index = 0; part_index < gobject.getObjectPartSize(); ++part_index)
				{
					GameObjectPartDesc& game_object_part = gobject.getGameObjectParts()[part_index];

					GameObjectPartId part_id = { gobject.m_go_id,part_index };
					
					bool is_entity_in_scene = m_render_scene->getInstanceIdAllocator().hasElement(part_id);
					RenderEntity render_entity;
					render_entity.m_instance_id = m_render_scene->getInstanceIdAllocator().allocateGUId(part_id);
					render_entity.model_matrix = game_object_part.m_transform_desc.m_transform_matrix;
					m_render_scene->addInstancedIdToGameObjectIdMap(render_entity.m_instance_id, gobject.getId());

					//mesh
					MeshSourceDesc mesh_source = { game_object_part.m_mesh_desc.m_mesh_file };
					const bool is_mesh_loaded = m_render_scene->getMeshAssetIdAllocator().hasElement(mesh_source);
					RenderMeshData mesh_data;
					if (!is_mesh_loaded)
					{
						mesh_data = m_render_resource->loadMeshData(mesh_source);
						m_render_resource->uploadGameObjectRenderResource(m_rhi, render_entity, mesh_data);
					}

					//material
					MaterialSourceDesc material_source = { game_object_part.m_material_desc.m_base_colour_texture_file };
					const bool is_material_loaded = m_render_scene->getMaterialAssetIdAllocator().hasElement(material_source);
					RenderMaterialData material_data;
					if (!is_material_loaded)
					{
						material_data = m_render_resource->loadMaterialData(material_source);
						m_render_resource->uploadGameObjectRenderResource(m_rhi, render_entity, material_data);
					}

					if (!is_entity_in_scene)
					{
						m_render_scene->m_render_entities.push_back(render_entity);
					}
					else
					{
						for (RenderEntity& entity : m_render_scene->m_render_entities)
						{
							if (entity.m_instance_id == render_entity.m_instance_id)
							{
								entity = render_entity;
								break;
							}
						}
					}
				}
				
				//remove this game object
				swap_data.m_game_object_resource->pop();
			}


			
		}
	}

	void RenderSystem::initialize()
	{
		m_rhi = std::make_shared<VulkanRHI>();
		m_rhi->initialize();
		m_render_pipeline = std::make_shared<RenderPipeline>();
		m_render_pipeline->m_rhi = m_rhi;
		m_render_pipeline->initialize();

		m_render_resource = std::make_shared<RenderResource>();
		///m_render_resource->uploadGameObjectRenderResource();

		m_render_scene = std::make_shared<RenderScene>();
		m_render_scene->setVisibleObjectNodesReference();

		m_render_camera = std::make_shared<RenderCamera>();
		m_render_camera->setCameraPosition(glm::vec3(0.f, 0.f, 5.f));
		m_render_resource->m_material_descriptor_set_layout = &static_cast<RenderPass*>(m_render_pipeline->m_main_camera_pass.get())->m_descriptors[MainCameraRenderPass::DescriptorSetLayoutType::mesh_per_material].descriptor_set_layout;
	}

	void RenderSystem::swapLogicRenderData()
	{
		m_swap_context.swapLogicRenderData();
		
	}

	
	
}
