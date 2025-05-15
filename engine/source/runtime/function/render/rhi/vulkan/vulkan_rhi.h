#pragma once
#include <algorithm>
#include <chrono>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <limits>
#include <array>
#include <optional>
#include <set>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <deque>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL

#ifdef _WIN32 
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#ifdef USE_VOLK
#define VK_NO_PROTOTYPES
#include "vulkan/vulkan.h"
#include <vulkan/volk.h>
#else
#include "vulkan/vulkan.h"
#endif // USE_VOLK

#include <vulkan/vk_mem_alloc.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>
#include "function/render/rhi/rhi.h"
#include <core/math/vector2.h>
#include <core/math/vector3.h>
#include <core/math/matrix4.h>
#include "function/render/render_type.h"
#include "core/utils/spin_lock.h"
#include "function/ui/image.h"
#include "core/helper.h"
struct MeshVertex {
	glm::vec3 pos;
	//glm::vec3 color;
	glm::vec2 texCoord;

	struct MeshVertexPosition
	{
		qyhs::Vector3 position;
	};

	struct MeshVertexNormal
	{
		qyhs::Vector3 normal;
	};

	struct MeshVertexTangent
	{
		qyhs::Vector3 tangent;  //切线
	};

	struct MeshVertexUV
	{
		qyhs::Vector2 uv;
	};

	static std::array<VkVertexInputBindingDescription, 4> getBindingDescription() {
		std::array<VkVertexInputBindingDescription, 4> binding_descriptions{};

		binding_descriptions[0].binding = 0;
		binding_descriptions[0].stride = sizeof(MeshVertexPosition);
		binding_descriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		binding_descriptions[1].binding = 1;
		binding_descriptions[1].stride = sizeof(MeshVertexNormal);
		binding_descriptions[1].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		binding_descriptions[2].binding = 2;
		binding_descriptions[2].stride = sizeof(MeshVertexTangent);
		binding_descriptions[2].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		binding_descriptions[3].binding = 3;
		binding_descriptions[3].stride = sizeof(MeshVertexUV);
		binding_descriptions[3].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return binding_descriptions;
	}

	static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(MeshVertexPosition, position);

		attributeDescriptions[1].binding = 1;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(MeshVertexNormal, normal);

		attributeDescriptions[2].binding = 2;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(MeshVertexTangent, tangent);

		attributeDescriptions[3].binding = 3;
		attributeDescriptions[3].location = 3;
		attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[3].offset = offsetof(MeshVertexUV, uv);

		return attributeDescriptions;
	}

	bool operator==(const MeshVertex& other) const {
		return pos == other.pos && texCoord == other.texCoord;
	}
};


const uint32_t WIDTH = 2500;
const uint32_t HEIGHT = 1600;

const std::string MODEL_PATH = "../../asset/objects/house.obj";
const std::string TEXTURE_PATH = "E:/VS_Project/QyhsEngine/asset/objects/_textures/n.tga";

const int MAX_FRAMES_IN_FLIGHT = 3;

#ifdef NDEBUG
const bool enableValidationLayers = false;
const bool enable_debug_utils_label = false;
#else
const bool enableValidationLayers = true;
const bool enable_debug_utils_label = true;
#endif



struct QueueFamilyIndices {
	std::optional<uint32_t> graphics_family;
	std::optional<uint32_t> present_family;
	std::optional<uint32_t> compute_family;
	std::optional<uint32_t> copy_family;
	std::optional<uint32_t> video_family;

	bool isComplete() {
		return graphics_family.has_value() && compute_family.has_value() && copy_family.has_value() && video_family.has_value();
	}
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> present_modes;
};

namespace std {
	template<> struct hash<MeshVertex> {
		size_t operator()(MeshVertex const& vertex) const {
			return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec2>()(vertex.texCoord) << 1)));
		}
	};
}

struct UniformBufferObject {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};
namespace qyhs
{
	static constexpr uint32_t DESCRIPTORBINDER_CBV_COUNT = 14;
	static constexpr uint32_t DESCRIPTORBINDER_SRV_COUNT = 16;
	static constexpr uint32_t DESCRIPTORBINDER_UAV_COUNT = 16;
	static constexpr uint32_t DESCRIPTORBINDER_SAMPLER_COUNT = 8;
	struct DescriptorBindingTable
	{
		GPUBuffer CONSTANT_BUFFER_VIEW[DESCRIPTORBINDER_CBV_COUNT];
		uint64_t CBV_offset[DESCRIPTORBINDER_CBV_COUNT] = {};
		GPUResource SHADER_RESOURCE_VIEW[DESCRIPTORBINDER_SRV_COUNT];
		int SRV_index[DESCRIPTORBINDER_SRV_COUNT] = {};
		GPUResource UNORDER_ACCESS_VIEW[DESCRIPTORBINDER_UAV_COUNT];
		int UAV_index[DESCRIPTORBINDER_UAV_COUNT] = {};
		Sampler SAMPLER[DESCRIPTORBINDER_SAMPLER_COUNT];
	};

	//TODO:Destroy buffer  when application is destroyed
	struct VulkanMaterial
	{
		//common base texture image
		VkImage	base_color_texture_image;
		VkImageView	base_color_texture_image_view;
		VkDeviceMemory base_color_texture_image_memory;

		VkBuffer material_uniform_buffer;
		VkDeviceMemory material_uniform_buffer_memory;
		void* uniform_buffer_mapped_data;
		VkDescriptorSet* material_descriptor_set;
	};

	struct BindingUsage
	{
		bool used = false;
		VkDescriptorSetLayoutBinding binding = {};
	};

	struct PipelineState_Vulkan
	{

