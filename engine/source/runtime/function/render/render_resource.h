#pragma once
#include "render_structure.h"
#include "render_entity.h"
#include <unordered_map>
#include <stdexcept>
#include "render_guid_allocator.h"
#include "render_type.h"
#include "rhi/rhi.h"
namespace QYHS
{
	class RenderResource
	{
	public:
		VulkanMaterial& getEntityMaterial(RenderEntity& entity);
		VulkanMesh& getEntityMesh(RenderEntity& entity);
		void uploadGameObjectRenderResource(std::shared_ptr<RHI> rhi,RenderEntity render_entity, RenderMeshData mesh_data);
		void uploadGameObjectRenderResource(std::shared_ptr<RHI> rhi, RenderEntity render_entity, RenderMaterialData material_data);
		RenderGUIDAllocator<MeshSourceDesc>& getMeshAssetIdAllocator();
		RenderMeshData loadMeshData(MeshSourceDesc& mesh_source);
		StaticMeshData loadStaticMesh(const std::string& mesh_file);
		VulkanMaterial& getOrCreateVulkanMaterial(std::shared_ptr<RHI> rhi, RenderEntity render_entity, RenderMaterialData material_data);
		VulkanMesh& getOrCreateVulkanMesh(std::shared_ptr<RHI> rhi, RenderEntity render_entity, RenderMeshData mesh_data);
		void updateMeshData(std::shared_ptr<RHI> rhi, uint32_t index_buffer_size, void* index_buffer, uint32_t vertex_buffer_size, MeshVertexDataDefinition* vertex_buffer, VulkanMesh& mesh);
		void updateVertexBuffer(std::shared_ptr<RHI> rhi, uint32_t vertex_buffer_size, const void* vertex_buffer, VulkanMesh& mesh);
		void updateIndexBuffer(std::shared_ptr<RHI> rhi, uint32_t index_buffer_size, const void* index_buffer, VulkanMesh& mesh);
		void updatePerframeBuffer(std::shared_ptr<class RenderCamera> camera);
		std::shared_ptr<TextureData> loadTexture(const std::string& material_file);
		RenderMaterialData loadMaterialData(MaterialSourceDesc& material_source);
		VkDescriptorSetLayout* m_material_descriptor_set_layout;
	private:
		std::unordered_map<size_t, VulkanMaterial> m_materials;
		std::unordered_map<size_t, VulkanMesh>		 m_meshes;
		RenderGUIDAllocator<MeshSourceDesc> m_mesh_asset_allocator;
	};
}