#include "render_system.h"
#include <function/render/render_pass.h>
#include "function/global/global_context.h"

namespace QYHS
{

	void RenderSystem::tick()
	{

		processSwapData();

		m_render_scene->updateVisibleObject(m_render_resource);

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
					GameObjectPartDesc& part = gobject.getGameObjectParts()[part_index];

					GameObjectPartId part_id = { gobject.id,part_index };
					
					bool is_entity_in_scene = m_render_scene->getInstanceIdAllocator().hasElement(part_id);
					RenderEntity render_entity;
					render_entity.m_instance_id = m_render_scene->getInstanceIdAllocator().allocateGUId(part_id);
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

		m_render_resource = std::shared_ptr<RenderResource>();
		m_render_resource->uploadGameObjectRenderResource();

		m_render_scene = std::make_shared<RenderScene>();
		m_render_scene->setVisibleObjectNodesReference();
	}

	void RenderSystem::swapLogicRenderData()
	{
		m_swap_context.swapLogicRenderData();
		
	}

	
	
}