		size_t hash = 0;
		size_t binding_hash = 0;
		VkGraphicsPipelineCreateInfo pipeline_info = {};
		VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
		VkPipelineRasterizationStateCreateInfo rasterizer = {};
		VkSampleMask samplemask = {};
		std::vector<VkDescriptorSetLayoutBinding> layout_bindings;
		std::vector<VkImageViewType> imageViewTypes;
		VkDeviceSize uniform_buffer_sizes[DESCRIPTORBINDER_CBV_COUNT] = {};
		std::vector<uint32_t> uniform_buffer_dynamic_slots;
		VkPushConstantRange pushconstants;
		std::vector<BindingUsage> bindlessBindings;
		VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
		size_t bindlessFirstSet = 0;
		std::vector<VkDescriptorSet> bindlessSets;
		VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
		VkPipelineShaderStageCreateInfo shaderStages[static_cast<size_t>(ShaderStage::COUNT)] = {};
		VkPipelineRasterizationDepthClipStateCreateInfoEXT depth_clip_state_info = {};
		VkPipelineRasterizationConservativeStateCreateInfoEXT rasterization_conservative_state = {};
		VkPipelineViewportStateCreateInfo viewport_state = {};
		VkPipelineDepthStencilStateCreateInfo depthstencil = {};
		VkPipelineTessellationStateCreateInfo tessellation_info = {};
		VkPipeline pipeline = VK_NULL_HANDLE;
	};

	//TODO:Destroy buffer
	struct VulkanMesh
	{
		VkBuffer mesh_vertex_position_buffer;
		VkDeviceMemory mesh_vertex_position_buffer_memory;

		VkBuffer mesh_vertex_normal_buffer;
		VkDeviceMemory mesh_vertex_normal_buffer_memory;

		VkBuffer mesh_vertex_tangent_buffer;
		VkDeviceMemory mesh_vertex_tangent_buffer_memory;

		VkBuffer mesh_vertex_uv_buffer;
		VkDeviceMemory mesh_vertex_uv_buffer_memory;

		VkBuffer mesh_vertex_index_buffer;
		VkDeviceMemory mesh_vertex_index_buffer_memory;

		VkBuffer mesh_vertex_blending_buffer;
		VkDeviceMemory mesh_vertex_blending_buffer_memory;

		size_t index_count;

		VkDescriptorSet* p_vertex_blending_descriptor_set{ nullptr };

	};

	struct Sampler_Vulkan
	{
		VkSampler resource;
		uint32_t index;
	};

	struct RenderMeshNode
	{
		Matrix4x4 model_matrix;
		VulkanMesh* p_mesh;
		VulkanMaterial* p_material;
		uint32_t node_id;
		bool enable_vertex_blending{ false };
		Matrix4x4* joint_matrices;
		size_t joint_count{ 0 };
	};

	struct Shader_Vulkan
	{
		VkShaderModule shader_module = VK_NULL_HANDLE;
		VkPipelineShaderStageCreateInfo stage_info{};
		VkPushConstantRange pushconstants{};
		std::vector<BindingUsage>bindingless_bindings;
		std::vector<VkDescriptorSetLayoutBinding>layout_bindings;
		std::vector<VkImageViewType> imageViewTypes;
		VkDeviceSize uniform_buffer_sizes[DESCRIPTORBINDER_CBV_COUNT] = {};
		std::vector<uint32_t> uniform_buffer_dynamic_slots;
	};

	struct Texture_Vulkan
	{
		VkImage resource = VK_NULL_HANDLE;
		VkImageLayout default_layout = VK_IMAGE_LAYOUT_GENERAL;
		uint32_t framebuffer_layercount = 0;
		VmaAllocation allocation = nullptr;
		struct TextureSubresource
		{
			VkImageView image_view = VK_NULL_HANDLE;
			int index = -1;
			int base_mip_level = 0;
			int mip_count = 1;
			int base_array_layer = 0;
			int layer_count = 1;
			bool isValid()const { return image_view != VK_NULL_HANDLE; }
		};
		TextureSubresource srv;
		TextureSubresource uav;
		TextureSubresource rtv;
		TextureSubresource dsv;
		std::vector<TextureSubresource> subresources_srv = {};
		std::vector<TextureSubresource> subresources_uav = {};
		std::vector<TextureSubresource> subresources_rtv = {};
		std::vector<TextureSubresource> subresources_dsv = {};
	};

	class VulkanRHI final :public RHI
	{
	public:
		
		struct DescriptorBinder
		{
			VulkanRHI * rhi = nullptr;
			std::vector<VkWriteDescriptorSet> descriptor_set_writes;
			VkDescriptorSet descriptorset_graphics;
			std::vector<VkDescriptorImageInfo> image_infos;
			std::vector<VkDescriptorBufferInfo> buffer_infos;
			uint32_t uniform_buffer_dynamic_offsets[DESCRIPTORBINDER_CBV_COUNT] = {};
			//update and bind descriptor set
			void flush(bool graphics, CommandList cmd);
			DescriptorBindingTable table;
			enum DIRTY_FLAGS
			{
				DIRTY_NONE = 0,
				DIRTY_DESCRIPTOR = 1<<0,
				DIRTY_OFFSET = 1<<1,
				DIRTY_ALL = ~0
			};
			DIRTY_FLAGS dirty = DIRTY_NONE;
			void init(VulkanRHI * vulkan_rhi)
			{
				rhi = vulkan_rhi;
			}
		};

		struct DescriptorBinderPool
		{
			VulkanRHI* rhi = nullptr;
			void init(VulkanRHI* vulkan_rhi);
			uint32_t pool_size = 256;
			VkDescriptorPool descriptor_pool = VK_NULL_HANDLE;
			void destroy()
			{
				if (descriptor_pool != VK_NULL_HANDLE)
				{
					rhi->allocation_handler->destroy_locker.lock();
					rhi->allocation_handler->destroy_descriptorPools.push_back(std::make_pair(descriptor_pool, rhi->frame_count));
					descriptor_pool = VK_NULL_HANDLE;
					rhi->allocation_handler->destroy_locker.unlock();
				}
			}
		};

