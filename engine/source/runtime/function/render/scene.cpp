#include "scene.h"
#include "function/render/renderer.h"
#include "core/jobsystem.h"
using namespace qyhs::ecs;
namespace qyhs::scene
{
	const uint32_t small_subtask_groupsize = 64u;
	Scene* getScene()
	{
		static Scene scene;
		return &scene;
	}

	void Scene::updateObjects(jobsystem::Context& ctx)
	{
		aabb_objects.resize(objects.getCount());
		object_matrices.resize(objects.getCount());
		occlusion_result_objects.resize(objects.getCount());
		jobsystem::dispatch(ctx, (uint32_t)objects.getCount(), small_subtask_groupsize, [&](jobsystem::JobArgs args) {
			Entity entity = objects.getEntity(args.job_index);
			primitive::AABB& aabb = aabb_objects[args.job_index];
			scene::ObjectComponent& object = objects[args.job_index];
			aabb = primitive::AABB();

			//TODO:update occlusion culling states
			OccludedResult occluded_result = occlusion_result_objects[args.job_index];
			
			if (object.mesh_entity != INVALID_ENTITY && meshes.contain(object.mesh_entity) && transforms.contain(entity))
			{
				object.mesh_index = meshes.getIndex(object.mesh_entity);
				const scene::MeshComponent& mesh = meshes[object.mesh_index];

				const scene::TransformComponent& transform = *transforms.getComponent(entity);
				XMMATRIX w = XMLoadFloat4x4(&transform.world);
				aabb = mesh.aabb.transform(w);

				object.center = aabb.getCenter();
				object.radius = aabb.getRadius();


				//create mesh instance gpu data
				ShaderMeshInstance instance;
				instance.init();

				XMStoreFloat4x4(object_matrices.data() + args.job_index, w);
				XMFLOAT4X4 world_matrix = object_matrices[args.job_index];

				if (graphics::IsFormatUnorm(mesh.position_format))
				{
					XMMATRIX remap = mesh.aabb.getUnormRemapMatrix();
					XMStoreFloat4x4(&world_matrix, remap * w);
				}

				instance.transform.create(world_matrix);
				std::memcpy(instance_upload_buffer_mapped + args.job_index, &instance, sizeof(instance));
			}


			});


	}

	void Scene::merge(Scene& other)
	{
		for (auto& entry : component_library.entries)
		{
			entry.second.component_manager->merge(*other.component_library.entries[entry.first].component_manager);
		}

		aabb_objects.insert(aabb_objects.end(), other.aabb_objects.begin(), other.aabb_objects.end());
		object_matrices.insert(object_matrices.end(), other.object_matrices.begin(), other.object_matrices.end());
	}

	void Scene::updateMeshes(jobsystem::Context& ctx)
	{
		jobsystem::dispatch(ctx, (uint32_t)meshes.getCount(), small_subtask_groupsize, [&](jobsystem::JobArgs args) {
			Entity entity = meshes.getEntity(args.job_index);
			scene::MeshComponent& mesh = meshes[args.job_index];

			if (geometry_upload_buffer_mapped != nullptr)
			{
				ShaderGeometry shader_geometry = {};
				shader_geometry.init();
				shader_geometry.index_buffer = mesh.ib.descriptor_srv;
				shader_geometry.vertex_buffer_position_wind = mesh.vb_pos_wind.descriptor_srv;
				shader_geometry.vb_uvs = mesh.vb_uvs.descriptor_srv;
				uint32_t subset_index = 0;
				for (auto& subset : mesh.subsets)
				{
					const scene::MaterialComponent* material = materials.getComponent(entity);
					if (material != nullptr)
					{
						subset.material_index = materials.getIndex(subset.materialID);
					}
					else
					{
						subset.material_index = 0;
					}
					ShaderGeometry subset_geometry = shader_geometry;

					std::memcpy(geometry_upload_buffer_mapped + mesh.geometry_offset + subset_index, &subset_geometry, sizeof(subset_geometry));
					++subset_index;
				}
			}


			});
	}

	void Scene::updateMaterial(jobsystem::Context& ctx)
	{
		jobsystem::dispatch(ctx, (uint32_t)materials.getCount(), small_subtask_groupsize, [&](jobsystem::JobArgs args) {
			MaterialComponent& material = materials[args.job_index];
			Entity entity = materials.getEntity(args.job_index);
			material.writeShaderMaterial(material_upload_buffer_mapped + args.job_index);
			});
	}

	Entity Scene::createMaterialEntity(const std::string& name)
	{
		Entity entity = createEntity();
		materials.create(entity);
		names.create(entity) = name;

		return entity;
	}

	Entity Scene::createObjectEntity(const std::string& name)
	{
		Entity entity = createEntity();
		objects.create(entity);
		names.create(entity) = name;
		transforms.create(entity);
		return entity;
	}

	Entity Scene::createMeshEntity(const std::string& name)
	{
		Entity entity = createEntity();
		meshes.create(entity);
		names.create(entity) = name;

		return entity;
	}

