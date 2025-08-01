#include "scene.h"
#include "function/render/renderer.h"
#include "core/jobsystem.h"
#include <cassert>
//#include "core/common_include.h"
//#include "core/math/math_library.h"
#include <iostream>
using namespace qyhs::ecs;
namespace qyhs::scene
{
	const uint32_t small_subtask_groupsize = 64u;
	Scene* getScene()
	{
		static Scene scene;
		return &scene;
	}

	ecs::Entity loadModel(Scene& scene, const std::string& filename, const XMMATRIX& transformMatrix, bool attached)
	{
		Entity root_entity = INVALID_ENTITY;
		if (attached)
		{
			root_entity = createEntity();
		}
		LoadModel2(scene, filename, transformMatrix, root_entity);
		return ecs::Entity();
	}

	void LoadModel2(Scene& scene, const std::string& fileName, const XMMATRIX& transformMatrix, ecs::Entity rootEntity)
	{
		Archive archive(fileName, true);
		if (!archive.isOpen())
		{
			return;
		}
		scene.serialize(archive);
		bool attached = true;
		if (rootEntity == INVALID_ENTITY)
		{
			rootEntity = createEntity();
			attached = false;
		}
		scene.transforms.create(rootEntity);

		{
			// Apply the optional transformation matrix to the new scene:

			// Parent all unparented transforms to new root entity
			for (size_t i = 0; i < scene.transforms.getCount(); ++i)
			{
				Entity entity = scene.transforms.getEntity(i);
				if (entity != rootEntity && !scene.hierarchy.contain(entity))
				{
					scene.attachComponent(entity, rootEntity);
				}
			}

			// The root component is transformed, scene is updated:
			TransformComponent* root_transform = scene.transforms.getComponent(rootEntity);
			root_transform->MatrixTransform(transformMatrix);

			scene.update(0);
		}

		if (!attached)
		{
			// In this case, we don't care about the root anymore, so delete it. This will simplify overall hierarchy
			scene.Component_DetachChildren(rootEntity);
			scene.Entity_Remove(rootEntity);
		}
	}

	PickResult Pick(const primitive::Ray& ray, uint32_t filterMask, uint32_t layerMask, const Scene& scene, uint32_t lod)
	{
		return scene.Intersects(ray,filterMask,lod);
	}

	XMVECTOR SkinVertex(const MeshComponent& mesh, const ArmatureComponent& armature, uint32_t index, XMVECTOR* N)
	{
		XMVECTOR P = XMLoadFloat3(&mesh.vertex_positions[index]);
		const XMUINT4& ind = mesh.vertex_boneindices[index];
		const XMFLOAT4& wei = mesh.vertex_boneweights[index];

		const XMFLOAT4X4 mat[] = {
			armature.bone_data[ind.x].getMatrix(),
			armature.bone_data[ind.y].getMatrix(),
			armature.bone_data[ind.z].getMatrix(),
			armature.bone_data[ind.w].getMatrix(),
		};
		const XMMATRIX M[] = {
			XMMatrixTranspose(XMLoadFloat4x4(&mat[0])),
			XMMatrixTranspose(XMLoadFloat4x4(&mat[1])),
			XMMatrixTranspose(XMLoadFloat4x4(&mat[2])),
			XMMatrixTranspose(XMLoadFloat4x4(&mat[3])),
		};

		XMVECTOR skinned;
		skinned = XMVectorScale( XMVector3Transform(P, M[0]) , wei.x);
		skinned += XMVectorScale(XMVector3Transform(P, M[1]) , wei.y);
		skinned += XMVectorScale(XMVector3Transform(P, M[2]) , wei.z);
		skinned += XMVectorScale(XMVector3Transform(P, M[3]) , wei.w);
		P = skinned;

		if (N != nullptr)
		{
			*N = XMLoadFloat3(&mesh.vertex_normals[index]);
			skinned = XMVectorScale(XMVector3TransformNormal(*N, M[0]) , wei.x);
			skinned += XMVectorScale(XMVector3TransformNormal(*N, M[1]) , wei.y);
			skinned += XMVectorScale(XMVector3TransformNormal(*N, M[2]) , wei.z);
			skinned += XMVectorScale(XMVector3TransformNormal(*N, M[3]) , wei.w);
			*N = XMVector3Normalize(skinned);
		}

		return P;
	}