		struct CommandList_Vulkan
		{
			DescriptorBinder binder;
			DescriptorBinderPool binder_pools[BUFFER_COUNT];
			VkCommandPool command_pools[BUFFER_COUNT][QUEUE_COUNT];
			VkCommandBuffer command_buffers[BUFFER_COUNT][QUEUE_COUNT];
			GPUBufferAllocator frame_allocators[BUFFER_COUNT];
			std::vector<VkImageMemoryBarrier2>renderpass_barriers_end;
			RenderPassInfo renderpass_info;
			const VkCommandBuffer getCommandBuffer() const { return command_buffers[m_buffer_index][queue]; }
			VkCommandPool getCommandPool() const { return command_pools[m_buffer_index][queue]; }
			const PipelineState* active_pso = nullptr;
			QueueType queue{};
			std::vector<VkSemaphore> signals;		//signal that after submit command list
			std::vector<VkSemaphore> waits;			//wait for signal
			std::vector<std::pair<QueueType, VkSemaphore>> wait_queues;	//wait for queue
			std::vector<SwapChain> prev_swapchains;
			std::vector<std::pair<size_t, VkPipeline>> pipelines_worker;
			uint32_t index{ 0 };
			size_t prev_pipeline_hash = 0;
			uint32_t m_buffer_index{ 0 };
			const Shader* active_cs = {};
			bool dirty_pso = true;

			void reset(uint32_t buffer_index)
			{
				m_buffer_index = buffer_index;
				signals.clear();
				waits.clear();
				wait_queues.clear();
				prev_swapchains.clear();
				frame_allocators[buffer_index].reset();
				active_pso = nullptr;
				active_cs = nullptr;
			}
		};

		struct CopyAllocator
		{
			struct CopyCMD
			{
				VkCommandBuffer transfer_command_buffer = VK_NULL_HANDLE;
				VkCommandBuffer transition_command_buffer = VK_NULL_HANDLE;
				VkCommandPool transfer_command_pool = VK_NULL_HANDLE;
				VkCommandPool transition_command_pool = VK_NULL_HANDLE;
				std::vector<VkSemaphore> semaphores = { VK_NULL_HANDLE,VK_NULL_HANDLE,VK_NULL_HANDLE };
				GPUBuffer upload_buffer;
				VkFence fence = VK_NULL_HANDLE;
				bool isValid() { return transfer_command_buffer != VK_NULL_HANDLE; }
			};
			std::vector<CopyCMD> free_cmd_list;
			std::mutex locker;
			VulkanRHI* rhi = nullptr;
			void init(VulkanRHI* rhi) { 
				this->rhi = rhi;
			}
			CopyCMD allocate(uint32_t staging_size)
			{
				CopyCMD cmd;

				locker.lock();
				// Try to search for a staging buffer that can fit the request:
				for (size_t i = 0; i < free_cmd_list.size(); ++i)
				{
					if (free_cmd_list[i].upload_buffer.desc.size >= staging_size)
					{
						if (vkGetFenceStatus(rhi->getDevice(), free_cmd_list[i].fence) == VK_SUCCESS)
						{
							cmd = std::move(free_cmd_list[i]);
							std::swap(free_cmd_list[i], free_cmd_list.back());
							free_cmd_list.pop_back();
							break;
						}
					}
				}
				locker.unlock();

				// If no buffer was found that fits the data, create one:
				if (!cmd.isValid())
				{
					VkCommandPoolCreateInfo poolInfo = {};
					poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
					poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
					poolInfo.queueFamilyIndex = rhi->queue_family_indices.copy_family.value();
					VkResult res = vkCreateCommandPool(rhi->m_device, &poolInfo, nullptr, &cmd.transfer_command_pool);
					assert(res == VK_SUCCESS);
					poolInfo.queueFamilyIndex = rhi->graphics_family;
					res = vkCreateCommandPool(rhi->m_device, &poolInfo, nullptr, &cmd.transition_command_pool);
					assert(res == VK_SUCCESS);

					VkCommandBufferAllocateInfo commandBufferInfo = {};
					commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
					commandBufferInfo.commandBufferCount = 1;
					commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
					commandBufferInfo.commandPool = cmd.transfer_command_pool;
					res = vkAllocateCommandBuffers(rhi->m_device, &commandBufferInfo, &cmd.transfer_command_buffer);
					assert(res == VK_SUCCESS);
					commandBufferInfo.commandPool = cmd.transition_command_pool;
					res = vkAllocateCommandBuffers(rhi->m_device, &commandBufferInfo, &cmd.transition_command_buffer);
					assert(res == VK_SUCCESS);

					VkFenceCreateInfo fenceInfo = {};
					fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
					res = vkCreateFence(rhi->m_device, &fenceInfo, nullptr, &cmd.fence);
					assert(res == VK_SUCCESS);

					VkSemaphoreCreateInfo semaphoreInfo = {};
					semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
					res = vkCreateSemaphore(rhi->m_device, &semaphoreInfo, nullptr, &cmd.semaphores[0]);
					assert(res == VK_SUCCESS);
					res = vkCreateSemaphore(rhi->m_device, &semaphoreInfo, nullptr, &cmd.semaphores[1]);
					assert(res == VK_SUCCESS);
					res = vkCreateSemaphore(rhi->m_device, &semaphoreInfo, nullptr, &cmd.semaphores[2]);
					assert(res == VK_SUCCESS);

					GPUBufferDesc uploaddesc;
					uploaddesc.size = helper::getNextPowerOfTwo(staging_size);
					uploaddesc.size = std::max(uploaddesc.size, uint64_t(65536));
					uploaddesc.usage = Usage::UPLOAD;
					bool upload_success = rhi->createBuffer(&uploaddesc, &cmd.upload_buffer);
					assert(upload_success);
					rhi->setName(&cmd.upload_buffer, "CopyAllocator::uploadBuffer");
				}
				//CopyCMD allocate(uint32_t staging_size)
				//{
				//	CopyCMD cmd;

				//	locker.lock();
				//	// Try to search for a staging buffer that can fit the request:
				//	for (size_t i = 0; i < free_cmd_list.size(); ++i)
				//	{
				//		if (free_cmd_list[i].upload_buffer.desc.size >= staging_size)
				//		{
				//			if (vkGetFenceStatus(rhi->getDevice(), free_cmd_list[i].fence) == VK_SUCCESS)
				//			{
				//				cmd = std::move(free_cmd_list[i]);
				//				std::swap(free_cmd_list[i], free_cmd_list.back());
				//				free_cmd_list.pop_back();
				//				break;
				//			}
				//		}
				//	}
				//	locker.unlock();

				//	// If no buffer was found that fits the data, create one:
				//	if (!cmd.isValid())
				//	{
				//		VkCommandPoolCreateInfo poolInfo = {};
				//		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
				//		poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
				//		poolInfo.queueFamilyIndex = rhi->transfer_family;
				//		VkResult res = vkCreateCommandPool(rhi->m_device, &poolInfo, nullptr, &cmd.transfer_command_pool);
				//		assert(res == VK_SUCCESS);
				//		poolInfo.queueFamilyIndex = rhi->graphics_family;
				//		res = vkCreateCommandPool(rhi->m_device, &poolInfo, nullptr, &cmd.transition_command_pool);
				//		assert(res == VK_SUCCESS);

				//		VkCommandBufferAllocateInfo commandBufferInfo = {};
				//		commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
				//		commandBufferInfo.commandBufferCount = 1;
				//		commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
				//		commandBufferInfo.commandPool = cmd.transfer_command_pool;
				//		res = vkAllocateCommandBuffers(rhi->m_device, &commandBufferInfo, &cmd.transfer_command_buffer);
				//		assert(res == VK_SUCCESS);
				//		commandBufferInfo.commandPool = cmd.transition_command_pool;
				//		res = vkAllocateCommandBuffers(rhi->m_device, &commandBufferInfo, &cmd.transition_command_buffer);
				//		assert(res == VK_SUCCESS);

				//		VkFenceCreateInfo fenceInfo = {};
				//		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
				//		res = vkCreateFence(rhi->m_device, &fenceInfo, nullptr, &cmd.fence);
				//		assert(res == VK_SUCCESS);

				//		VkSemaphoreCreateInfo semaphoreInfo = {};
				//		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
				//		res = vkCreateSemaphore(rhi->m_device, &semaphoreInfo, nullptr, &cmd.semaphores[0]);
				//		assert(res == VK_SUCCESS);
				//		res = vkCreateSemaphore(rhi->m_device, &semaphoreInfo, nullptr, &cmd.semaphores[1]);
				//		assert(res == VK_SUCCESS);
				//		res = vkCreateSemaphore(rhi->m_device, &semaphoreInfo, nullptr, &cmd.semaphores[2]);
				//		assert(res == VK_SUCCESS);

				//		GPUBufferDesc uploaddesc;
				//		uploaddesc.size = helper::getNextPowerOfTwo(staging_size);
				//		uploaddesc.size = std::max(uploaddesc.size, uint64_t(65536));
				//		uploaddesc.usage = Usage::UPLOAD;
				//		bool upload_success = rhi->createBuffer(&uploaddesc, &cmd.upload_buffer);
				//		assert(upload_success);
				//		rhi->setName(&cmd.upload_buffer, "CopyAllocator::uploadBuffer");
				//	}

				// begin command list in valid state:
				VkResult res = vkResetCommandPool(rhi->m_device, cmd.transfer_command_pool, 0);
				assert(res == VK_SUCCESS);
				res = vkResetCommandPool(rhi->m_device, cmd.transition_command_pool, 0);
				assert(res == VK_SUCCESS);

				VkCommandBufferBeginInfo beginInfo = {};
				beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
				beginInfo.pInheritanceInfo = nullptr;
				res = vkBeginCommandBuffer(cmd.transfer_command_buffer, &beginInfo);
				assert(res == VK_SUCCESS);
				res = vkBeginCommandBuffer(cmd.transition_command_buffer, &beginInfo);
				assert(res == VK_SUCCESS);

				res = vkResetFences(rhi->m_device, 1, &cmd.fence);
				assert(res == VK_SUCCESS);

				return cmd;
			}

