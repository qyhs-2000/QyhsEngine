#pragma once
#include "scene_component.h"
#include "function/framework/component/ecs.h"
#include "core/jobsystem.h"
#include "function/file/archive.h"
namespace qyhs::scene
{
	class Scene
	{
	public:
		ecs::ComponentLibrary component_library;
		ecs::ComponentManager<scene::MeshComponent>& meshes = component_library.Register<MeshComponent>("qyhs::scene::Scene::meshes");
		ecs::ComponentManager<scene::ObjectComponent>& objects = component_library.Register<ObjectComponent>("qyhs::scene::Scene::objects");
		ecs::ComponentManager<scene::TransformComponent>& transforms = component_library.Register<scene::TransformComponent>("qyhs::scene::Scene::transforms");
		ecs::ComponentManager<scene::NameComponent>& names = component_library.Register<scene::NameComponent>("qyhs::scene::Scene::names");
		ecs::ComponentManager<scene::MaterialComponent>& materials = component_library.Register<scene::MaterialComponent>("qyhs::scene::Scene::materials");
		ecs::ComponentManager<scene::HierarchyComponent>& hierarchy = component_library.Register<scene::HierarchyComponent>("qyhs::scene::Scene::hierarchy");
		ecs::ComponentManager<scene::ArmatureComponent>& armatures = component_library.Register<scene::ArmatureComponent>("qyhs::scene::Scene::armatures");
		ecs::ComponentManager<scene::AnimationComponent>& animations = component_library.Register<scene::AnimationComponent>("qyhs::scene::Scene::animations");
		ecs::ComponentManager<scene::AnimationDataComponent>& animation_datas = component_library.Register<scene::AnimationDataComponent>("qyhs::scene::Scene::animation_datas");
		struct OccludedResult
		{
			bool isOccluded()const
			{
				return false;
			}
		};
		void attachComponent(ecs::Entity entity, ecs::Entity parent, bool child_is_in_local_space = false)
		{
			assert(entity != parent);
			if (hierarchy.contain(entity))
			{
				detachComponent(entity);
			}
			scene::HierarchyComponent& parent_component = hierarchy.create(entity);
			parent_component.parent_id = parent;

			TransformComponent* transform_parent = transforms.getComponent(parent);
			TransformComponent* transform_child = transforms.getComponent(entity);
			if (transform_parent != nullptr && transform_child != nullptr)
			{
				if (!child_is_in_local_space)
				{
					XMMATRIX parent_inv = XMMatrixInverse(nullptr, XMLoadFloat4x4(&transform_parent->world));
					transform_child->MatrixTransform(parent_inv);
					transform_child->updateTransform();
				}
				transform_child->updateTransformParented(transform_parent);
			}
		}
		struct AnimationQueue
		{
			std::unordered_set<ecs::Entity> entities;
			std::vector<AnimationComponent*> animations;
		};
		jobsystem::context animation_dependency_scan_workload;
		std::vector<AnimationQueue> animation_queues;
		
		void scanAnimationDepedencies();
		void Component_DetachChildren(ecs::Entity entity);
		void Entity_Remove(ecs::Entity entity,bool recursive = true);
		
		void serialize(Archive& archive);
		void merge(Scene& other);
		void updateObjects(jobsystem::context &ctx);
		void updateMeshes(jobsystem::context& ctx);
		void updateMaterials(jobsystem::context& ctx);
		void detachComponent(ecs::Entity entity)
		{
			const HierarchyComponent* parent = hierarchy.getComponent(entity);
			if (parent != nullptr)
			{
				TransformComponent* transform = transforms.getComponent(entity);
				if (transform != nullptr)
				{
					transform->applyWorldToLocal();
				}
				hierarchy.remove(entity);
			}
		}
		ecs::Entity createMaterialEntity(const std::string& name);
		ecs::Entity createObjectEntity(const std::string& name);
		ecs::Entity createMeshEntity(const std::string& name);
		ShaderScene shader_scene;
		size_t animation_queue_count = 0;
		float dt = 0;
		void update(float delta_time);
		void updateAnimations(jobsystem::context& ctx);
		void updateTransforms(jobsystem::context &ctx);
		void updateArmatures(jobsystem::context& ctx);
		void updateHierarchy(jobsystem::context& ctx);
		std::vector<OccludedResult> occlusion_result_objects;
		void updateShaderScene();
		
		uint32_t scene_instance_count = 0;
		uint32_t materials_array_size = 0;
		uint32_t geometry_array_size = 0;
		uint32_t skinning_data_size = 0;
		graphics::GPUBuffer instance_upload_buffers[RHI::getBufferCount()];
		graphics::GPUBuffer material_upload_buffers[RHI::getBufferCount()];
		graphics::GPUBuffer geometry_upload_buffers[RHI::getBufferCount()];
		graphics::GPUBuffer skinning_upload_buffers[RHI::getBufferCount()];
		graphics::GPUBuffer instance_buffer;
		graphics::GPUBuffer material_buffer;
		graphics::GPUBuffer geometry_buffer;
		graphics::GPUBuffer skinning_buffer;
		ShaderMeshInstance* instance_upload_buffer_mapped = nullptr;
		ShaderMaterial* material_upload_buffer_mapped = nullptr;
		ShaderGeometry * geometry_upload_buffer_mapped = nullptr;
		void* skinning_upload_buffer_mapped = nullptr;
		std::vector<primitive::AABB> aabb_objects;
		std::vector<XMFLOAT4X4> object_matrices;
	private:
		std::atomic<uint32_t> geometry_allocator{ 0 };
		std::atomic<uint32_t> skinning_allocator{ 0 };
	};

	Scene* getScene();
	ecs::Entity loadModel(Scene& scene, const std::string& fileName, const XMMATRIX& transformMatrix = XMMatrixIdentity(), bool attached = false);
	void LoadModel2(Scene& scene, const std::string& fileName, const XMMATRIX& transformMatrix = XMMatrixIdentity(), ecs::Entity rootEntity = ecs::INVALID_ENTITY);
}