	Scene::RayIntersectionResult Scene::Intersects(const primitive::Ray& ray, uint32_t filterMask,uint32_t lod) const
	{
		RayIntersectionResult result;

		const XMVECTOR rayOrigin = XMLoadFloat3(&ray.origin);
		const XMVECTOR rayDirection = XMVector3Normalize(XMLoadFloat3(&ray.direction));

		if (filterMask & FILTER_OBJECT_ALL)
		{
			for (size_t objectIndex = 0; objectIndex < aabb_objects.size(); ++objectIndex)
			{
				const primitive::AABB& aabb = aabb_objects[objectIndex];
				if (!ray.intersects(aabb))
				{
					continue;
				}
				const ObjectComponent& object = objects[objectIndex];
				if (object.mesh_entity == INVALID_ENTITY)
				{
					continue;
				}
				if ((filterMask & object.getFilterMask()) == 0)
				{
					continue;
				}

				const Entity entity = objects.getEntity(objectIndex);
				const MeshComponent* mesh = meshes.getComponent(object.mesh_entity);
				const ArmatureComponent* armature = mesh->isSkinned() ? armatures.getComponent(mesh->armatureID):nullptr;
				XMMATRIX objectMatrix = XMLoadFloat4x4(&object_matrices[objectIndex]);
				XMMATRIX objectMatrix_Inverse = XMMatrixInverse(nullptr, objectMatrix);
				const XMVECTOR rayOrigin_local = XMVector3Transform(rayOrigin, objectMatrix_Inverse);
				const XMVECTOR rayDirection_local = XMVector3Transform(rayDirection, objectMatrix_Inverse);
				if (!mesh)
				{
					continue;
				}

				auto intersect_triangle = [&](uint32_t subsetIndex, uint32_t indexOffset, uint32_t triangleIndex)
					{
						const uint32_t i0 = mesh->indices[indexOffset + triangleIndex * 3 + 0];
						const uint32_t i1 = mesh->indices[indexOffset + triangleIndex * 3 + 1];
						const uint32_t i2 = mesh->indices[indexOffset + triangleIndex * 3 + 2];

						XMVECTOR p0;
						XMVECTOR p1;
						XMVECTOR p2;

						if (armature == nullptr || armature->bone_data.empty())
						{
							p0 = XMLoadFloat3(&mesh->vertex_positions[i0]);
							p1 = XMLoadFloat3(&mesh->vertex_positions[i1]);
							p2 = XMLoadFloat3(&mesh->vertex_positions[i2]);
						}
						else
						{
							p0 = SkinVertex(*mesh, *armature, i0);
							p1 = SkinVertex(*mesh, *armature, i1);
							p2 = SkinVertex(*mesh, *armature, i2);
						}

						float distance;
						XMFLOAT2 bary;
						if (math::RayTriangleIntersects(rayOrigin_local, rayDirection_local, p0, p1, p2, distance, bary))
						{
							const XMVECTOR pos_local = XMVectorAdd(rayOrigin_local, XMVectorScale(rayDirection_local ,distance));
							const XMVECTOR pos = XMVector3Transform(pos_local, objectMatrix);

							distance = math::Distance(pos, rayOrigin);

							if (distance < result.distance && distance >= ray.TMin && distance <= ray.TMax)
							{
								result.entity = entity;
								XMStoreFloat3(&result.position, pos);
								result.distance = distance;
								result.subsetIndex = (int)subsetIndex;
								result.vertexID0 = (int)i0;
								result.vertexID1 = (int)i1;
								result.vertexID2 = (int)i2;
							}
						}
					};
				
				// Brute-force intersection test:
				int first_subset = 0;
				int last_subset = 0;
				mesh->getLodSubsetRange(lod, first_subset, last_subset);
				for (uint32_t subsetIndex = first_subset; subsetIndex < last_subset; ++subsetIndex)
				{
					const MeshComponent::MeshSubset& subset = mesh->subsets[subsetIndex];
					if (subset.index_count == 0)
						continue;
					const uint32_t indexOffset = subset.index_offset;
					const uint32_t triangleCount = subset.index_count / 3;

					for (uint32_t triangleIndex = 0; triangleIndex < triangleCount; ++triangleIndex)
					{
						intersect_triangle(subsetIndex, indexOffset, triangleIndex);
					}
				}
			}
		}

		return result;
	}

	void Scene::scanAnimationDepedencies()
	{
		if (animations.getCount() == 0)
		{
			animation_queue_count = 0;
			return;
		}

		animation_queues.reserve(animations.getCount());
		animation_queue_count = 0;

		jobsystem::Execute(animation_dependency_scan_workload, [&](jobsystem::JobArgs args) {

			for (size_t i = 0; i < animations.getCount(); ++i)
			{
				AnimationComponent& animationA = animations[i];
				if (!animationA.isPlaying() && animationA.last_update_time == animationA.timer)
				{
					continue;
				}
				bool dependency = false;
				for (size_t queue_index = 0; queue_index < animation_queue_count; ++queue_index)
				{
					AnimationQueue& queue = animation_queues[queue_index];
					for (auto& channelA : animationA.channels)
					{
						if (dependency)
						{
							// If dependency has been found, record all other entities in this animation too:
							queue.entities.insert(channelA.target);
						}
						else if (queue.entities.find(channelA.target) != queue.entities.end())
						{
							// If two animations target the same entity, they have a dependency and need to be executed in order:
							dependency = true;
							queue.animations.push_back(&animationA);
						}
					}
					if (dependency) break;
				}
				if (!dependency)
				{
					// No dependency, it can be executed on a separate queue (thread)
					if (animation_queues.size() <= animation_queue_count)
					{
						animation_queues.resize(animation_queue_count + 1);
					}
					AnimationQueue& queue = animation_queues[animation_queue_count];
					queue.animations.clear();
					queue.animations.push_back(&animationA);
					queue.entities.clear();
					for (auto& channelA : animationA.channels)
					{
						queue.entities.insert(channelA.target);
					}
					animation_queue_count++;
				}
			}
			});
	}