			void submit(CopyCMD& cmd);
		} copy_allocator;

		/*struct VulkanTexture
		{
			VkImage resource;
			VmaAllocation allocation;
		};*/

		struct CommandQueue
		{
			VkQueue queue{ VK_NULL_HANDLE };
			std::shared_ptr<std::mutex> locker;
			std::vector<VkCommandBufferSubmitInfo> submit_cmds;
			std::vector<VkSemaphoreSubmitInfo> submit_waitSemaphoreInfos;
			std::vector<VkSemaphoreSubmitInfo> submit_signalSemaphoreInfos;
			std::vector<VkSwapchainKHR> submit_swapchains;
			std::vector<VkSemaphore> submit_signalSemaphores;
			std::vector<SwapChain> swapchain_updates;
			std::vector<uint32_t> submit_swapchain_image_indices;
			bool sparse_binding_supported{ false };
			void submit(VulkanRHI* rhi, VkFence fence);
			void signal(VkSemaphore& semaphore);
			void wait(VkSemaphore& semaphore);
		} queues[QUEUE_COUNT];

		struct SwapChain_Vulkan
		{
			VkSurfaceKHR surface{ VK_NULL_HANDLE };
			SwapChainDesc desc;
			VkSwapchainKHR swapchain{ VK_NULL_HANDLE };
			graphics::ColorSpace color_space{ graphics::ColorSpace::SRGB };
			VkExtent2D swapChainExtent;
			VkFormat swapchainImageFormat;
			std::vector<VkImage> swapchain_images;
			std::vector<VkImageView> swapchain_image_views;
			uint32_t swapchain_image_index{ 0 };
			std::vector<VkSemaphore> swapchainAcquireSemaphores;
			VkSemaphore swapchain_release_semaphore = VK_NULL_HANDLE;
			uint32_t swapchain_acquire_semaphore_index{ 0 };
			std::mutex locker;
		};

