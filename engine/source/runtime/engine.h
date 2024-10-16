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
	
	class QyhsEngine
	{
	public:
		void run() {
			initWindow();
			loadAssets();
			mainLoop();
		}
		bool tick(double delta_time);
		void logicTick(double delta_time);
		void renderTick();
		void loadAssets();
		void loadglTFFile(const std::string& path);
	private:
		GLFWwindow* window;

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

		 

		void initWindow() {
			glfwInit();

			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

			window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

		}


		void mainLoop() {
			while (!glfwWindowShouldClose(window)) {
				glfwPollEvents();
				drawFrame();
			}

			vkDeviceWaitIdle(device);
		}

		bool hasStencilComponent(VkFormat format) {
			return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
		}

		void drawFrame() {
			
		}
		
	public:
		void startEngine(const std::string& config_file_path);
		void initialize();
		double caculateDeltaTime();
	private:
		double current_time;
	};
}