#include "render_system.h"
#include <function/render/render_pass.h>
#include "function/global/global_context.h"
#include "render_type.h"
#include "passes/main_camera_render_pass.h"
#include "function/framework/component/motor/motor_component.h"

#include "resource/config_manager/config_manager.h"
#include "resource/asset_manager/asset_manager.h"
#include "resource/type/global_rendering_resource.h"
#include "render_camera.h"
#include "core/utils/utils.h"
#include "core/utils/model_importer_gltf.h"
namespace qyhs
{
	void RenderSystem::tick()
	{
		MotorComponent motor;
		//TransformComponent transform;
		processSwapData();

		m_render_scene->updateVisibleObject(m_render_resource);
		m_rhi->prepareContext();
		m_render_resource->updatePerframeBuffer(m_render_camera);

		m_render_pipeline->prepareDataForRenderPass(m_render_resource);
		m_render_pipeline->render(m_render_resource);

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
					render_entity.m_joint_matrices.resize(game_object_part.m_skeleton_animation_result.m_transforms.size());
					render_entity.mesh_asset_id = game_object_part.m_mesh_id;
					render_entity.material_asset_id = game_object_part.m_material_id;
					for (size_t i = 0; i < render_entity.m_joint_matrices.size(); ++i)
					{
						render_entity.m_joint_matrices[i] = game_object_part.m_skeleton_animation_result.m_transforms[i].m_matrix;
					}
					render_entity.enable_vertex_blending = game_object_part.m_skeleton_animation_result.m_transforms.size() > 1;
					m_render_scene->addInstancedIdToGameObjectIdMap(render_entity.m_instance_id, gobject.getId());

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

			m_swap_context.resetGameObjectRenderSwapData();

		}
		if (swap_data.m_camera_swap_data.has_value())
		{
			if (swap_data.m_camera_swap_data->m_view_matrix.has_value())
			{
				m_render_camera->setMainViewMatrix(swap_data.m_camera_swap_data->m_view_matrix.value());
			}
			if (swap_data.m_camera_swap_data->m_camera_type.has_value())
			{
				m_render_camera->setCurrentCameraType(swap_data.m_camera_swap_data->m_camera_type.value());
			}

		}
		m_swap_context.resetCameraSwapData();
	}

	void RenderSystem::postLoadMeshComponentRes(MeshComponentRes& mesh_res)
	{

	}

	size_t RenderSystem::getGObjectIDByMeshID(size_t mesh_id)
	{
		return  m_render_scene->getGameObjectIDByInstance(mesh_id);
	}

	void RenderSystem::initialize()
	{
		m_rhi = std::make_shared<VulkanRHI>();
		m_rhi->initialize();
		GlobalRenderConfig global_render_config;
		const std::string& config_url = g_runtime_global_context.m_config_manager->getGlobalRenderingResUrl();
		g_runtime_global_context.m_asset_manager->loadAsset(config_url, global_render_config);

		//render resource
		LevelResourceDesc level_resource_desc;
		level_resource_desc.m_ibl_resource_desc.m_skybox_specular_map = global_render_config.m_skybox_specular_map;
		m_render_resource = std::make_shared<RenderResource>();
		m_render_resource->uploadGlobalRenderResource(m_rhi, level_resource_desc);

		//render pipeline
		RenderPipelineInitInfo init_info;
		init_info.m_render_resource = m_render_resource;
		m_render_pipeline = std::make_shared<RenderPipeline>();
		m_render_pipeline->m_rhi = m_rhi;
		m_render_pipeline->initialize(init_info);

		//initialize render scene
		m_render_scene = std::make_shared<RenderScene>();
		m_render_scene->setVisibleObjectNodesReference();

		//initialize render camera
		CameraPose& camera_pose = global_render_config.camera_config.m_pose;
		m_render_camera = std::make_shared<RenderCamera>();
		m_render_camera->setCameraPosition(Vector3(0.f, 0.f, 70.f));
		m_render_camera->lookAt(camera_pose.m_position, camera_pose.m_target, camera_pose.m_up);
		m_render_camera->m_zfar = global_render_config.camera_config.m_z_far;
		m_render_camera->m_znear = global_render_config.camera_config.m_z_near;
		m_render_camera->setAspect(global_render_config.camera_config.m_aspect.x / global_render_config.camera_config.m_aspect.y);

		m_render_resource->m_material_descriptor_set_layout = static_cast<RenderPass*>(m_render_pipeline->m_main_camera_pass.get())->m_descriptors[MainCameraRenderPass::DescriptorSetLayoutType::mesh_per_material].descriptor_set_layout;
		m_render_resource->m_per_mesh_descriptor_set_layout = static_cast<RenderPass*>(m_render_pipeline->m_main_camera_pass.get())->m_descriptors[MainCameraRenderPass::DescriptorSetLayoutType::per_mesh].descriptor_set_layout;

	}

	void RenderSystem::swapLogicRenderData()
	{
		m_swap_context.swapLogicRenderData();

	}

	uint32_t RenderSystem::getMeshIDByPickedUV(Vector2 picked_uv)
	{
		return  m_render_pipeline->getGUIDOfPickedMesh(picked_uv);
	}

	void RenderSystem::setVisibleAxis(std::optional<RenderEntity> axis)
	{
		if (axis.has_value())
		{
			m_render_scene->setVisibleAxisEntity(axis);
			static_cast<RenderPipeline*>(m_render_pipeline.get())->setVisibleAxis(true);
		}
		else
		{
			static_cast<RenderPipeline*>(m_render_pipeline.get())->setVisibleAxis(false);
		}
	}

	RenderGUIDAllocator<MeshSourceDesc>& RenderSystem::getMeshAssetIDAllocator()
	{
		return m_render_scene->getMeshAssetIdAllocator();
	}

	RenderGUIDAllocator<GameObjectPartId>& RenderSystem::getInstanceIDAllocator()
	{
		return m_render_scene->getInstanceIdAllocator();
	}

	void RenderSystem::updateEngineContentViewport(Vector2 content_position, Vector2 content_size)
	{
		VulkanRHI* m_vulkan_rhi = static_cast<VulkanRHI*>(m_rhi.get());
		m_vulkan_rhi->m_viewport.x = content_position.x;
		m_vulkan_rhi->m_viewport.y = content_position.y;
		m_vulkan_rhi->m_viewport.width = content_size.x;
		m_vulkan_rhi->m_viewport.height = content_size.y;
	}

	void RenderSystem::uploadGameResource(RenderEntity* entity, RenderMeshData mesh_data)
	{
		m_render_resource->uploadGameObjectRenderResource(m_rhi, entity->mesh_asset_id, mesh_data);
	}

	void RenderSystem::setSelectedAxis(size_t selected_axis)
	{
		static_cast<RenderPipeline*>(m_render_pipeline.get())->setSelectedAxis(selected_axis);
	}
	void RenderSystem::initializeUIRenderBackend(WindowUI* ui)
	{
		m_render_pipeline->initializeUIRenderBackend(ui);
	}

	size_t RenderSystem::createMaterial(RenderMaterialData& material_data)
	{
		size_t material_id = m_render_scene->getMaterialAssetIdAllocator().allocateNewGUId();
		m_render_resource->getOrCreateVulkanMaterial(m_rhi, material_id, material_data);
		return material_id;
	}

	size_t RenderSystem::postAndLoadMeshSource(const std::string& file_path)
	{
		//mesh
		MeshSourceDesc mesh_source = { file_path };
		const bool is_mesh_loaded = m_render_scene->getMeshAssetIdAllocator().hasElement(mesh_source);
		
		size_t mesh_asset_id = m_render_scene->getMeshAssetIdAllocator().allocateGUId(mesh_source);
		if (!is_mesh_loaded)
		{
			RenderMeshData mesh_data;
			
			mesh_data = m_render_resource->loadMeshData(mesh_source);
			m_render_resource->uploadGameObjectRenderResource(m_rhi, mesh_asset_id, mesh_data);
		}
		return mesh_asset_id;
		
	}

	size_t RenderSystem::postAndLoadMaterialSource(MaterialRes & material_res,bool with_texture)
	{
		std::shared_ptr<AssetManager> asset_manager = g_runtime_global_context.m_asset_manager;
		//material
		MaterialSourceDesc material_source ;
		if (with_texture)
		{
			material_source = { 
				asset_manager->getFullPath(material_res.m_base_colour_texture_file).generic_string()
				 };
		}
		else
		{
			material_source = { "E:/VS_Project/QyhsEngine/engine/source/runtime/asset/texture/default/albedo.jpg"};
		}
		const bool is_material_loaded = m_render_scene->getMaterialAssetIdAllocator().hasElement(material_source);

		size_t material_asset_id = m_render_scene->getMaterialAssetIdAllocator().allocateGUId(material_source);
		if (!is_material_loaded)
		{
			RenderMaterialData material_data;

			material_data = m_render_resource->loadMaterialData(material_source);
			m_render_resource->uploadGameObjectRenderResource(m_rhi, material_asset_id, material_data);
		}
		return material_asset_id;
	}

	size_t RenderSystem::createNewEntity(std::string name)
	{
		return size_t();
	}

	size_t RenderSystem::createMesh(RenderMeshData& mesh_data)
	{
		size_t mesh_id = m_render_scene->getMeshAssetIdAllocator().allocateNewGUId();
		m_render_resource->getOrCreateVulkanMesh(m_rhi, mesh_id, mesh_data);
		return mesh_id;
	}


}