	void Scene::update(float delta_time)
	{
		jobsystem::Context ctx;
		RHI* rhi = rhi::getRHI();
		scene_instance_count = objects.getCount();
		if (instance_upload_buffers[0].desc.size < scene_instance_count * sizeof(ShaderMeshInstance))
		{
			GPUBufferDesc desc;
			desc.stride = sizeof(ShaderMeshInstance);
			desc.size = sizeof(ShaderMeshInstance) * scene_instance_count * 2;
			desc.bind_flags = BindFlag::SHADER_RESOURCE;
			desc.misc_flags = ResourceMiscFlag::BUFFER_STRUCTURED;
			rhi->createBuffer(&desc, &instance_buffer);
			rhi->setName(&instance_buffer, "Scene::instance_buffer");
			desc.bind_flags = BindFlag::NONE;
			desc.misc_flags = ResourceMiscFlag::NONE;
			desc.usage = Usage::UPLOAD;
			for (int i = 0; i < arraysize(instance_upload_buffers); ++i)
			{
				rhi->createBuffer(&desc, &instance_upload_buffers[i]);
				rhi->setName(&instance_upload_buffers[i], "Scene::instance_upload_buffer");
			}
		}
		instance_upload_buffer_mapped = (ShaderMeshInstance*)instance_upload_buffers[rhi->getBufferIndex()].mapped_data;
		materials_array_size = materials.getCount();
		if (material_upload_buffers[0].desc.size < materials_array_size * sizeof(ShaderMaterial))
		{
			GPUBufferDesc desc;
			desc.stride = sizeof(ShaderMaterial);
			desc.size = sizeof(ShaderMaterial) * materials_array_size * 2;
			desc.bind_flags = BindFlag::SHADER_RESOURCE;
			desc.misc_flags = ResourceMiscFlag::BUFFER_STRUCTURED;
			rhi->createBuffer(&desc, &material_buffer);
			rhi->setName(&material_buffer, "Scene::material_buffer");
			desc.bind_flags = BindFlag::NONE;
			desc.misc_flags = ResourceMiscFlag::NONE;
			desc.usage = Usage::UPLOAD;
			for (int i = 0; i < arraysize(material_upload_buffers); ++i)
			{
				rhi->createBuffer(&desc, &material_upload_buffers[i]);
				rhi->setName(&material_upload_buffers[i], "Scene::material_upload_buffer");
			}
		}
		material_upload_buffer_mapped = (ShaderMaterial*)material_upload_buffers[rhi->getBufferIndex()].mapped_data;

		if (renderer::getOcclusionCullingEnabled())
		{

		}

		if (delta_time > 0)
		{
			geometry_allocator.store(0u);
			jobsystem::dispatch(ctx, meshes.getCount(), small_subtask_groupsize, [&](jobsystem::JobArgs args)
				{
					MeshComponent& mesh = meshes[args.job_index];
					mesh.geometry_offset = geometry_allocator.fetch_add((uint32_t)mesh.subsets.size());
				});
			jobsystem::execute(ctx, [&](jobsystem::JobArgs args) {
				ShaderMeshInstance instance;
				instance.init();
				for (int i = 0; i < scene_instance_count; ++i)
				{
					std::memcpy(instance_upload_buffer_mapped + i, &instance, sizeof(instance));
				}
				});

		}

		jobsystem::wait(ctx);

		geometry_array_size = geometry_allocator.load();
		if (geometry_upload_buffers[0].desc.size < geometry_array_size * sizeof(ShaderGeometry))
		{
			GPUBufferDesc desc;
			desc.stride = sizeof(ShaderGeometry);
			desc.size = sizeof(ShaderGeometry) * geometry_array_size * 2;
			desc.bind_flags = BindFlag::SHADER_RESOURCE;
			desc.misc_flags = ResourceMiscFlag::BUFFER_STRUCTURED;
			rhi->createBuffer(&desc, &geometry_buffer);
			rhi->setName(&geometry_buffer, "Scene::geometry_buffer");
			desc.bind_flags = BindFlag::NONE;
			desc.misc_flags = ResourceMiscFlag::NONE;
			desc.usage = Usage::UPLOAD;
			for (int i = 0; i < arraysize(geometry_upload_buffers); ++i)
			{
				rhi->createBuffer(&desc, &geometry_upload_buffers[i]);
				rhi->setName(&geometry_upload_buffers[i], "Scene::geometry_upload_buffer");
			}
		}
		geometry_upload_buffer_mapped = (ShaderGeometry*)geometry_upload_buffers[rhi->getBufferIndex()].mapped_data;

		updateObjects(ctx);
		updateMeshes(ctx);
		updateMaterial(ctx);
		jobsystem::wait(ctx);

		updateShaderScene();
	}

	void Scene::updateShaderScene()
	{
		RHI* rhi = rhi::getRHI();
		shader_scene.instance_buffer = rhi->getDescriptorIndex(&instance_buffer, SubresourceType::SRV);
		shader_scene.geometry_buffer = rhi->getDescriptorIndex(&geometry_buffer, SubresourceType::SRV);
		shader_scene.material_buffer = rhi->getDescriptorIndex(&material_buffer, SubresourceType::SRV);
	}

}
