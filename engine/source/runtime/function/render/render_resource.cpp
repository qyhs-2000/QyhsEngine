#include "render_resource.h"
#include <function/render/render_entity.h>
#include "function/global/global_context.h"
#include <filesystem>
#include "resource/asset_manager/asset_manager.h"
#include "resource/type/data/mash_data.h"
#include "core/math/vector2.h"
#include "function/render/rhi/vulkan/vulkan_rhi.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "core/utils/model_importer_obj.h"
#include "render_camera.h"
#include <iostream>
namespace QYHS
{
	VulkanMaterial& RenderResource::getEntityMaterial(RenderEntity& entity)
	{
		size_t asset_id = entity.material_asset_id;
		if (m_materials.find(asset_id) != m_materials.end())
		{
			return m_materials[asset_id];
		}
		else
		{
			throw std::runtime_error("falied to get entity material!");
		}
	}
	VulkanMesh& RenderResource::getEntityMesh(RenderEntity& entity)
	{
		size_t asset_id = entity.mesh_asset_id;
		if (m_meshes.find(asset_id) != m_meshes.end())
		{
			return m_meshes[asset_id];
		}
		else
		{
			throw std::runtime_error("falied to get entity mesh!");
		}
	}
	void RenderResource::uploadGameObjectRenderResource(std::shared_ptr<RHI> rhi, size_t asset_id, RenderMeshData mesh_data)
	{
		getOrCreateVulkanMesh(rhi, asset_id, mesh_data);
	}

	void RenderResource::uploadGameObjectRenderResource(std::shared_ptr<RHI> rhi, size_t material_asset_id, RenderMaterialData material_data)
	{
		getOrCreateVulkanMaterial(rhi, material_asset_id, material_data);
	}
	RenderGUIDAllocator<MeshSourceDesc>& RenderResource::getMeshAssetIdAllocator()
	{
		return m_mesh_asset_allocator;
	}
	RenderMeshData RenderResource::loadMeshData(MeshSourceDesc& mesh_source)
	{
		RenderMeshData ret;
		std::shared_ptr<AssetManager> asset_manager = g_runtime_global_context.m_asset_manager;
		assert(asset_manager && "failed to get asset manager");
		if (std::filesystem::path(mesh_source.mesh_source_file).extension() == ".obj")
		{
			ret.m_static_mesh_data = loadStaticMesh(mesh_source.mesh_source_file);
		}
		if (std::filesystem::path(mesh_source.mesh_source_file).extension() == ".json")
		{
			std::shared_ptr<MeshData> bind_data = std::make_shared<MeshData>();
			asset_manager->loadAsset(mesh_source.mesh_source_file, *bind_data);

			size_t vertex_buffer_size = sizeof(MeshVertexDataDefinition) * bind_data->vertex_buffer.size();
			ret.m_static_mesh_data.m_vertex_buffer = std::make_shared<BufferData>(vertex_buffer_size);
			//vertex buffer
			MeshVertexDataDefinition* vertex = (MeshVertexDataDefinition*)ret.m_static_mesh_data.m_vertex_buffer->m_data;
			for (size_t i = 0; i < bind_data->vertex_buffer.size(); ++i)
			{
				vertex[i].x  = bind_data->vertex_buffer[i].px;
                vertex[i].y  = bind_data->vertex_buffer[i].py;
                vertex[i].z  = bind_data->vertex_buffer[i].pz;
                vertex[i].nx = bind_data->vertex_buffer[i].nx;
                vertex[i].ny = bind_data->vertex_buffer[i].ny;
                vertex[i].nz = bind_data->vertex_buffer[i].nz;
                vertex[i].tx = bind_data->vertex_buffer[i].tx;
                vertex[i].ty = bind_data->vertex_buffer[i].ty;
                vertex[i].tz = bind_data->vertex_buffer[i].tz;
                vertex[i].u  = bind_data->vertex_buffer[i].u;
                vertex[i].v  = bind_data->vertex_buffer[i].v;
			}
			//index buffer
			size_t index_buffer_size = sizeof(uint16_t) * bind_data->index_buffer.size();
			ret.m_static_mesh_data.m_index_buffer = std::make_shared<BufferData>(index_buffer_size);
			uint16_t *index = (uint16_t*)ret.m_static_mesh_data.m_index_buffer->m_data;
			for (size_t i = 0; i < bind_data->index_buffer.size(); ++i)
			{
				index[i] = static_cast<uint16_t>(bind_data->index_buffer[i]);
			}
			//skeleton blending buffer
			size_t vertex_bindings_buffer_size = sizeof(MeshVertexBindingDataDefinition) * bind_data->bind.size();
			ret.m_skeleton_binding_buffer = std::make_shared<BufferData>(vertex_bindings_buffer_size);
			MeshVertexBindingDataDefinition* skeleton_binding = (MeshVertexBindingDataDefinition*)ret.m_skeleton_binding_buffer->m_data;
			for (size_t i = 0; i < bind_data->bind.size(); ++i)
			{
				skeleton_binding[i].index0 = bind_data->bind[i].index0;
				skeleton_binding[i].index1 = bind_data->bind[i].index1;
				skeleton_binding[i].index2 = bind_data->bind[i].index2;
				skeleton_binding[i].index3 = bind_data->bind[i].index3;

				skeleton_binding[i].weight0 = bind_data->bind[i].weight0;
				skeleton_binding[i].weight1 = bind_data->bind[i].weight1;
				skeleton_binding[i].weight2 = bind_data->bind[i].weight2;
				skeleton_binding[i].weight3 = bind_data->bind[i].weight3;
			}
			
		}

		return ret;
	}
	