	void Scene::Component_DetachChildren(Entity parent)
	{
		for (int i = 0; i < hierarchy.getCount();)
		{
			if (hierarchy[i].parent_id == parent)
			{
				Entity entity = hierarchy.getEntity(i);
				detachComponent(entity);
			}
			else
			{
				++i;
			}
		}
	}

	void Scene::Entity_Remove(ecs::Entity entity, bool recursive)
	{
		if (recursive)
		{
			std::vector<Entity> entities_to_remove;
			for (int i = 0; i < hierarchy.getCount(); ++i)
			{
				const HierarchyComponent& hier = hierarchy[i];
				if (hier.parent_id == entity)
				{
					Entity child = hierarchy.getEntity(i);
					entities_to_remove.push_back(child);
				}
			}
			for (int i = 0; i < entities_to_remove.size(); ++i)
			{
				Entity_Remove(entities_to_remove[i]);
			}
		}

		for (auto& entry : component_library.entries)
		{
			entry.second.component_manager->remove(entity);
		}
	}


	void Scene::serialize(Archive& archive)
	{
		if (archive.isReadMode())
		{
			uint32_t reserved;
			archive >> reserved;
		}
		else
		{
			uint32_t reserved = 0;
			archive << reserved;
		}

		size_t jump_before = 0;
		size_t jump_after = 0;
		size_t original_pos = 0;

		if (archive.isReadMode())
		{
			archive >> jump_before;
			archive >> jump_after;
			original_pos = archive.getPos();
			archive.jump(jump_before);
		}
		else
		{
			jump_before = archive.writeUnKnownJumpPosition();
			jump_after = archive.writeUnKnownJumpPosition();
		}
		resourcemanager::ResourceSerializer resource_seri;
		if (archive.isReadMode())
		{
			resourcemanager::Serialize_READ(archive, resource_seri);
			archive.jump(original_pos);
		}
		EntitySerializer seri;
		seri.ctx.priority = jobsystem::Priority::Low;
		component_library.serialize(archive, seri);

		if (archive.isReadMode())
		{
			archive.jump(jump_after);
		}
		else
		{
			archive.patchUnKnownJumpPosition(jump_before);
			resourcemanager::Serialize_WRITE(archive, seri.resource_registration);
			archive.patchUnKnownJumpPosition(jump_after);

		}
	}

