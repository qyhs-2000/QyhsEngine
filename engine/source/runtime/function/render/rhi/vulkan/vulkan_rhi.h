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
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>
#include "function/render/rhi/rhi.h"
#include "function/render/model.h"
#include "vulkanmemoryallocator/include/vk_mem_alloc.h"


struct Vertex {
	glm::vec3 pos;
	//glm::vec3 color;
	glm::vec2 texCoord;

	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, texCoord);

		return attributeDescriptions;
	}

	bool operator==(const Vertex& other) const {
		return pos == other.pos  && texCoord == other.texCoord;
	}
};


const uint32_t WIDTH = 2500;
const uint32_t HEIGHT = 1600;

const std::string MODEL_PATH = "../../asset/objects/house.obj";
const std::string TEXTURE_PATH = "E:/VS_Project/QyhsEngine/asset/objects/_textures/n.tga";

const int MAX_FRAMES_IN_FLIGHT = 3;

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif



struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

namespace std {
	template<> struct hash<Vertex> {
		size_t operator()(Vertex const& vertex) const {
			return ((hash<glm::vec3>()(vertex.pos)  ^ (hash<glm::vec2>()(vertex.texCoord) << 1)));
		}
	};
}

struct UniformBufferObject {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};
namespace QYHS
{
	class VulkanRHI final :public RHI
	{
	public:
		virtual ~VulkanRHI() override final;
		void initialize();
		void cleanup();
		VkCommandBuffer beginSingleTimeCommands();

		void endSingleTimeCommands(VkCommandBuffer commandBuffer);
		void waitForFence();
		void getNextImage();
		void updateUniformBuffer();
		void resetFence();
		void resetCommandBuffer();
		void recordCommandBuffer();
		void submitCommandBuffer();
		int getMaxFrameInFlight() { return MAX_FRAMES_IN_FLIGHT; }
		VkDevice getDevice() { return device; }
		VkPhysicalDevice getPhysicalDevice() { return physical_device; }
		VkFormat getSwapChainImageFormat() { return swapChainImageFormat; }
		VkSampleCountFlagBits getMSAASamples() { return VK_SAMPLE_COUNT_1_BIT; }
		VkFormat getDepthImageFormat() { return m_depth_image_format; }
		VkExtent2D getSwapChainExtent() { return swapChainExtent; }
		VkCommandBuffer& getCurrentCommandBuffer() { return commandBuffers[m_current_frame_index]; }
		VkBuffer getVertexBuffer() { return vertexBuffer; }
		VkBuffer getIndexBuffer() { return indexBuffer; }
		std::vector<uint32_t>& getIndices() { return indices; }
		VkDescriptorPool& getDescriptorPool();
		VkBuffer getUniformBuffer(uint32_t index) {
			return uniformBuffers[index];
		}
		VkImageView& getTextureImageView() { return textureImageView; }
		VkSampler& getTextureSampler() { return textureSampler; }
		uint32_t getCurrentFrameIndex() { return m_current_frame_index; }
		std::vector<VkImageView>& getSwapChainImageView()
		{
			return swapChainImageViews;
		}
		VkViewport &getViewport() { return m_viewport; }
		void prepareBeforeRender();
	private:
		void initVulkan();

		void createInstance();
		void setupDebugMessenger();
		void createSurface();
		void pickPhysicalDevice();
		void createLogicalDevice();
		void createSwapChain();
		void createImageViews();
		void createRenderPass();
		void createDescriptorSetLayout();
		void createGraphicsPipeline();
		void createCommandPool();
		void createDepthResources();
		void createColorResource();
		void createFramebuffers();
		void createTextureImage();
		void createTextureImageView();
		void createTextureSampler();
		void loadModel();
		void initCamera();
		void loadAssets();
		void loadglTFFile(const std::string& path);
		void createVertexBuffer();
		void createIndexBuffer();
		void createUniformBuffers();
		void createDescriptorPool();
		void createDescriptorSets();
		void createCommandBuffers();
		void createSyncObjects();
		void createAllocator();
		VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
		void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
		bool isDeviceSuitable(VkPhysicalDevice device);
		bool checkDeviceExtensionSupport(VkPhysicalDevice device);
		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
		VkSampleCountFlagBits getMaxAvailableSamplerCount();
		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

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

	public:
		VmaAllocator m_assets_allocator;
	private:
		VkCommandPool command_pool;
		VkQueue graphics_queue;
		VkQueue present_queue;
		VkDevice device;
		VkInstance instance;
		VkDebugUtilsMessengerEXT debugMessenger;
		GLFWwindow* m_window;
		VkSurfaceKHR surface;
		VkPhysicalDevice physical_device;
		VkViewport m_viewport;

		VkSampleCountFlagBits msaa_samples = VK_SAMPLE_COUNT_1_BIT;

		VkSwapchainKHR swap_chain;
		std::vector<VkImage> swapChainImages;
		VkFormat swapChainImageFormat;
		VkFormat m_depth_image_format;
		VkExtent2D swapChainExtent;
		std::vector<VkImageView> swapChainImageViews;
		std::vector<VkFramebuffer> swapChainFramebuffers;

		VkRenderPass renderPass;
		VkDescriptorSetLayout descriptorSetLayout;
		VkPipelineLayout pipelineLayout;
		VkPipeline graphicsPipeline;

		VkCommandPool commandPool;

		VkImage depthImage;
		VkDeviceMemory depthImageMemory;
		VkImageView depthImageView;

		VkImage colorImage;
		VkDeviceMemory colorImageMemory;
		VkImageView colorImageView;


		VkImage textureImage;
		VkDeviceMemory textureImageMemory;
		VkImageView textureImageView;
		VkSampler textureSampler;

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;

		std::vector<VkBuffer> uniformBuffers;
		std::vector<VkDeviceMemory> uniformBuffersMemory;
		std::vector<void*> uniformBuffersMapped;

		VkDescriptorPool descriptorPool;
		std::vector<VkDescriptorSet> descriptorSets;

		std::vector<VkCommandBuffer> commandBuffers;

		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;
		uint32_t m_current_frame_index = 0;

		bool framebufferResized = false;
		uint32_t mip_levels;
		uint32_t current_image_index = 0;

		float delta_time;
		float current_time;

		GLTFModel m_model;
		uint32_t                       m_vulkan_api_version {VK_API_VERSION_1_0};
		
	};
}