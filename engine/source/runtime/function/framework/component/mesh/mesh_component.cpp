#include "mesh_component.h"
//#include <function/render/game_object_desc.h>
#include <function/global/global_context.h>
#include <function/render/render_swap_context.h>
#include <function/render/render_system.h>
#include "function/framework/component/transform/transform_component.h"
#include "resource/asset_manager/asset_manager.h"
#include "resource/type/data/material.h"
#include "function/framework/component/animation/animation.h"
#include <iostream>
namespace QYHS
{
	void MeshComponent::tick(double time)
	{
		//dirty means that something about the component has changed,so component must be reload
		TransformComponent* owner_transform_comp = m_owner.lock()->TryGetComponent(TransformComponent);
		Matrix4x4 parent_world_matrix;
		if (m_owner.lock()->getParent() != nullptr)
		{
			auto parent_trans_comp = m_owner.lock()->getParent()->TryGetComponent(TransformComponent);
			parent_world_matrix = parent_trans_comp->world_matrix;
		}
		else
		{
			parent_world_matrix = Matrix4x4::IDENTITY;
		}
		if (owner_transform_comp && owner_transform_comp->isDirty())
		{
			std::vector<GameObjectPartDesc>	dirty_mesh_parts;
			SkeletonAnimationResult animation_result;
			animation_result.m_transforms.push_back({ Matrix4x4::IDENTITY });
			AnimationComponent* animation_component = m_owner.lock()->TryGetComponent(AnimationComponent);
			if (animation_component)
			{
				std::vector<AnimationResultElement> nodes = animation_component->getResult().node;
				for (auto& node : nodes)
				{
					animation_result.m_transforms.push_back({ Matrix4x4(node.transform) });
				}
			}
			for (GameObjectPartDesc& part : m_raw_meshes)
			{
				if (animation_component)
				{
					
					part.m_skeleton_animation_result = animation_result;
					//part.m_skeleton_binding_desc.m_skeleton_binding_file = part.m_mesh_desc.m_mesh_file;
				}
				Matrix4x4 local_transform_matrix = part.m_transform_desc.m_transform_matrix;
				part.m_transform_desc.m_transform_matrix = local_transform_matrix * owner_transform_comp->getMatrix();
				Vector3 translate = part.m_transform_desc.m_transform_matrix.getTrans();
				dirty_mesh_parts.emplace_back(part);
				part.m_transform_desc.m_transform_matrix = local_transform_matrix;
			}
			RenderSwapContext& swap_context = g_runtime_global_context.m_render_system->getSwapContext();
			SwapData& logic_swap_data = swap_context.getLogicSwapData();


			logic_swap_data.addDirtyGameObject(GameObjectDesc{ m_owner.lock()->getObjectId(),dirty_mesh_parts });
		}

		owner_transform_comp->setDirty(false);

	}

	void MeshComponent::postLoadResource(std::weak_ptr<GameObject> parent_object)
	{
		m_owner = parent_object;
		Component::postLoadResource(parent_object);
		std::shared_ptr<RenderSystem> render_system = g_runtime_global_context.m_render_system;
		render_system->postLoadMeshComponentRes(m_mesh_res);
		m_raw_meshes.resize(m_mesh_res.m_sub_meshes.size());
		std::shared_ptr<AssetManager> asset_manager = g_runtime_global_context.m_asset_manager;
		for (size_t index = 0; index < m_raw_meshes.size(); ++index)
		{
			GameObjectPartDesc& raw_mesh = m_raw_meshes[index];
			SubMeshRes& sub_mesh = m_mesh_res.m_sub_meshes[index];

			raw_mesh.m_mesh_id = render_system->postAndLoadMeshSource(asset_manager->getFullPath(sub_mesh.m_obj_file_ref).generic_string());
			
			MaterialRes material_res;
			if (!(sub_mesh.m_material.empty()))
			{
				assert(asset_manager->loadAsset(sub_mesh.m_material, material_res) && "failed to load material res!");
			}
			else
			{
				material_res = { "E:/VS_Project/QyhsEngine/engine/source/runtime/asset/texture/default/albedo.jpg"};
			}
			
			raw_mesh.m_material_id = render_system->postAndLoadMaterialSource(material_res,true);
			
			auto object_space_transform = sub_mesh.m_transform.getMatrix();
			raw_mesh.m_transform_desc.m_transform_matrix = object_space_transform;
		}
	}
}