	std::shared_ptr<TextureData> RenderResource::loadTexture(const std::string& material_file)
	{
		std::shared_ptr<AssetManager> asset_manager = g_runtime_global_context.m_asset_manager;
		assert(asset_manager && "failed to get asset_manager");
		std::shared_ptr<TextureData> texture_data = std::make_shared<TextureData>();
		int tex_width, tex_height, tex_channels;

		texture_data->m_pixels = stbi_load(asset_manager->getFullPath(material_file).generic_string().c_str(), &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
		texture_data->m_width = tex_width;
		texture_data->m_height = tex_height;
		return texture_data;
	}

	std::shared_ptr<TextureData> RenderResource::loadTextureHDR(const std::string& material_file,int desired_channels)
	{
		std::shared_ptr<AssetManager> asset_manager = g_runtime_global_context.m_asset_manager;
		assert(asset_manager && "failed to get asset_manager");
		std::shared_ptr<TextureData> texture_data = std::make_shared<TextureData>();
		int tex_width, tex_height, tex_channels;

		texture_data->m_pixels = stbi_loadf(asset_manager->getFullPath(material_file).generic_string().c_str(), &tex_width, &tex_height, &tex_channels, desired_channels);
		texture_data->m_width = tex_width;
		texture_data->m_height = tex_height;
		switch (desired_channels)
        {
            case 2:
                texture_data->m_format = PIXEL_FORMAT::FORMAT_R32G32_FLOAT;
                break;
            case 4:
                texture_data->m_format = PIXEL_FORMAT::FORMAT_R32G32B32A32_FLOAT;
                break;
            default:
                // three component format is not supported in some vulkan driver implementations
                throw std::runtime_error("unsupported channels number");
                break;
        }
		return texture_data;
	}

	Material_ID RenderResource::createNewMaterial()
	{

		return Material_ID();
	}

	RenderMaterialData RenderResource::loadMaterialData(MaterialSourceDesc& material_source)
	{
		RenderMaterialData material_data;
		material_data.m_base_color_texture = loadTexture(material_source.m_base_color_file);

		return material_data;
	}

	void RenderResource::uploadGlobalRenderResource(std::shared_ptr<RHI> rhi,LevelResourceDesc & level_resource_desc)
	{
		createAndMapStorageBuffer(rhi);
		uploadIBLResource(rhi,level_resource_desc);
	}

	//TODO:destroy global ring buffer and axis_storage_buffer when application is closed
	void RenderResource::createAndMapStorageBuffer(std::shared_ptr<RHI> rhi)
	{
		//global ring buffer
		VulkanRHI* m_rhi = static_cast<VulkanRHI*>(rhi.get());
		StorageBuffer& global_storage_buffer = m_global_render_resource.storage_buffer;
		uint32_t global_storage_buffer_size = 1024 * 1024 * 128;
		m_rhi->createBuffer(m_rhi->getPhysicalDevice(),m_rhi->getDevice(),global_storage_buffer_size,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			global_storage_buffer.global_ringbuffer,global_storage_buffer.global_ringbuffer_memory);
		
		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(m_rhi->getPhysicalDevice(), &properties);
		global_storage_buffer.min_storage_buffer_offset_alignment = static_cast<uint32_t>(properties.limits.minStorageBufferOffsetAlignment);
		global_storage_buffer.max_storage_buffer_size = static_cast<uint32_t>(properties.limits.maxStorageBufferRange);
		global_storage_buffer.min_uniform_buffer_offset_alignment = static_cast<uint32_t>(properties.limits.minUniformBufferOffsetAlignment);
		
		int max_frame_in_flight = m_rhi->getMaxFrameInFlight();
		global_storage_buffer.ringbuffer_begin.resize(max_frame_in_flight);
		global_storage_buffer.ringbuffer_end.resize(max_frame_in_flight);
		global_storage_buffer.ringbuffer_size.resize(max_frame_in_flight);
		for (int i = 0; i < max_frame_in_flight; ++i)
		{
			global_storage_buffer.ringbuffer_begin[i] = global_storage_buffer_size * i / max_frame_in_flight;
			global_storage_buffer.ringbuffer_size[i] = global_storage_buffer_size * (i + 1) / max_frame_in_flight 
															- global_storage_buffer_size * i / max_frame_in_flight;

		}

		vkMapMemory(m_rhi->getDevice(), global_storage_buffer.global_ringbuffer_memory, 0, VK_WHOLE_SIZE, 0, &global_storage_buffer.global_ringbuffer_memory_pointer);

		//axis storage buffer
		m_rhi->createBuffer(m_rhi->physical_device,m_rhi->m_device,sizeof(AxisStorageBufferObject),VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,m_global_render_resource.storage_buffer.axis_storage_buffer,m_global_render_resource.storage_buffer.axis_storage_buffer_memory);

		vkMapMemory(m_rhi->m_device,global_storage_buffer.axis_storage_buffer_memory,0,VK_WHOLE_SIZE,0,&global_storage_buffer.axis_storage_buffer_memory_pointer);
	}

	void RenderResource::uploadIBLResource(std::shared_ptr<RHI> rhi,LevelResourceDesc level_resource_desc)
	{
		SkyBoxSpecularMap sky_box_specular_map = level_resource_desc.m_ibl_resource_desc.m_skybox_specular_map;
		std::shared_ptr<TextureData> specular_pos_x_map = loadTextureHDR(sky_box_specular_map.m_positive_x_map);
		std::shared_ptr<TextureData> specular_neg_x_map = loadTextureHDR(sky_box_specular_map.m_negative_x_map);
		std::shared_ptr<TextureData> specular_pos_y_map = loadTextureHDR(sky_box_specular_map.m_positive_y_map);
		std::shared_ptr<TextureData> specular_neg_y_map = loadTextureHDR(sky_box_specular_map.m_negative_y_map);
		std::shared_ptr<TextureData> specular_pos_z_map = loadTextureHDR(sky_box_specular_map.m_positive_z_map);
		std::shared_ptr<TextureData> specular_neg_z_map = loadTextureHDR(sky_box_specular_map.m_negative_z_map);

		createIBLSampler(rhi);

		std::array<std::shared_ptr<TextureData>, 6> specular_map = {
			specular_pos_x_map,
			specular_neg_x_map,
			specular_pos_z_map,
			specular_neg_z_map,
			specular_pos_y_map,
			specular_neg_y_map
		};

		createIBLTexture(rhi,specular_map);

	}
	
	void RenderResource::createIBLTexture(std::shared_ptr<RHI> rhi,std::array<std::shared_ptr<TextureData>, 6 > &specular_maps)
	{
		uint32_t specular_cubemaps_miplevels = static_cast<uint32_t>(std::log2(std::max(specular_maps[0]->m_width, specular_maps[0]->m_height)) + 1);
		VulkanRHI* vulkan_rhi = static_cast<VulkanRHI*>(rhi.get());
		vulkan_rhi->createCubeMap(rhi.get(), m_global_render_resource.ibl_resource.specular_texture_image,
			m_global_render_resource.ibl_resource.specular_texture_image_view,
			m_global_render_resource.ibl_resource.specular_texture_image_allocation,
			specular_maps[0]->m_width, specular_maps[0]->m_height,
			{ specular_maps[0]->m_pixels,
			specular_maps[1]->m_pixels,
			specular_maps[2]->m_pixels,
			specular_maps[3]->m_pixels,
			specular_maps[4]->m_pixels,
			specular_maps[5]->m_pixels },
			specular_maps[0]->m_format,
			specular_cubemaps_miplevels
		);

	}

	void RenderResource::createIBLSampler(std::shared_ptr<RHI> rhi)
	{
		VulkanRHI* vulkan_rhi = static_cast<VulkanRHI*>(rhi.get());
		VkPhysicalDeviceProperties physical_device_properties{};
		vkGetPhysicalDeviceProperties(vulkan_rhi->getPhysicalDevice(), &physical_device_properties);

		VkSamplerCreateInfo specular_sampler_create_info{};
		specular_sampler_create_info.sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        specular_sampler_create_info.magFilter               = VK_FILTER_LINEAR;
        specular_sampler_create_info.minFilter               = VK_FILTER_LINEAR;
        specular_sampler_create_info.addressModeU            = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        specular_sampler_create_info.addressModeV            = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        specular_sampler_create_info.addressModeW            = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        specular_sampler_create_info.anisotropyEnable        = VK_TRUE;                                                // close:false
        specular_sampler_create_info.maxAnisotropy           = physical_device_properties.limits.maxSamplerAnisotropy; // close :1.0f
        specular_sampler_create_info.borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        specular_sampler_create_info.unnormalizedCoordinates = VK_FALSE;
        specular_sampler_create_info.compareEnable           = VK_FALSE;
        specular_sampler_create_info.compareOp               = VK_COMPARE_OP_ALWAYS;
        specular_sampler_create_info.mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        specular_sampler_create_info.maxLod                  = 0.0f;

		if (m_global_render_resource.ibl_resource.specular_texture_image_sampler != VK_NULL_HANDLE)
		{
			vkDestroySampler(vulkan_rhi->getDevice(), m_global_render_resource.ibl_resource.specular_texture_image_sampler, nullptr);
		}
		if (vkCreateSampler(vulkan_rhi->getDevice(), &specular_sampler_create_info, nullptr, &m_global_render_resource.ibl_resource.specular_texture_image_sampler) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create specular sampler!");
		}
	}

	void RenderResource::resetRingBufferOffset(uint32_t current_frame_index)
	{
		m_global_render_resource.storage_buffer.ringbuffer_end[current_frame_index] = m_global_render_resource.storage_buffer.ringbuffer_begin[current_frame_index];
	}

	VulkanMesh& RenderResource::getOrCreateVulkanMesh(std::shared_ptr<RHI> rhi, size_t asset_id, RenderMeshData mesh_data)
	{
		auto iter = m_meshes.find(asset_id);
		if (iter != m_meshes.end())
		{
			return iter->second;
		}
		else
		{
			VulkanMesh tmp;
			const auto insert_result = m_meshes.insert(std::make_pair(asset_id, std::move(tmp)));
			assert(insert_result.second);

			VulkanMesh& mesh = insert_result.first->second;
			uint32_t index_buffer_size = (uint32_t)mesh_data.m_static_mesh_data.m_index_buffer->m_size;
			void* index_buffer = mesh_data.m_static_mesh_data.m_index_buffer->m_data;
			uint32_t vertex_buffer_size = (uint32_t)mesh_data.m_static_mesh_data.m_vertex_buffer->m_size;
			MeshVertexDataDefinition* vertex_buffer = reinterpret_cast<MeshVertexDataDefinition*>(mesh_data.m_static_mesh_data.m_vertex_buffer->m_data);
			updateMeshData(rhi, index_buffer_size, index_buffer, vertex_buffer_size, vertex_buffer, mesh);
			static_cast<VulkanRHI*>(rhi.get())->allocateDescriptorSets(m_per_mesh_descriptor_set_layout, 1, mesh.p_vertex_blending_descriptor_set);
			if (mesh_data.m_skeleton_binding_buffer)
			{
				uint32_t joint_buffer_size = mesh_data.m_skeleton_binding_buffer->m_size;
				uint32_t index_count = index_buffer_size / sizeof(uint16_t);
				MeshVertexBindingDataDefinition* joint_binding_buffer_data = reinterpret_cast<MeshVertexBindingDataDefinition*>(mesh_data.m_skeleton_binding_buffer->m_data);
				updateVertexBindingData(rhi,joint_buffer_size,joint_binding_buffer_data,index_count,mesh);
			}

			return mesh;
		}
	}
	VulkanMaterial& RenderResource::getOrCreateVulkanMaterial(std::shared_ptr<RHI> rhi, 
								size_t material_asset_id, RenderMaterialData material_data)
	{
		auto iter = m_materials.find(material_asset_id);
		if (iter != m_materials.end())
		{
			return iter->second;
		}
		else
		{
			VulkanRHI* vulkan_rhi = static_cast<VulkanRHI*>(rhi.get());
			assert(vulkan_rhi);
			VulkanMaterial tmp;
			const auto insert_result = m_materials.insert(std::make_pair(material_asset_id, std::move(tmp)));
			assert(insert_result.second);
			VulkanMaterial& material = insert_result.first->second;
			VkPhysicalDevice physical_device = vulkan_rhi->getPhysicalDevice();
			VkDevice device = vulkan_rhi->getDevice();

			VkDeviceSize imageSize = material_data.m_base_color_texture->m_width * material_data.m_base_color_texture->m_height * 4;
			unsigned int mip_levels = std::floor(std::log2(std::max(material_data.m_base_color_texture->m_width, material_data.m_base_color_texture->m_height))) + 1;

			VkBuffer stagingBuffer;
			VkDeviceMemory stagingBufferMemory;
			vulkan_rhi->createBuffer(physical_device, device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

			void* data;
			vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
			memcpy(data, material_data.m_base_color_texture->m_pixels, static_cast<size_t>(imageSize));
			vkUnmapMemory(device, stagingBufferMemory);

			vulkan_rhi->createImage(physical_device, device, material_data.m_base_color_texture->m_width, material_data.m_base_color_texture->m_height, VK_FORMAT_R8G8B8A8_SRGB, mip_levels, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, material.base_color_texture_image, material.base_color_texture_image_memory);

			vulkan_rhi->transitionImageLayout(vulkan_rhi, material.base_color_texture_image, VK_FORMAT_R8G8B8A8_SRGB, 1,mip_levels, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
			vulkan_rhi->copyBufferToImage(vulkan_rhi, stagingBuffer, material.base_color_texture_image, static_cast<uint32_t>(material_data.m_base_color_texture->m_width), static_cast<uint32_t>(material_data.m_base_color_texture->m_height),1);

			vkDestroyBuffer(device, stagingBuffer, nullptr);
			vkFreeMemory(device, stagingBufferMemory, nullptr);
			vulkan_rhi->generateMipmaps(vulkan_rhi, material.base_color_texture_image, VK_FORMAT_R8G8B8A8_SRGB, material_data.m_base_color_texture->m_width, material_data.m_base_color_texture->m_height, mip_levels);
			vulkan_rhi->createTextureImageView(device, material.base_color_texture_image, material.base_color_texture_image_view, mip_levels);


			//create unofirm buffer
			VkDeviceSize uniform_buffer_size = sizeof(UniformBufferObject);
			vulkan_rhi->createBuffer(physical_device,device,uniform_buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, material.material_uniform_buffer, material.material_uniform_buffer_memory);
			vkMapMemory(device, material.material_uniform_buffer_memory, 0, uniform_buffer_size, 0, &material.uniform_buffer_mapped_data);
			

			//create descriptor sets
			vulkan_rhi->allocateDescriptorSets(m_material_descriptor_set_layout, 1, material.material_descriptor_set);

			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = material.base_color_texture_image_view;
			imageInfo.sampler = vulkan_rhi->getOrCreateMipMapSampler(physical_device,device,mip_levels);

			std::array<VkWriteDescriptorSet, 1> descriptorWrites{};

			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].pNext = NULL;
			descriptorWrites[0].dstSet = *material.material_descriptor_set;
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

		}
		return m_materials[material_asset_id];
	}
	void RenderResource::updateMeshData(std::shared_ptr<RHI> rhi, uint32_t index_buffer_size, void* index_buffer, uint32_t vertex_buffer_size, MeshVertexDataDefinition* vertex_buffer, VulkanMesh& mesh)
	{

		updateVertexBuffer(rhi, vertex_buffer_size, vertex_buffer, mesh);
		mesh.index_count = index_buffer_size/sizeof(uint16_t);
		updateIndexBuffer(rhi, index_buffer_size, index_buffer, mesh);
		
	}

	void RenderResource::updateVertexBindingData(std::shared_ptr<RHI> rhi,uint32_t joint_buffer_size, MeshVertexBindingDataDefinition* joint_buffer_data, uint32_t index_count,VulkanMesh & mesh)
	{
		VulkanRHI* vulkan_rhi = static_cast<VulkanRHI*>(rhi.get());
		
		

		VkBuffer storage_buffer;
		VkDeviceMemory storage_buffer_memory;
		size_t vertex_blending_buffer_size = sizeof(MeshVertexBindingDataDefinition) * index_count;
		vulkan_rhi->createStorageBuffer(vertex_blending_buffer_size,storage_buffer,storage_buffer_memory);
		void* data;
		vkMapMemory(vulkan_rhi->m_device, storage_buffer_memory, 0, vertex_blending_buffer_size, 0, &data);
		MeshVertexBindingDataDefinition* p_vertex_binding = (MeshVertexBindingDataDefinition*)data;
		for (int i = 0; i < index_count; ++i)
		{
			p_vertex_binding[i].index0 = joint_buffer_data[i].index0;
			p_vertex_binding[i].index1 = joint_buffer_data[i].index1;
			p_vertex_binding[i].index2 = joint_buffer_data[i].index2;
			p_vertex_binding[i].index3 = joint_buffer_data[i].index3;

			p_vertex_binding[i].weight0 = joint_buffer_data[i].weight0;
			p_vertex_binding[i].weight1 = joint_buffer_data[i].weight1;
			p_vertex_binding[i].weight2 = joint_buffer_data[i].weight2;
			p_vertex_binding[i].weight3 = joint_buffer_data[i].weight3;
		}
		vkUnmapMemory(vulkan_rhi->m_device, storage_buffer_memory);
		vulkan_rhi->createBuffer(vulkan_rhi->physical_device, vulkan_rhi->m_device, vertex_blending_buffer_size,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, mesh.mesh_vertex_blending_buffer, mesh.mesh_vertex_blending_buffer_memory);
		vulkan_rhi->copyBuffer(rhi.get(), storage_buffer, mesh.mesh_vertex_blending_buffer, 0, 0, vertex_blending_buffer_size);

		VkDescriptorBufferInfo vertex_blending_buffer_info{};
		vertex_blending_buffer_info.offset = 0;
		vertex_blending_buffer_info.range = vertex_blending_buffer_size;
		vertex_blending_buffer_info.buffer = mesh.mesh_vertex_blending_buffer;

		VkWriteDescriptorSet writes_infos[1];
		writes_infos[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writes_infos[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		writes_infos[0].descriptorCount = 1;
		writes_infos[0].dstArrayElement = 0;
		writes_infos[0].dstBinding = 0;
		writes_infos[0].pNext = nullptr;
		writes_infos[0].dstSet = *mesh.p_vertex_blending_descriptor_set;
		writes_infos[0].pBufferInfo = &vertex_blending_buffer_info;
		vulkan_rhi->updateDescriptorSet(sizeof(writes_infos) / sizeof(writes_infos[0]), writes_infos);
	}

	void RenderResource::updateVertexBuffer(std::shared_ptr<RHI> rhi, uint32_t vertex_buffer_size, MeshVertexDataDefinition const* vertex_buffer, VulkanMesh& mesh)
	{
		VulkanRHI* vulkan_rhi = static_cast<VulkanRHI*>(rhi.get());

		uint32_t vertex_count = vertex_buffer_size / sizeof(MeshVertexDataDefinition);
		VkDeviceSize vertex_position_buffer_size = sizeof(MeshVertex::MeshVertexPosition) * vertex_count;
		VkDeviceSize vertex_normal_buffer_size = sizeof(MeshVertex::MeshVertexNormal) * vertex_count;
		VkDeviceSize vertex_tangent_buffer_size = sizeof(MeshVertex::MeshVertexTangent) * vertex_count;
		VkDeviceSize vertex_uv_buffer_size = sizeof(MeshVertex::MeshVertexUV) * vertex_count;

		VkDeviceSize stage_buffer_size = vertex_buffer_size;
		VkBuffer stage_buffer;
		VkDeviceMemory stage_buffer_memory;
		vulkan_rhi->createBuffer(vulkan_rhi->getPhysicalDevice(), vulkan_rhi->getDevice(), stage_buffer_size,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
			stage_buffer, 
			stage_buffer_memory);
		void* buffer_begin_pointer;

		VkDeviceSize vertex_position_buffer_offset = 0;
		VkDeviceSize vertex_normal_buffer_offset = vertex_position_buffer_offset + vertex_position_buffer_size;
		VkDeviceSize vertex_tangent_buffer_offset = vertex_normal_buffer_offset + vertex_normal_buffer_size;
		VkDeviceSize vertex_uv_buffer_offset = vertex_tangent_buffer_offset + vertex_tangent_buffer_size;

		vkMapMemory(vulkan_rhi->getDevice(), stage_buffer_memory, 0, VK_WHOLE_SIZE, 0, &buffer_begin_pointer);
		MeshVertex::MeshVertexPosition* mesh_vertex_position_pointer = reinterpret_cast<MeshVertex::MeshVertexPosition*>(reinterpret_cast<uintptr_t>(buffer_begin_pointer) + vertex_position_buffer_offset);
		MeshVertex::MeshVertexNormal* mesh_vertex_normal_pointer = reinterpret_cast<MeshVertex::MeshVertexNormal*>(reinterpret_cast<uintptr_t>(buffer_begin_pointer) + vertex_normal_buffer_offset);
		MeshVertex::MeshVertexTangent * mesh_vertex_tangent_pointer = reinterpret_cast<MeshVertex::MeshVertexTangent*>(reinterpret_cast<uintptr_t>(buffer_begin_pointer) + vertex_tangent_buffer_offset);
		MeshVertex::MeshVertexUV * mesh_vertex_UV_pointer = reinterpret_cast<MeshVertex::MeshVertexUV*>(reinterpret_cast<uintptr_t>(buffer_begin_pointer) + vertex_uv_buffer_offset);
		//memcpy(data, vertex_buffer, (size_t)stage_buffer_size);
		for (size_t vertex_index = 0; vertex_index < vertex_count; ++vertex_index)
		{
			mesh_vertex_position_pointer[vertex_index].position = Vector3(vertex_buffer[vertex_index].x, vertex_buffer[vertex_index].y, vertex_buffer[vertex_index].z);
			mesh_vertex_normal_pointer[vertex_index].normal = Vector3(vertex_buffer[vertex_index].nx, vertex_buffer[vertex_index].ny, vertex_buffer[vertex_index].nz);
			mesh_vertex_tangent_pointer[vertex_index].tangent = Vector3(vertex_buffer[vertex_index].tx, vertex_buffer[vertex_index].ty, vertex_buffer[vertex_index].tz);
			mesh_vertex_UV_pointer[vertex_index].uv = Vector2(vertex_buffer[vertex_index].u, vertex_buffer[vertex_index].v);
		}
		vkUnmapMemory(vulkan_rhi->getDevice(), stage_buffer_memory);
		
		vulkan_rhi->createBuffer(vulkan_rhi->getPhysicalDevice(), vulkan_rhi->getDevice(), 
			vertex_position_buffer_size, 
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			mesh.mesh_vertex_position_buffer, 
			mesh.mesh_vertex_position_buffer_memory);

		vulkan_rhi->createBuffer(vulkan_rhi->getPhysicalDevice(), vulkan_rhi->getDevice(), 
			vertex_normal_buffer_size, 
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			mesh.mesh_vertex_normal_buffer, 
			mesh.mesh_vertex_normal_buffer_memory);
		
		vulkan_rhi->createBuffer(vulkan_rhi->getPhysicalDevice(), vulkan_rhi->getDevice(), 
			vertex_tangent_buffer_size, 
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			mesh.mesh_vertex_tangent_buffer, 
			mesh.mesh_vertex_tangent_buffer_memory);
		
		vulkan_rhi->createBuffer(vulkan_rhi->getPhysicalDevice(), vulkan_rhi->getDevice(), 
			vertex_uv_buffer_size, 
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			mesh.mesh_vertex_uv_buffer, 
			mesh.mesh_vertex_uv_buffer_memory);

		vulkan_rhi->copyBuffer(vulkan_rhi, stage_buffer, mesh.mesh_vertex_position_buffer,vertex_position_buffer_offset,0, vertex_position_buffer_size);
		vulkan_rhi->copyBuffer(vulkan_rhi, stage_buffer, mesh.mesh_vertex_normal_buffer,vertex_normal_buffer_offset,0, vertex_normal_buffer_size);
		vulkan_rhi->copyBuffer(vulkan_rhi, stage_buffer, mesh.mesh_vertex_tangent_buffer,vertex_tangent_buffer_offset,0, vertex_tangent_buffer_size);
		vulkan_rhi->copyBuffer(vulkan_rhi, stage_buffer, mesh.mesh_vertex_uv_buffer,vertex_uv_buffer_offset,0, vertex_uv_buffer_size);
		
		vkDestroyBuffer(vulkan_rhi->getDevice(), stage_buffer, nullptr);
		vkFreeMemory(vulkan_rhi->getDevice(), stage_buffer_memory, nullptr);
	}

	void RenderResource::updateIndexBuffer(std::shared_ptr<RHI> rhi, uint32_t index_buffer_size, const void* index_buffer, VulkanMesh& mesh)
	{
		VulkanRHI* vulkan_rhi = static_cast<VulkanRHI*>(rhi.get());
		VkDeviceSize bufferSize = index_buffer_size;
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		vulkan_rhi->createBuffer(vulkan_rhi->getPhysicalDevice(), vulkan_rhi->getDevice(), bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(vulkan_rhi->getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, index_buffer, (size_t)bufferSize);
		vkUnmapMemory(vulkan_rhi->getDevice(), stagingBufferMemory);

		vulkan_rhi->createBuffer(vulkan_rhi->getPhysicalDevice(), vulkan_rhi->getDevice(), bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			mesh.mesh_vertex_index_buffer, mesh.mesh_vertex_index_buffer_memory);

		vulkan_rhi->copyBuffer(vulkan_rhi, stagingBuffer, mesh.mesh_vertex_index_buffer,0,0, bufferSize);

		vkDestroyBuffer(vulkan_rhi->getDevice(), stagingBuffer, nullptr);
		vkFreeMemory(vulkan_rhi->getDevice(), stagingBufferMemory, nullptr);
	}
	void RenderResource::updatePerframeBuffer(std::shared_ptr<RenderCamera> camera)
	{
		
		Matrix4x4 view_matrix = camera->getViewMatrix();
		Matrix4x4 project_matrix = camera->getProjMatrix();
		Vector3 camera_position = camera->getCameraPos();
		Matrix4x4 proj_view_matrix = project_matrix * view_matrix;
		m_mesh_per_frame_storage_buffer_object.project_view_matrix = proj_view_matrix;
		m_mesh_per_frame_storage_buffer_object.camera_position = camera_position;
		m_mesh_inefficient_pick_perframe_storage_buffer_object.project_view_matrix = proj_view_matrix;
	}
}