		VulkanRHI();
		virtual ~VulkanRHI() override final;
		void initialize() override;
		void initialize2() override;
		void cleanup();
		VkCommandBuffer beginSingleTimeCommands();
		void endSingleTimeCommands(VkCommandBuffer commandBuffer);
		void waitForFence();
		void getNextImage(std::function<void()> update_pass_after_recreate_swap_chain);
		void updateUniformBuffer();
		void resetFence();
		void resetCommandBuffer();
		void resetCommandPool();
		void submitRender(std::function<void()>);
		void destroyImage(VkImage image);
		void destroyImageView(VkImageView image_view);
		void freeMemory(VkDeviceMemory memory);
		void destroyFrameBuffer(VkFramebuffer framebuffer);
		bool checkValidationLayerSupport();
		void updateDescriptorSet(uint32_t write_count, VkWriteDescriptorSet* p_descriptor_write, uint32_t copy_count = 0, const VkCopyDescriptorSet* p_descriptor_copies = nullptr);
		void allocateDescriptorSets(VkDescriptorSetLayout* p_descriptor_set_layout, uint32_t descriptor_set_count, VkDescriptorSet*& p_descriptor_set, VkDescriptorSetAllocateInfo* p_next = NULL);
		void createStorageBuffer(VkDeviceSize buffer_size, VkBuffer& storage_buffer, VkDeviceMemory& storage_buffer_memory);
		void cmdBindDescriptorSets(VkPipelineBindPoint bind_point, VkPipelineLayout* pipeline_layout, int first_set, int set_count, const VkDescriptorSet* const* pDescriptorSets, uint32_t dynamic_offset_count, const uint32_t* p_dynamic_offsets);
		void createDescriptorSetLayout(VkDescriptorSetLayoutCreateInfo* create_info, const VkAllocationCallbacks* callbacks, VkDescriptorSetLayout*& p_descriptor_set_layout);
		virtual bool createSampler(const SamplerDesc* sampler_desc, Sampler* sampler) override;
		CommandList beginCommandList(QueueType queue) override;
		int getMaxFrameInFlight() { return MAX_FRAMES_IN_FLIGHT; }
		virtual int createSubresource(Texture* texture, SubresourceType type, uint32_t firstSlice, uint32_t sliceCount, uint32_t firstMip, uint32_t mipCount, const Format* format_change = nullptr, const ImageAspect* aspect = nullptr, const Swizzle* swizzle = nullptr, float min_lod_clamp = 0) const override;
		virtual int createSubresource(GPUBuffer* buffer, SubresourceType type, uint64_t offset, uint64_t size = ~0ull,const Format * format_change = nullptr) override;
		void drawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t startVertexLocation, uint32_t startInstanceLocation, CommandList cmd) override;
		VkDevice getDevice() { return m_device; }
		VkPhysicalDevice getPhysicalDevice() { return physical_device; }
		VkFormat getSwapChainImageFormat() { return swapChainImageFormat; }
		VkSampleCountFlagBits getMSAASamples() { return VK_SAMPLE_COUNT_1_BIT; }
		VkFormat getDepthImageFormat() { return m_depth_image_format; }
		VkExtent2D getSwapChainExtent() { return m_swapchain_extent; }
		virtual void bindScissorRects(uint32_t scissor_count, const Rect* rect, CommandList cmd) override;
		VkCommandBuffer& getCurrentCommandBuffer() { return m_command_buffers[m_current_frame_index]; }
		VkDescriptorPool& getDescriptorPool() { return m_descriptor_pool; }
		virtual void bindPipelineState(const PipelineState* state, CommandList cmd) override;
		VkBuffer getUniformBuffer(uint32_t index) {
			return uniformBuffers[index];
		}
		virtual void setName(Shader* shader, const char* name) const override;
		virtual void setName(GPUResource* resource, const char* name) const override;
		std::mutex pso_layout_cache_mutex;
		struct PSOLayout
		{
			VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
			VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
			std::vector<VkDescriptorSet> bindlessSets;
			size_t bindlessFirstSet = 0;
		};
		std::unordered_map<size_t, PSOLayout> pso_layout_cache;
		void predraw(CommandList cmd);
		void psoValidate(CommandList cmd);
		virtual void endEvent(CommandList cmd) override;
		virtual void drawIndexed(uint32_t indexCount, uint32_t startIndexLocation, int32_t baseVertexLocation, CommandList cmd) override;
		virtual void draw(uint32_t vertex_count, uint32_t start_draw_location, CommandList cmd) override;
		virtual bool createPipelineState(const PipelineStateDesc* desc, PipelineState* state, const RenderPassInfo* renderpass_info=nullptr) override;
		virtual int getDescriptorIndex(const Sampler* sampler)override;
		virtual GPUBufferAllocator& getFrameAllocator(CommandList cmd) override;
		virtual bool createBuffer(const GPUBufferDesc *desc, GPUBuffer* buffer,const std::function<void(void*)>& init_buffer_callback = nullptr) override;
		virtual bool createTexture(const TextureDesc* desc, graphics::Texture* texture,SubresourceData * initial_data = nullptr) override;
		virtual RenderPassInfo getRenderPassInfo(CommandList cmd) const override;
		VkSamplerAddressMode convertTextureAddressMode(const TextureAddressMode& address_mode);
		virtual void drawImage(const graphics::Texture* texture, const qyhs::image::Params& params, CommandList cmd) override;
		virtual void submitCommandLists() override;
		VkImageView& getTextureImageView() { return textureImageView; }
		VkSampler& getTextureSampler() { return textureSampler; }
		uint32_t getCurrentFrameIndex() { return m_current_frame_index; }
		std::vector<VkImageView>& getSwapChainImageView()
		{
			return swapChainImageViews;
		}
		VkPhysicalDeviceDepthClipEnableFeaturesEXT depth_clip_enable_features = {};
		bool checkExtensionSupport(const char* check_extension, const std::vector<VkExtensionProperties>& available_extensions);
		VkPipelineCache pipeline_cache;
		uint64_t getMinOffsetAlignment(const GPUBufferDesc* desc) const override;
		VkViewport& getViewport() { return m_viewport; }
		void prepareBeforeRender(std::function<void()> update_pass_after_recreate_swap_chain);
		void beginEvent(VkCommandBuffer command_buffer, std::string event_name, std::array<float, 4> color = { 1.0f,1.0f,1.0f,1.0f });
		virtual void beginEvent(const std::string& name, CommandList cmd) override;
		void endEvent(VkCommandBuffer command_buffer);
		virtual void prepareContext() override;
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
		VkQueue getGraphicsQueue() { return graphics_queue; }
		VkResult allocatecommandbuffers(VkCommandBufferAllocateInfo* allocate_info, VkCommandBuffer* command_buffer);
		VkResult beginCommandBuffer(VkCommandBuffer* command_buffer, VkCommandBufferBeginInfo* begin_info);
		VkResult endCommandBuffer(VkCommandBuffer* command_buffer);
		VkResult queueSubmit(VkQueue queue, uint32_t submit_count, VkSubmitInfo* submit_info, VkFence fence);
		VkResult queueWaitIdle(VkQueue queue);
		void freeCommandBuffers(VkCommandPool command_pool, size_t free_count, VkCommandBuffer* p_command_buffer);
		virtual int getDescriptorIndex(const GPUResource* resource, SubresourceType type, int subresource = -1)const override;
		VkShaderModule createShaderModule(VkDevice device, const std::vector<char>& code);
		void createImage(VkPhysicalDevice physical_device, VkDevice device, uint32_t width, uint32_t height, VkFormat format, uint32_t mip_levels, VkSampleCountFlagBits num_samples, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
		VkImageView createImageView(VkDevice device, VkImage image, VkFormat format, VkImageViewType image_view_type, uint32_t layer_count, uint32_t mip_levels, VkImageAspectFlags aspectFlags);
		uint32_t findMemoryType(VkPhysicalDevice physical_device, uint32_t typeFilter, VkMemoryPropertyFlags properties);
		void createBuffer(VkPhysicalDevice physical_device, VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		void transitionImageLayout(RHI* vulkan_rhi, VkImage image, VkFormat format, uint32_t layer_count, uint32_t mip_levels, VkImageLayout oldLayout, VkImageLayout newLayout);
		void copyBufferToImage(RHI* vulkan_rhi, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layer_count);
		void generateMipmaps(RHI* vulkan_rhi, VkImage image, VkFormat format, uint32_t tex_width, uint32_t tex_height, uint32_t mip_levels);
		void createTextureImageView(VkDevice device, VkImage image, VkImageView& image_view, uint32_t mip_levels);
		void copyBuffer(RHI* rhi, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize src_offset, VkDeviceSize dst_offset, VkDeviceSize size);
		std::vector<VkSampler> immutable_samplers;
		VkSampler& getOrCreateMipMapSampler(VkPhysicalDevice physical_device, VkDevice device, uint32_t mip_levels);
		std::unordered_map<uint32_t, VkSampler> m_mipmap_sampler_map;
		void createCubeMap(RHI* rhi, VkImage& image, VkImageView& image_view, VmaAllocation& allocation, uint32_t width, uint32_t height, std::array<void*, 6> pixels, PIXEL_FORMAT texture_image_format, uint32_t miplevels);
		VkSampler getOrCreateNearestSampler(VkPhysicalDevice physical_device, VkDevice device);
		virtual bool createSwapChain(platform::WindowType window, SwapChain* swapchain, SwapChainDesc desc)override;
		void createSurface(SwapChain* swapchain = nullptr, HWND hwnd = NULL);
		virtual void bindIndexBuffer(const GPUBuffer* indexBuffer, const IndexBufferFormat format, uint64_t offset, CommandList cmd) override;
		//bool createInternalSwapChain(SwapChain_Vulkan* internal_state);
		static constexpr uint32_t immutable_sampler_slot_begin = 100;
		std::unordered_map<size_t, VkPipeline> pipelines_global;
		VkSampler null_sampler = VK_NULL_HANDLE;
		VkImageView nullImageView1D = VK_NULL_HANDLE;
		VkImageView nullImageView2D = VK_NULL_HANDLE;
		VkImageView nullImageView3D = VK_NULL_HANDLE;
		VkImageView nullImageViewCube = VK_NULL_HANDLE;
		VkImageView nullImageView1DArray = VK_NULL_HANDLE;
		VkImageView nullImageView2DArray = VK_NULL_HANDLE;
		VkImageView nullImageViewCubeArray = VK_NULL_HANDLE;

		VkBuffer nullBuffer = VK_NULL_HANDLE;
	private:
		void initVulkan();
		void initVulkan2();
		void initVolk();
		void createInstance();
		void setupDebugMessenger();
		//void createSurface();
		void pickPhysicalDevice();
		void createLogicalDevice();
		void createSwapChain();
		void createImageViews();
		void createCommandPool();
		void createDepthResources();
		void createFramebuffers();
		void loadModel();
		void createDescriptorPool();
		void createCommandBuffers();
		void createSyncObjects();
		void createAllocator();
		VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
		void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
		void initInstanceFunction();


		bool isDeviceSuitable(VkPhysicalDevice device);
		bool checkDeviceExtensionSupport(VkPhysicalDevice device);
		//SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device,VkSurfaceKHR & surface);
		VkSampleCountFlagBits getMaxAvailableSamplerCount();
		//VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		CommandList_Vulkan& getCommandList(CommandList cmd)const;
		void createImage(uint32_t width, uint32_t height, VkFormat format, uint32_t mip_levels, VkSampleCountFlagBits num_samples, VkImageTiling tiling,
			VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
		void transitionImageLayout(VkImage image, VkFormat format, uint32_t mip_levels, VkImageLayout oldLayout, VkImageLayout newLayout);
		void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		VkImageView createImageView(VkImage image, VkFormat format, uint32_t mip_levels, VkImageAspectFlags aspectFlags);
		VkFormat findDepthFormat();
		VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
		VkShaderModule createShaderModule(const std::vector<char>& code);
		void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		void generateMipmaps(VkImage image, VkFormat format, uint32_t tex_width, uint32_t tex_height);
		void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		void recreateSwapChain();
		void cleanupSwapChain();
		void beginCommandBuffer();
		void createViewport();
		VkPipelineDynamicStateCreateInfo dynamic_state_info = {};
		std::vector<VkDynamicState> pso_dynamic_states = {};
		VkPipelineDynamicStateCreateInfo dynamic_state_info_mesh_shader = {};
		PFN_vkCmdBeginDebugUtilsLabelEXT _vkCmdBeginDebugUtilsLabelEXT;
		PFN_vkCmdEndDebugUtilsLabelEXT _vkCmdEndDebugUtilsLabelEXT;
		virtual void bindViewports(CommandList& cmd_list, uint32_t viewport_count, Viewport* viewport) override;
		virtual void beginRenderPass(SwapChain* swapchain, CommandList& cmd_list) override;
		virtual void beginRenderPass(const RenderPassImage *image, uint32_t size, CommandList cmd,RenderPassFlags flags = RenderPassFlags::NONE) override;
		virtual void endRenderPass(CommandList& cmd_list) override;
		virtual void bindStencilRef(uint32_t stencil_ref, CommandList cmd) override;
		virtual ShaderFormat getShaderFormat()const override { return ShaderFormat::SPIRV; };
		virtual bool createShader(ShaderStage stage, const void* shader_code, size_t shader_size, Shader* shader) override;
		virtual void bindConstantBuffer(const GPUBuffer* buffer, uint32_t slot, CommandList cmd, uint32_t offset = 0) override;
		virtual void pushConstants(const void* data, uint32_t size, CommandList cmd, uint32_t offset = 0) override;
		virtual void drawIndexedInstanced(uint32_t index_count, uint32_t instance_count, uint32_t start_index_location, int32_t base_vertex_location, uint32_t start_instance_location, CommandList cmd) override;
		virtual void copyBuffer(const GPUBuffer* dst_buffer, uint64_t dst_offset, const GPUBuffer* src_buffer, uint64_t src_offset, uint64_t size, CommandList cmd) override;
	public:
		
		VmaAllocator m_assets_allocator;
		VkRect2D m_scissor;

		VkImage depthImage;
		VkDeviceMemory depthImageMemory;
		VkImageView depth_image_view;

		uint8_t* m_p_current_frame_index{ nullptr };
		VkCommandPool* m_p_command_pools{ nullptr };
		VkCommandBuffer* m_p_command_buffers{ nullptr };
		VkCommandBuffer  m_current_command_buffer;
		VkFence frame_fences[BUFFER_COUNT][QUEUE_COUNT];
		std::unordered_map<uint32_t, std::shared_ptr<std::mutex>> queue_lockers;
	public:
		VkQueue graphics_queue;
		VkQueue present_queue;
		VkQueue compute_queue;
		VkQueue copy_queue;
		VkQueue video_queue;
		VkDevice m_device;
		VkInstance instance;
		VkDebugUtilsMessengerEXT debugMessenger;
		GLFWwindow* m_window{ nullptr };
		VkSurfaceKHR surface;
		VkPhysicalDevice physical_device;
		VkViewport m_viewport;

		const std::vector<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};

		VkSampleCountFlagBits msaa_samples = VK_SAMPLE_COUNT_1_BIT;

		VkSwapchainKHR swap_chain;
		std::vector<VkImage> swapChainImages;
		VkFormat swapChainImageFormat;
		VkFormat m_depth_image_format;
		VkExtent2D m_swapchain_extent;
		std::vector<VkImageView> swapChainImageViews;
		std::vector<VkFramebuffer> swapChainFramebuffers;

		VkRenderPass renderPass;
		VkDescriptorSetLayout descriptorSetLayout;
		VkPipelineLayout pipelineLayout;

		//base  render command pool
		VkCommandPool command_pool;

		//other command pool and command buffer
		VkCommandPool m_command_pools[MAX_FRAMES_IN_FLIGHT];

		VkImage colorImage;
		VkDeviceMemory colorImageMemory;
		VkImageView colorImageView;
		VkPhysicalDeviceProperties2 m_physical_device_properties_2{};
		VkPhysicalDeviceVulkan11Properties m_physical_device_properties_1_1 = {};
		VkPhysicalDeviceVulkan12Properties m_physical_device_properties_1_2 = {};
		VkPhysicalDeviceVulkan13Properties m_physical_device_properties_1_3 = {};

		VkPhysicalDeviceFeatures2 m_physical_device_features_2{};
		VkPhysicalDeviceVulkan11Features  m_physical_device_features_1_1{};
		VkPhysicalDeviceVulkan12Features  m_physical_device_features_1_2{};
		VkPhysicalDeviceVulkan13Features  m_physical_device_features_1_3{};

		QueueFamilyIndices queue_family_indices;
		std::vector<VkQueueFamilyProperties2> queue_families;
		std::vector<uint32_t> families;
		std::vector<VkQueueFamilyVideoPropertiesKHR> queue_families_video;
		uint32_t graphics_family = VK_QUEUE_FAMILY_IGNORED;
		uint32_t compute_family = VK_QUEUE_FAMILY_IGNORED;
		uint32_t transfer_family = VK_QUEUE_FAMILY_IGNORED;
		uint32_t video_family = VK_QUEUE_FAMILY_IGNORED;
		VkImage textureImage;
		VkDeviceMemory textureImageMemory;
		VkImageView textureImageView;
		VkSampler textureSampler;

		std::vector<MeshVertex> vertices;
		std::vector<uint32_t> indices;
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;

		std::vector<VkBuffer> uniformBuffers;
		std::vector<VkDeviceMemory> uniformBuffersMemory;
		std::vector<void*> uniformBuffersMapped;

		VkDescriptorPool m_descriptor_pool;
		std::vector<VkDescriptorSet> descriptorSets;

		std::vector<VkCommandBuffer> m_command_buffers;

		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> m_is_frame_in_flight_fences;
		std::vector<std::unique_ptr<CommandList_Vulkan>> commandlists;
		uint8_t m_current_frame_index = 0;

		bool framebufferResized = false;
		uint32_t mip_levels;
		uint32_t current_image_index = 0;

		float delta_time;
		float current_time;

		uint32_t                       m_vulkan_api_version{ VK_API_VERSION_1_0 };
		uint32_t cmd_count{ 0 };
		bool m_enable_debug_util{ true };
		static VkSampler m_nearest_sampler;
		SpinLock cmd_locker;
		std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		std::vector<const char*> enabled_extensions;

		struct AllocationHandler
		{
			VkDevice device = VK_NULL_HANDLE;
			uint64_t frame_count = 0;
			std::mutex destroy_locker;
			std::deque<std::pair<VkDescriptorPool, uint64_t>> destroy_descriptorPools;
			std::deque<std::pair<int, uint64_t>> destroyer_bindless_storage_buffers;
			std::deque<std::pair<std::pair<VkBuffer, VmaAllocation>, uint64_t>> destroyer_buffers;
			std::deque<std::pair<VkBufferView, uint64_t>> destroyer_bufferviews;
			std::deque<std::pair<uint32_t, uint64_t>> destroyer_bindless_uniform_texel_buffers;
			std::deque<std::pair<uint32_t, uint64_t>> destroyer_bindless_storage_texel_buffers;
			void update(uint64_t FRAME_COUNT, uint32_t BUFFER_COUNT)
			{
				const auto destroy = [&](auto&& queue, auto&& handler)
					{
						while (!queue.empty())
						{
							if (queue.front().second + BUFFER_COUNT < FRAME_COUNT)
							{
								auto item = queue.front();
								queue.pop_front();
								handler(item.first);
							}
							else
							{
								break;
							}
						}
					};
				frame_count = FRAME_COUNT;

				destroy_locker.lock();
				destroy(destroy_descriptorPools, [&](auto& item) {
					vkDestroyDescriptorPool(device, item, nullptr);
					});
				destroy(destroyer_bindless_storage_buffers, [&](auto& item)
					{
						bindless_storage_buffers.free(item);
					});
				destroy(destroyer_buffers, [&](auto& item)
					{
						vmaDestroyBuffer(allocator, item.first, item.second);
					});
				destroy(destroyer_bufferviews, [&](auto& item)
					{
						vkDestroyBufferView(device, item, nullptr);
					});
				destroy(destroyer_bindless_storage_texel_buffers, [&](auto& item)
					{
						bindless_storage_texel_buffers.free(item);
					});
				destroy(destroyer_bindless_uniform_texel_buffers, [&](auto& item)
					{
						bindless_uniform_texel_buffers.free(item);
					});

				destroy_locker.unlock();
			}
			struct BindlessDescriptorHeap
			{
				std::vector<unsigned int> free_list;
				VkDescriptorSet descriptor_set = VK_NULL_HANDLE;
				VkDescriptorPool descriptor_pool = VK_NULL_HANDLE;
				VkDescriptorSetLayout descriptor_set_layout = VK_NULL_HANDLE;
				std::mutex locker;
				uint32_t allocate()
				{
					locker.lock();
					if (!free_list.empty())
					{
						int index = free_list.back();
						free_list.pop_back();
						locker.unlock();
						return index;
					}
					locker.unlock();
					return -1;
				}

				void init(VkDevice device, VkDescriptorType type, uint32_t descriptor_count)
				{
					descriptor_count = std::min(descriptor_count, 500000u);
					VkDescriptorPoolSize pool_size{};
					pool_size.descriptorCount = descriptor_count;
					pool_size.type = type;

					VkDescriptorPoolCreateInfo pool_create_info{};
					pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
					pool_create_info.poolSizeCount = 1;
					pool_create_info.pPoolSizes = &pool_size;
					pool_create_info.maxSets = 1;
					pool_create_info.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;

					VkResult res = vkCreateDescriptorPool(device, &pool_create_info, nullptr, &descriptor_pool);
					assert(res == VK_SUCCESS);

					VkDescriptorSetLayoutBinding binding{};
					binding.descriptor_count = descriptor_count;
					binding.descriptorType = type;
					binding.binding = 0;
					binding.stageFlags = VK_SHADER_STAGE_ALL;
					binding.pImmutableSamplers = nullptr;

					VkDescriptorSetLayoutCreateInfo layout_info{};
					layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
					layout_info.bindingCount = 1;
					layout_info.pBindings = &binding;
					layout_info.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
					VkDescriptorSetLayoutBindingFlagsCreateInfo binding_flags_info{};
					binding_flags_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
					binding_flags_info.bindingCount = 1;
					VkDescriptorBindingFlags binding_flags =
						VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT |
						VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT |
						VK_DESCRIPTOR_BINDING_UPDATE_UNUSED_WHILE_PENDING_BIT;
					binding_flags_info.pBindingFlags = &binding_flags;
					layout_info.pNext = &binding_flags_info;

					res = vkCreateDescriptorSetLayout(device, &layout_info, nullptr, &descriptor_set_layout);
					assert(res == VK_SUCCESS);

					VkDescriptorSetAllocateInfo allocate_info{};
					allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
					allocate_info.descriptorPool = descriptor_pool;
					allocate_info.descriptorSetCount = 1;
					allocate_info.pSetLayouts = &descriptor_set_layout;
					res = vkAllocateDescriptorSets(device, &allocate_info, &descriptor_set);
					assert(res == VK_SUCCESS);

					for (int i = 0; i < (int)descriptor_count; ++i)
					{
						free_list.push_back((int)descriptor_count - i - 1);
					}
				}

				void free(int index)
				{
					if (index >= 0)
					{
						locker.lock();
						free_list.push_back(index);
						locker.unlock();
					}
				}
			};
			BindlessDescriptorHeap bindless_samplers;
			BindlessDescriptorHeap bindless_storage_buffers;
			BindlessDescriptorHeap bindless_uniform_texel_buffers;
			BindlessDescriptorHeap bindless_storage_texel_buffers;
			BindlessDescriptorHeap bindless_sampled_images;
			BindlessDescriptorHeap bindless_storage_images;
			BindlessDescriptorHeap bindless_acceleration_structures;
			VmaAllocator allocator = nullptr;
		};
		std::shared_ptr<AllocationHandler> allocation_handler;
	};
}

template<>
struct enable_bitmask_operators<qyhs::VulkanRHI::DescriptorBinder::DIRTY_FLAGS>
{
	static constexpr bool enable = true;
};