#include "scene_component.h"
#include "function/framework/component/ecs.h"
namespace qyhs::scene
{
	void MaterialComponent::serialize(Archive& archive, ecs::EntitySerializer& seri)
	{
		if (archive.isReadMode())
		{
			archive >> (uint8_t&)userBlendMode;
			archive >> textures[BASECOLORMAP].name;
			archive >> textures[BASECOLORMAP].uvset;
			//jobsystem::execute(seri.ctx, [&](jobsystem::JobArgs args) {
				createRenderData();
			//	});
		}
		else
		{
			archive << (uint8_t)userBlendMode;
			archive << textures[BASECOLORMAP].name;
			archive << textures[BASECOLORMAP].uvset;

			for (auto& texture : textures)
			{
				seri.registerResource(texture.name);
			}

		}
	}

	void ObjectComponent::serialize(Archive& archive, ecs::EntitySerializer& seri)
	{
		if (archive.isReadMode())
		{
			ecs::serializeEntity(archive, mesh_entity, seri);
			archive >> filter_mask;
		}
		else
		{
			ecs::serializeEntity(archive, mesh_entity, seri);
			archive << filter_mask;
		}
	}

	void MeshComponent::serialize(Archive& archive, ecs::EntitySerializer& seri)
	{
		if (archive.isReadMode())
		{
			archive >> _flags;
			archive >> vertex_positions;
			archive >> vertex_normals;
			archive >> vertex_uvset_0;
			archive >> vertex_boneindices;
			archive >> vertex_boneweights;
			archive >> vertex_atlas;
			archive >> vertex_colors;
			archive >> indices;

			uint32_t subset_count = 0;
			archive >> subset_count;
			subsets.resize(subset_count);
			for (int i = 0; i < subsets.size(); ++i)
			{
				ecs::serializeEntity(archive, subsets[i].materialID, seri);
				archive >> subsets[i].index_offset;
				archive >> subsets[i].index_count;
			}

			//jobsystem::execute(seri.ctx, [&](jobsystem::JobArgs args) {
				createRenderData();
				//});
		}
		else
		{
			archive << _flags;
			archive << vertex_positions;
			archive << vertex_normals;
			archive << vertex_uvset_0;
			archive << vertex_boneindices;
			archive << vertex_boneweights;
			archive << vertex_atlas;
			archive << vertex_colors;
			archive << indices;
			archive << subsets.size();
			for (int i = 0; i < subsets.size(); ++i)
			{
				ecs::serializeEntity(archive, subsets[i].materialID, seri);
				archive << subsets[i].index_offset;
				archive << subsets[i].index_count;
			}

		}
	}

	void NameComponent::serialize(Archive& archive, ecs::EntitySerializer& seri)
	{
	}

	void HierarchyComponent::serialize(Archive& archive, ecs::EntitySerializer& seri)
	{
	}
}