	void Scene::updateObjects(jobsystem::context& ctx)
	{
		aabb_objects.resize(objects.getCount());
		object_matrices.resize(objects.getCount());
		occlusion_result_objects.resize(objects.getCount());
		jobsystem::Dispatch(ctx, (uint32_t)objects.getCount(), small_subtask_groupsize, [&](jobsystem::JobArgs args) {
			Entity entity = objects.getEntity(args.jobIndex);
			primitive::AABB& aabb = aabb_objects[args.jobIndex];
			scene::ObjectComponent& object = objects[args.jobIndex];
			aabb = primitive::AABB();

			//TODO:update occlusion culling states
			OccludedResult occluded_result = occlusion_result_objects[args.jobIndex];

			if (object.mesh_entity != INVALID_ENTITY && meshes.contain(object.mesh_entity) && transforms.contain(entity))
			{
				object.mesh_index = meshes.getIndex(object.mesh_entity);
				const scene::MeshComponent& mesh = meshes[object.mesh_index];

				const scene::TransformComponent& transform = *transforms.getComponent(entity);
				XMMATRIX w = XMLoadFloat4x4(&transform.world);
				aabb = mesh.aabb.transform(w);

				object.center = aabb.getCenter();
				object.radius = aabb.getRadius();

				if (mesh.isSkinned())
				{

					const ArmatureComponent* armature = armatures.getComponent(mesh.armatureID);
					if (armature != nullptr)
					{
						aabb = primitive::AABB::Merge(aabb, armature->aabb);
					}
				}

				int first_subset = 0, last_subset = 0;
				mesh.getLodSubsetRange(object.lod, first_subset, last_subset);
				for (int i = first_subset; i < last_subset; ++i)
				{
					const MeshComponent::MeshSubset& subset = mesh.subsets[i];
					const MaterialComponent* material = materials.getComponent(subset.materialID);
					if (material != nullptr)
					{
						object.filter_mask_dynamic |= material->getFilterMask();
					}
				}
				//create mesh instance gpu data
				ShaderMeshInstance instance;
				instance.init();

				XMStoreFloat4x4(object_matrices.data() + args.jobIndex, w);
				XMFLOAT4X4 world_matrix = object_matrices[args.jobIndex];

				if (graphics::IsFormatUnorm(mesh.position_format) && !mesh.shader_output_pos.IsValid())
				{
					XMMATRIX remap = mesh.aabb.getUnormRemapMatrix();
					XMStoreFloat4x4(&world_matrix, remap * w);
				}

				instance.transform.create(world_matrix);
				std::memcpy(instance_upload_buffer_mapped + args.jobIndex, &instance, sizeof(instance));
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

	void Scene::updateMeshes(jobsystem::context& ctx)
	{
		jobsystem::Dispatch(ctx, (uint32_t)meshes.getCount(), small_subtask_groupsize, [&](jobsystem::JobArgs args) {
			Entity entity = meshes.getEntity(args.jobIndex);
			scene::MeshComponent& mesh = meshes[args.jobIndex];

			/*if (mesh.shader_output_pos.IsValid() && mesh.shader_output_pos_pre.IsValid())
			{
				std::swap(mesh.shader_output_pos, mesh.shader_output_pos_pre);
			}*/

			if (geometry_upload_buffer_mapped != nullptr)
			{
				ShaderGeometry shader_geometry = {};
				shader_geometry.init();
				shader_geometry.index_buffer = mesh.ib.descriptor_srv;
				if (mesh.shader_output_pos.IsValid())
				{
					shader_geometry.vertex_buffer_position_wind = mesh.shader_output_pos.descriptor_srv;
				}
				else
				{
					shader_geometry.vertex_buffer_position_wind = mesh.vb_pos_wind.descriptor_srv;
				}

				shader_geometry.vb_uvs = mesh.vb_uvs.descriptor_srv;
				uint32_t subset_index = 0;
				for (auto& subset : mesh.subsets)
				{
					const scene::MaterialComponent* material = materials.getComponent(subset.materialID);
					if (material != nullptr)
					{
						//TODO:
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

	void Scene::updateMaterials(jobsystem::context& ctx)
	{
		jobsystem::Dispatch(ctx, (uint32_t)materials.getCount(), small_subtask_groupsize, [&](jobsystem::JobArgs args) {
			MaterialComponent& material = materials[args.jobIndex];
			material.writeShaderMaterial(material_upload_buffer_mapped + args.jobIndex);
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
		this->dt = delta_time;
		jobsystem::context ctx;
		scanAnimationDepedencies();
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
			skinning_allocator.store(0u);
			jobsystem::Dispatch(ctx, meshes.getCount(), small_subtask_groupsize, [&](jobsystem::JobArgs args)
				{
					MeshComponent& mesh = meshes[args.jobIndex];
					mesh.geometry_offset = geometry_allocator.fetch_add((uint32_t)mesh.subsets.size());
				});
			jobsystem::Dispatch(ctx, armatures.getCount(), small_subtask_groupsize, [&](jobsystem::JobArgs args) {
				ArmatureComponent& armature = armatures[args.jobIndex];
				skinning_allocator.fetch_add((uint32_t)(armature.bone_collection.size() * sizeof(ShaderTransform)));
				});
			jobsystem::Execute(ctx, [&](jobsystem::JobArgs args) {
				ShaderMeshInstance instance;
				instance.init();
				for (int i = 0; i < scene_instance_count; ++i)
				{
					std::memcpy(instance_upload_buffer_mapped + i, &instance, sizeof(instance));
				}
				});

		}

		jobsystem::Wait(ctx);

		skinning_data_size = skinning_allocator.load();
		skinning_allocator.store(0);
		if (skinning_upload_buffers[0].desc.size < skinning_data_size)
		{
			GPUBufferDesc desc;
			//desc.stride = sizeof(ShaderTransform);
			desc.size = skinning_data_size * 2;
			desc.bind_flags = BindFlag::SHADER_RESOURCE;
			desc.misc_flags = ResourceMiscFlag::BUFFER_RAW;
			rhi->createBuffer(&desc, &skinning_buffer);
			rhi->setName(&skinning_buffer, "Scene::skinning_buffer");
			desc.bind_flags = BindFlag::NONE;
			desc.misc_flags = ResourceMiscFlag::NONE;
			desc.usage = Usage::UPLOAD;
			for (int i = 0; i < arraysize(skinning_upload_buffers); ++i)
			{
				rhi->createBuffer(&desc, &skinning_upload_buffers[i]);
				rhi->setName(&skinning_upload_buffers[i], "Scene::skinning_upload_buffer");
			}
		}
		skinning_upload_buffer_mapped = skinning_upload_buffers[rhi->getBufferIndex()].mapped_data;

		updateAnimations(ctx);
		updateTransforms(ctx);
		jobsystem::Wait(ctx);
		updateHierarchy(ctx);

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


		updateMeshes(ctx);
		updateMaterials(ctx);
		jobsystem::Wait(ctx);
		updateArmatures(ctx);
		jobsystem::Wait(ctx);
		updateObjects(ctx);
		updateShaderScene();
	}

	void Scene::updateAnimations(jobsystem::context& ctx)
	{

		static float time_test = 0.f;
		jobsystem::Wait(animation_dependency_scan_workload);
		jobsystem::Dispatch(ctx, (uint32_t)animation_queue_count, 1, [&](jobsystem::JobArgs args) {

			AnimationQueue& animation_queue = animation_queues[args.jobIndex];
			for (size_t animation_index = 0; animation_index < animation_queue.animations.size(); ++animation_index)
			{
				AnimationComponent& animation = *animation_queue.animations[animation_index];
				if (!animation.isPlaying())
					continue;
				animation.last_update_time = animation.timer;

				for (const AnimationComponent::AnimationChannel& channel : animation.channels)
				{
					assert(channel.sampler_index < (int)animation.samplers.size());
					const AnimationComponent::AnimationSampler& sampler = animation.samplers[channel.sampler_index];
					const Scene* data_scene = sampler.scene == nullptr ? this : (const Scene*)sampler.scene;
					const AnimationDataComponent* animationdata = data_scene->animation_datas.getComponent(sampler.data);
					if (animationdata == nullptr)
						continue;
					if (animationdata->keyframe_times.empty())
						continue;

					const AnimationComponent::AnimationChannel::PathDataType path_data_type = channel.getPathDataType();

					float timeFirst = std::numeric_limits<float>::max();
					float timeLast = std::numeric_limits<float>::min();
					int keyLeft = 0;	float timeLeft = std::numeric_limits<float>::min();
					int keyRight = 0;	float timeRight = std::numeric_limits<float>::max();

					// search for usable keyframes:
					for (int k = 0; k < (int)animationdata->keyframe_times.size(); ++k)
					{
						const float time = animationdata->keyframe_times[k];
						if (time < timeFirst)
						{
							timeFirst = time;
						}
						if (time > timeLast)
						{
							timeLast = time;
						}
						if (time <= animation.timer && time > timeLeft)
						{
							timeLeft = time;
							keyLeft = k;
						}
						if (time >= animation.timer && time < timeRight)
						{
							timeRight = time;
							keyRight = k;
						}
					}
					if (path_data_type != AnimationComponent::AnimationChannel::PathDataType::Event)
					{
						if (animation.timer < timeFirst)
						{
							// animation beginning haven't been reached, force first keyframe:
							timeLeft = timeFirst;
							timeRight = timeFirst;
							keyLeft = 0;
							keyRight = 0;
						}
					}
					else
					{
						timeLeft = std::max(timeLeft, timeFirst);
						timeRight = std::max(timeRight, timeLast);
					}

					const float left = animationdata->keyframe_times[keyLeft];
					const float right = animationdata->keyframe_times[keyRight];

					union Interpolator
					{
						XMFLOAT4 f4;
						XMFLOAT3 f3;
						XMFLOAT2 f2;
						float f;
					} interpolator = {};

					TransformComponent* target_transform = nullptr;
					MeshComponent* target_mesh = nullptr;

					CameraComponent* target_camera = nullptr;

					MaterialComponent* target_material = nullptr;

					if (
						channel.path == AnimationComponent::AnimationChannel::Path::TRANSLATION ||
						channel.path == AnimationComponent::AnimationChannel::Path::ROTATION ||
						channel.path == AnimationComponent::AnimationChannel::Path::SCALE
						)
					{
						target_transform = transforms.getComponent(channel.target);
						if (target_transform == nullptr)
							continue;
						switch (channel.path)
						{
						case AnimationComponent::AnimationChannel::Path::TRANSLATION:
							interpolator.f3 = target_transform->local_position;
							break;
						case AnimationComponent::AnimationChannel::Path::ROTATION:
							interpolator.f4 = target_transform->local_rotation;
							break;
						case AnimationComponent::AnimationChannel::Path::SCALE:
							interpolator.f3 = target_transform->local_scale;
							break;
						default:
							break;
						}
					}
					else if (channel.path == AnimationComponent::AnimationChannel::Path::WEIGHTS)
					{
						target_mesh = meshes.getComponent(channel.target);
						if (target_mesh == nullptr)
						{
							// Also try going through object's mesh reference:
							ObjectComponent* object = objects.getComponent(channel.target);
							if (object == nullptr)
								continue;
							target_mesh = meshes.getComponent(object->mesh_entity);
						}
						if (target_mesh == nullptr)
							continue;
					}
					else
					{
						assert(0);
						continue;
					}

					{
						// Path data interpolation:
						switch (sampler.mode)
						{
						default:
						case AnimationComponent::AnimationSampler::Mode::STEP:
						{
							// Nearest neighbor method:
							const int key = math::inverseLerp(timeLeft, timeRight, animation.timer) > 0.5f ? keyRight : keyLeft;
							switch (path_data_type)
							{
							default:
							case AnimationComponent::AnimationChannel::PathDataType::Float:
							{
								assert(animationdata->keyframe_data.size() == animationdata->keyframe_times.size());
								interpolator.f = animationdata->keyframe_data[key];
							}
							break;
							case AnimationComponent::AnimationChannel::PathDataType::Float2:
							{
								assert(animationdata->keyframe_data.size() == animationdata->keyframe_times.size() * 2);
								interpolator.f2 = ((const XMFLOAT2*)animationdata->keyframe_data.data())[key];
							}
							break;
							case AnimationComponent::AnimationChannel::PathDataType::Float3:
							{
								assert(animationdata->keyframe_data.size() == animationdata->keyframe_times.size() * 3);
								interpolator.f3 = ((const XMFLOAT3*)animationdata->keyframe_data.data())[key];
							}
							break;
							case AnimationComponent::AnimationChannel::PathDataType::Float4:
							{
								assert(animationdata->keyframe_data.size() == animationdata->keyframe_times.size() * 4);
								interpolator.f4 = ((const XMFLOAT4*)animationdata->keyframe_data.data())[key];
							}
							break;
							}
						}
						break;
						case AnimationComponent::AnimationSampler::Mode::LINEAR:
						{
							// Linear interpolation method:
							float t;
							if (keyLeft == keyRight)
							{
								t = 0;
							}
							else
							{
								t = (animation.timer - left) / (right - left);
							}
							t = saturate(t);

							switch (path_data_type)
							{
							default:
							case AnimationComponent::AnimationChannel::PathDataType::Float:
							{
								assert(animationdata->keyframe_data.size() == animationdata->keyframe_times.size());
								float vLeft = animationdata->keyframe_data[keyLeft];
								float vRight = animationdata->keyframe_data[keyRight];
								float vAnim = math::lerp(vLeft, vRight, t);
								interpolator.f = vAnim;
							}
							break;
							case AnimationComponent::AnimationChannel::PathDataType::Float2:
							{
								assert(animationdata->keyframe_data.size() == animationdata->keyframe_times.size() * 2);
								const XMFLOAT2* data = (const XMFLOAT2*)animationdata->keyframe_data.data();
								XMVECTOR vLeft = XMLoadFloat2(&data[keyLeft]);
								XMVECTOR vRight = XMLoadFloat2(&data[keyRight]);
								XMVECTOR vAnim = XMVectorLerp(vLeft, vRight, t);
								XMStoreFloat2(&interpolator.f2, vAnim);
							}
							break;
							case AnimationComponent::AnimationChannel::PathDataType::Float3:
							{
								assert(animationdata->keyframe_data.size() == animationdata->keyframe_times.size() * 3);
								const XMFLOAT3* data = (const XMFLOAT3*)animationdata->keyframe_data.data();
								XMVECTOR vLeft = XMLoadFloat3(&data[keyLeft]);
								XMVECTOR vRight = XMLoadFloat3(&data[keyRight]);
								XMVECTOR vAnim = XMVectorLerp(vLeft, vRight, t);
								XMStoreFloat3(&interpolator.f3, vAnim);
							}
							break;
							case AnimationComponent::AnimationChannel::PathDataType::Float4:
							{
								assert(animationdata->keyframe_data.size() == animationdata->keyframe_times.size() * 4);
								const XMFLOAT4* data = (const XMFLOAT4*)animationdata->keyframe_data.data();
								XMVECTOR vLeft = XMLoadFloat4(&data[keyLeft]);
								XMVECTOR vRight = XMLoadFloat4(&data[keyRight]);
								XMVECTOR vAnim;
								if (channel.path == AnimationComponent::AnimationChannel::Path::ROTATION)
								{
									vAnim = XMQuaternionSlerp(vLeft, vRight, t);
									vAnim = XMQuaternionNormalize(vAnim);
								}
								else
								{
									vAnim = XMVectorLerp(vLeft, vRight, t);
								}
								XMStoreFloat4(&interpolator.f4, vAnim);
							}
							break;
							}
						}
						break;
						case AnimationComponent::AnimationSampler::Mode::CUBICSPLINE:
						{
							// Cubic Spline interpolation method:
							float t;
							if (keyLeft == keyRight)
							{
								t = 0;
							}
							else
							{
								t = (animation.timer - left) / (right - left);
							}
							t = saturate(t);

							const float t2 = t * t;
							const float t3 = t2 * t;

							switch (path_data_type)
							{
							default:
							case AnimationComponent::AnimationChannel::PathDataType::Float:
							{
								assert(animationdata->keyframe_data.size() == animationdata->keyframe_times.size());
								float vLeft = animationdata->keyframe_data[keyLeft * 3 + 1];
								float vLeftTanOut = animationdata->keyframe_data[keyLeft * 3 + 2];
								float vRightTanIn = animationdata->keyframe_data[keyRight * 3 + 0];
								float vRight = animationdata->keyframe_data[keyRight * 3 + 1];
								float vAnim = (2 * t3 - 3 * t2 + 1) * vLeft + (t3 - 2 * t2 + t) * vLeftTanOut + (-2 * t3 + 3 * t2) * vRight + (t3 - t2) * vRightTanIn;
								interpolator.f = vAnim;
							}
							break;
							case AnimationComponent::AnimationChannel::PathDataType::Float2:
							{
								assert(animationdata->keyframe_data.size() == animationdata->keyframe_times.size() * 2 * 3);
								const XMFLOAT2* data = (const XMFLOAT2*)animationdata->keyframe_data.data();

								// 加载控制点数据
								XMVECTOR vLeft = XMLoadFloat2(&data[keyLeft * 3 + 1]);
								XMVECTOR vLeftTanOut = XMVectorScale(XMLoadFloat2(&data[keyLeft * 3 + 2]), dt);
								XMVECTOR vRightTanIn = XMVectorScale(XMLoadFloat2(&data[keyRight * 3 + 0]), dt);
								XMVECTOR vRight = XMLoadFloat2(&data[keyRight * 3 + 1]);

								// 计算三次样条插值
								XMVECTOR part1 = XMVectorScale(vLeft, (2 * t3 - 3 * t2 + 1));
								XMVECTOR part2 = XMVectorScale(vLeftTanOut, (t3 - 2 * t2 + t));
								XMVECTOR part3 = XMVectorScale(vRight, (-2 * t3 + 3 * t2));
								XMVECTOR part4 = XMVectorScale(vRightTanIn, (t3 - t2));

								XMVECTOR vAnim = XMVectorAdd(part1, XMVectorAdd(part2, XMVectorAdd(part3, part4)));
								XMStoreFloat2(&interpolator.f2, vAnim);
							}
							break;

							case AnimationComponent::AnimationChannel::PathDataType::Float3:
							{
								assert(animationdata->keyframe_data.size() == animationdata->keyframe_times.size() * 3 * 3);
								const XMFLOAT3* data = (const XMFLOAT3*)animationdata->keyframe_data.data();

								XMVECTOR vLeft = XMLoadFloat3(&data[keyLeft * 3 + 1]);
								XMVECTOR vLeftTanOut = XMVectorScale(XMLoadFloat3(&data[keyLeft * 3 + 2]), dt);
								XMVECTOR vRightTanIn = XMVectorScale(XMLoadFloat3(&data[keyRight * 3 + 0]), dt);
								XMVECTOR vRight = XMLoadFloat3(&data[keyRight * 3 + 1]);

								XMVECTOR part1 = XMVectorScale(vLeft, (2 * t3 - 3 * t2 + 1));
								XMVECTOR part2 = XMVectorScale(vLeftTanOut, (t3 - 2 * t2 + t));
								XMVECTOR part3 = XMVectorScale(vRight, (-2 * t3 + 3 * t2));
								XMVECTOR part4 = XMVectorScale(vRightTanIn, (t3 - t2));

								XMVECTOR vAnim = XMVectorAdd(part1, XMVectorAdd(part2, XMVectorAdd(part3, part4)));
								XMStoreFloat3(&interpolator.f3, vAnim);
							}
							break;

							case AnimationComponent::AnimationChannel::PathDataType::Float4:
							{
								assert(animationdata->keyframe_data.size() == animationdata->keyframe_times.size() * 4 * 3);
								const XMFLOAT4* data = (const XMFLOAT4*)animationdata->keyframe_data.data();

								XMVECTOR vLeft = XMLoadFloat4(&data[keyLeft * 3 + 1]);
								XMVECTOR vLeftTanOut = XMVectorScale(XMLoadFloat4(&data[keyLeft * 3 + 2]), dt);
								XMVECTOR vRightTanIn = XMVectorScale(XMLoadFloat4(&data[keyRight * 3 + 0]), dt);
								XMVECTOR vRight = XMLoadFloat4(&data[keyRight * 3 + 1]);

								XMVECTOR part1 = XMVectorScale(vLeft, (2 * t3 - 3 * t2 + 1));
								XMVECTOR part2 = XMVectorScale(vLeftTanOut, (t3 - 2 * t2 + t));
								XMVECTOR part3 = XMVectorScale(vRight, (-2 * t3 + 3 * t2));
								XMVECTOR part4 = XMVectorScale(vRightTanIn, (t3 - t2));

								XMVECTOR vAnim = XMVectorAdd(part1, XMVectorAdd(part2, XMVectorAdd(part3, part4)));

								if (channel.path == AnimationComponent::AnimationChannel::Path::ROTATION)
								{
									vAnim = XMQuaternionNormalize(vAnim);
								}
								XMStoreFloat4(&interpolator.f4, vAnim);
							}
							break;

							}
						}
						break;
						}
					}

					// The interpolated raw values will be blended on top of component values:
					const float t = animation.amount;

					// CheckIf this channel is the root motion bone or not.
					const bool isRootBone = (animation.isRootMotion() && animation.root_motion_bone != ecs::INVALID_ENTITY && (target_transform == transforms.getComponent(animation.root_motion_bone)));

					if (target_transform != nullptr)
					{
						target_transform->setDirty();

						switch (channel.path)
						{
						case AnimationComponent::AnimationChannel::Path::TRANSLATION:
						{
							const XMVECTOR aT = XMLoadFloat3(&target_transform->local_position);
							XMVECTOR bT = XMLoadFloat3(&interpolator.f3);

							const XMVECTOR T = XMVectorLerp(aT, bT, t);
							if (!isRootBone)
							{
								// Not root motion bone.
								XMStoreFloat3(&target_transform->local_position, T);
							}

						}
						break;
						case AnimationComponent::AnimationChannel::Path::ROTATION:
						{
							const XMVECTOR aR = XMLoadFloat4(&target_transform->local_rotation);
							XMVECTOR bR = XMLoadFloat4(&interpolator.f4);

							const XMVECTOR R = XMQuaternionSlerp(aR, bR, t);
							if (!isRootBone)
							{
								// Not root motion bone.
								XMStoreFloat4(&target_transform->local_rotation, R);
							}


						}
						break;
						case AnimationComponent::AnimationChannel::Path::SCALE:
						{
							const XMVECTOR aS = XMLoadFloat3(&target_transform->local_scale);
							XMVECTOR bS = XMLoadFloat3(&interpolator.f3);

							const XMVECTOR S = XMVectorLerp(aS, bS, t);
							XMStoreFloat3(&target_transform->local_scale, S);
						}
						break;
						default:
							break;
						}
					}







				}

				if (animation.timer > animation.end && animation.speed > 0)
				{
					if (animation.isLooped())
					{
						animation.timer = animation.start;
						for (auto& channel : animation.channels)
						{
							channel.next_event = 0;
						}
					}
					else
					{
						animation.timer = animation.end;
						animation.pause();
					}
				}
				else if (animation.timer < animation.start && animation.speed < 0)
				{
					if (animation.isLooped())
					{
						animation.timer = animation.end;
						for (auto& channel : animation.channels)
						{
							channel.next_event = 0;
						}
					}
					else
					{
						animation.timer = animation.start;
						animation.pause();
					}
				}

				if (animation.isPlaying())
				{
					animation.timer += dt * animation.speed;
					static float time_test = 0.f;
					time_test += dt * animation.speed;
					//std::cout << time_test << std::endl;
				}
			}
			});
		jobsystem::Wait(ctx);
	}

	void Scene::updateTransforms(jobsystem::context& ctx)
	{
		jobsystem::Dispatch(ctx, transforms.getCount(), small_subtask_groupsize, [&](jobsystem::JobArgs args) {
			TransformComponent& transform = transforms[args.jobIndex];
			transform.updateTransform();
			});
	}

	void Scene::updateArmatures(jobsystem::context& ctx)
	{
		jobsystem::Dispatch(ctx, (uint32_t)armatures.getCount(), 1, [&](jobsystem::JobArgs args) {

			ArmatureComponent& armature = armatures[args.jobIndex];
			Entity entity = armatures.getEntity(args.jobIndex);
			if (!transforms.contain(entity))
				return;
			const TransformComponent& transform = *transforms.getComponent(entity);

			// The transform world matrices are in world space, but skinning needs them in armature-local space, 
			//	so that the skin is reusable for instanced meshes.
			//	We remove the armature's world matrix from the bone world matrix to obtain the bone local transform
			//	These local bone matrices will only be used for skinning, the actual transform components for the bones
			//	remain unchanged.
			//
			//	This is useful for an other thing too:
			//	If a whole transform tree is transformed by some parent (even gltf import does that to convert from RH to LH space)
			//	then the inverseBindMatrices are not reflected in that because they are not contained in the hierarchy system. 
			//	But this will correct them too.
			XMMATRIX R = XMMatrixInverse(nullptr, XMLoadFloat4x4(&transform.world));

			armature.gpu_bone_offset = skinning_allocator.fetch_add(uint32_t(armature.bone_collection.size() * sizeof(ShaderTransform)));
			ShaderTransform* gpu_dst = (ShaderTransform*)((uint8_t*)skinning_upload_buffer_mapped + armature.gpu_bone_offset);

			if (armature.bone_data.size() != armature.bone_collection.size())
			{
				armature.bone_data.resize(armature.bone_collection.size());
			}

			XMFLOAT3 _min = XMFLOAT3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
			XMFLOAT3 _max = XMFLOAT3(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());

			uint32_t boneIndex = 0;
			for (Entity boneEntity : armature.bone_collection)
			{
				const TransformComponent* bone = transforms.getComponent(boneEntity);
				if (bone == nullptr)
					continue;

				XMMATRIX B = XMLoadFloat4x4(&armature.inverse_bind_matrices[boneIndex]);
				XMMATRIX W = XMLoadFloat4x4(&bone->world);
				XMMATRIX M = B * W * R;



				XMFLOAT4X4 mat;
				XMStoreFloat4x4(&mat, M);

				ShaderTransform& shadertransform = armature.bone_data[boneIndex];
				shadertransform.create(mat);
				if (skinning_upload_buffer_mapped != nullptr)
				{
					std::memcpy(gpu_dst + boneIndex, &shadertransform, sizeof(shadertransform));
				}

				const float bone_radius = 1;
				XMFLOAT3 bonepos = bone->getPosition();
				primitive::AABB boneAABB;
				boneAABB.createFromHalfWidth(bonepos, XMFLOAT3(bone_radius, bone_radius, bone_radius));
				_min = math::min(_min, boneAABB._min);
				_max = math::max(_max, boneAABB._max);

				boneIndex++;
			}

			armature.aabb = primitive::AABB(_min, _max);
			});
	}

	void Scene::updateHierarchy(jobsystem::context& ctx)
	{
		jobsystem::Dispatch(ctx, (uint32_t)hierarchy.getCount(), small_subtask_groupsize, [&](jobsystem::JobArgs args) {

			HierarchyComponent& hier = hierarchy[args.jobIndex];
			Entity entity = hierarchy.getEntity(args.jobIndex);

			TransformComponent* transform_child = transforms.getComponent(entity);
			XMMATRIX worldmatrix;
			if (transform_child != nullptr)
			{
				worldmatrix = transform_child->getLocalMatrix();
			}


			if (transform_child == nullptr)
				return;

			Entity parentID = hier.parent_id;
			while (parentID != INVALID_ENTITY)
			{
				TransformComponent* transform_parent = transforms.getComponent(parentID);
				if (transform_child != nullptr && transform_parent != nullptr)
				{
					worldmatrix *= transform_parent->getLocalMatrix();
				}


				const HierarchyComponent* hier_recursive = hierarchy.getComponent(parentID);
				if (hier_recursive != nullptr)
				{
					parentID = hier_recursive->parent_id;
				}
				else
				{
					parentID = INVALID_ENTITY;
				}
			}

			if (transform_child != nullptr)
			{
				XMStoreFloat4x4(&transform_child->world, worldmatrix);
			}

			});
	}

	void Scene::updateShaderScene()
	{
		RHI* rhi = rhi::getRHI();
		shader_scene.instance_buffer = rhi->getDescriptorIndex(&instance_buffer, SubresourceType::SRV);
		shader_scene.geometry_buffer = rhi->getDescriptorIndex(&geometry_buffer, SubresourceType::SRV);
		shader_scene.material_buffer = rhi->getDescriptorIndex(&material_buffer, SubresourceType::SRV);
	}

}
