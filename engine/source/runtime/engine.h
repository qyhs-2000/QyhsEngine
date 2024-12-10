#pragma once
//#include <filesystem>
//#include <thread>

#include <fstream>
#include <stdexcept>


#ifdef VK_USE_PLATFORM_ANDROID_KHR
#	define TINYGLTF_ANDROID_LOAD_FROM_ASSETS
#endif

#include "function/render/rhi/vulkan/vulkan_rhi.h"


namespace QYHS
{
	extern bool g_is_editor_mode;
	class QyhsEngine
	{
	public:
		
		bool tick(double delta_time);
		void logicTick(double delta_time);
		void renderTick();
		void loadAssets();
		void loadglTFFile(const std::string& path);
		void createWindow();
		virtual void update(float delta_time);
		bool shouldWindowClose();
		void startEngine(const std::string& config_file_path);
		virtual void initialize();
		double caculateDeltaTime();
		void setConfigFile(std::string file_path) { engine_config_file = file_path; }
		void run();
	private:

		VkInstance instance;
		VkDebugUtilsMessengerEXT debugMessenger;
		VkSurfaceKHR surface;

		VkPhysicalDevice physical_device = VK_NULL_HANDLE;
		VkDevice device;

		VkQueue graphicsQueue;
		VkQueue presentQueue;

		VkSwapchainKHR swapChain;
		std::vector<VkImage> swapChainImages;
		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;
		std::vector<VkImageView> swapChainImageViews;
		std::vector<VkFramebuffer> swapChainFramebuffers;

		bool hasStencilComponent(VkFormat format) {
			return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
		}
		
	public:
		
	protected:
		std::string engine_config_file;
	private:
		double current_time;
		bool initialized{ false };
	};
}