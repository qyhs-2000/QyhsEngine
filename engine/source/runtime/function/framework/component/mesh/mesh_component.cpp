#include "mesh_component.h"
//#include <function/render/game_object_desc.h>
#include <function/global/global_context.h>
#include <function/render/render_swap_context.h>
#include <function/render/render_system.h>
#include "function/framework/component/transform/transform_component.h"
#include "resource/asset_manager/asset_manager.h"
#include "resource/type/data/material.h"
#include <iostream>
namespace QYHS
{
	void MeshComponent::tick(double time)
	{
		//dirty means that something about the component has changed,so component must be reload
		TransformComponent* transform_component = m_parent_object.lock()->TryGetComponent(TransformComponent);
		if (transform_component->isDirty())
		{
			std::vector<GameObjectPartDesc>	dirty_mesh_parts;

			for (GameObjectPartDesc& part : m_raw_meshes)
			{
				Matrix4x4 local_transform_matrix = part.m_transform_desc.m_transform_matrix;
				part.m_transform_desc.m_transform_matrix = transform_component->getMatrix() * local_transform_matrix;
				Vector3 translate = part.m_transform_desc.m_transform_matrix.getTrans();
				dirty_mesh_parts.emplace_back(part);
				part.m_transform_desc.m_transform_matrix = local_transform_matrix;
			}
			RenderSwapContext& swap_context = g_runtime_global_context.m_render_system->getSwapContext();
			SwapData& logic_swap_data = swap_context.getLogicSwapData();


			logic_swap_data.addDirtyGameObject(GameObjectDesc{ m_parent_object.lock()->getObjectId(),dirty_mesh_parts });
		}

		transform_component->setDirty(false);

	}

	void MeshComponent::postLoadResource(std::weak_ptr<GameObject> parent_object)
	{
		m_parent_object = parent_object;
		Component::postLoadResource(parent_object);
		m_raw_meshes.resize(m_mesh_res.m_sub_meshes.size());
		std::shared_ptr<AssetManager> asset_manager = g_runtime_global_context.m_asset_manager;
		for (size_t index = 0; index < m_raw_meshes.size(); ++index)
		{
			GameObjectPartDesc& mesh_component = m_raw_meshes[index];
			SubMeshRes& sub_mesh = m_mesh_res.m_sub_meshes[index];
			mesh_component.m_mesh_desc.m_mesh_file = asset_manager->getFullPath(sub_mesh.m_obj_file_ref).generic_string();
			mesh_component.m_material_desc.m_with_texture = !sub_mesh.m_material.empty();
			if (mesh_component.m_material_desc.m_with_texture)
			{
				MaterialRes material_res;
				assert(asset_manager->loadAsset(sub_mesh.m_material, material_res) && "failed to load material res!");
				mesh_component.m_material_desc.m_base_colour_texture_file = asset_manager->getFullPath(material_res.m_base_colour_texture_file).generic_string();

			}
			auto object_space_transform = sub_mesh.m_transform.getMatrix();
			mesh_component.m_transform_desc.m_transform_matrix = object_space_transform;
		}
	}
}
