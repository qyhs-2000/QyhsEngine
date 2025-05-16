#pragma once
#include "scene_component.h"
#include "function/framework/component/ecs.h"
#include "core/jobsystem.h"
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
		
		void updateObjects(jobsystem::Context &ctx);
		void updateMeshes(jobsystem::Context& ctx);
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
		void update(float delta_time);
		std::vector<uint32_t>visible_objects;
		std::vector<OccludedResult> occlusion_result_objects;
		void updateShaderScene();
		uint32_t scene_instance_count = 0;
		uint32_t materials_array_size = 0;
		uint32_t geometry_array_size = 0;
		graphics::GPUBuffer instance_upload_buffers[RHI::getBufferCount()];
		graphics::GPUBuffer material_upload_buffers[RHI::getBufferCount()];
		graphics::GPUBuffer geometry_upload_buffers[RHI::getBufferCount()];
		graphics::GPUBuffer instance_buffer;
		graphics::GPUBuffer material_buffer;
		graphics::GPUBuffer geometry_buffer;
		ShaderMeshInstance* instance_upload_buffer_mapped = nullptr;
		ShaderMaterial* material_upload_buffer_mapped = nullptr;
		ShaderGeometry * geometry_upload_buffer_mapped = nullptr;
		std::vector<primitive::AABB> aabb_objects;
	private:
		std::atomic<uint32_t> geometry_allocator{ 0 };
	};

	Scene* getScene();

}