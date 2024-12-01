#pragma once
#include "render_structure.h"
#include "render_entity.h"
#include <unordered_map>
#include <stdexcept>
#include "render_guid_allocator.h"
#include "render_type.h"
#include "rhi/rhi.h"
#include "render_resource_base.h"
#include "render_common.h"
#include "vulkanmemoryallocator/include/vk_mem_alloc.h"

namespace QYHS
{
	struct StorageBuffer
	{
		VkBuffer global_ringbuffer;
		VkDeviceMemory global_ringbuffer_memory;
		void* global_ringbuffer_memory_pointer;

		std::vector<uint32_t> ringbuffer_begin;
		std::vector<uint32_t> ringbuffer_end;
		std::vector<uint32_t> ringbuffer_size;

		VkBuffer axis_storage_buffer;
		VkDeviceMemory axis_storage_buffer_memory;
		void * axis_storage_buffer_memory_pointer;

		uint32_t min_uniform_buffer_offset_alignment{ 256 };
		uint32_t min_storage_buffer_offset_alignment{ 256 };
		uint32_t max_storage_buffer_size{ 1 << 27 };
	};

	struct IBLResource
	{
		//TODO:study what is specular and what is brdf
		VkImage specular_texture_image;
		VkImageView specular_texture_image_view;
		VkSampler specular_texture_image_sampler;
		VmaAllocation specular_texture_image_allocation;
	};
	
	struct GlobalRenderResource
	{
		StorageBuffer storage_buffer;
		IBLResource ibl_resource;
	};

	class RenderResource:public RenderResourceBase
	{
	public:
		VulkanMaterial& getEntityMaterial(RenderEntity& entity);
		VulkanMesh& getEntityMesh(RenderEntity& entity);
		void uploadGameObjectRenderResource(std::shared_ptr<RHI> rhi,RenderEntity render_entity, RenderMeshData mesh_data);
		void uploadGameObjectRenderResource(std::shared_ptr<RHI> rhi, RenderEntity render_entity, RenderMaterialData material_data);
		RenderGUIDAllocator<MeshSourceDesc>& getMeshAssetIdAllocator();
		RenderMeshData loadMeshData(MeshSourceDesc& mesh_source);
		VulkanMaterial& getOrCreateVulkanMaterial(std::shared_ptr<RHI> rhi, RenderEntity render_entity, RenderMaterialData material_data);
		VulkanMesh& getOrCreateVulkanMesh(std::shared_ptr<RHI> rhi, RenderEntity render_entity, RenderMeshData mesh_data);
		void updateMeshData(std::shared_ptr<RHI> rhi, uint32_t index_buffer_size, void* index_buffer, uint32_t vertex_buffer_size, MeshVertexDataDefinition* vertex_buffer, VulkanMesh& mesh);
		void updateVertexBindingData(std::shared_ptr<RHI> rhi,uint32_t joint_buffer_size, MeshVertexBindingDataDefinition* joint_buffer_data, uint32_t index_count,VulkanMesh & mesh);
		void updateVertexBuffer(std::shared_ptr<RHI> rhi, uint32_t vertex_buffer_size, MeshVertexDataDefinition const * vertex_buffer, VulkanMesh& mesh);
		void updateIndexBuffer(std::shared_ptr<RHI> rhi, uint32_t index_buffer_size, const void* index_buffer, VulkanMesh& mesh);
		void updatePerframeBuffer(std::shared_ptr<class RenderCamera> camera);
		std::shared_ptr<TextureData> loadTexture(const std::string& material_file);
		RenderMaterialData loadMaterialData(MaterialSourceDesc& material_source);
		VkDescriptorSetLayout* m_material_descriptor_set_layout;
		VkDescriptorSetLayout* m_per_mesh_descriptor_set_layout;
		void uploadGlobalRenderResource(std::shared_ptr<RHI> rhi,LevelResourceDesc & level_resource_desc);
		void createAndMapStorageBuffer(std::shared_ptr<RHI> rhi);
		void resetRingBufferOffset(uint32_t current_frame_index);
		void uploadIBLResource(std::shared_ptr<RHI> rhi,LevelResourceDesc level_resource_desc);
		void createIBLTexture(std::shared_ptr<RHI> rhi,std::array<std::shared_ptr<TextureData>,6 > &specular_map);
		void createIBLSampler(std::shared_ptr<RHI> rhi);
		std::shared_ptr<TextureData> loadTextureHDR(const std::string& material_file,int desired_channels=4);
		GlobalRenderResource m_global_render_resource;
		MeshPerFrameStorageBufferObject m_mesh_per_frame_storage_buffer_object;
		MeshInefficientPickPerFrameStorageBuffer m_mesh_inefficient_pick_perframe_storage_buffer_object;
		
	private:
		std::unordered_map<size_t, VulkanMaterial> m_materials;
		std::unordered_map<size_t, VulkanMesh>		 m_meshes;
		RenderGUIDAllocator<MeshSourceDesc> m_mesh_asset_allocator;
	};
}