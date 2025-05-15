#pragma once
//#include <filesystem>
//#include <thread>

#include <fstream>
#include <stdexcept>
#include <function/ui/render_path.h>
//make max and min function in windows.h unvalid


#ifdef VK_USE_PLATFORM_ANDROID_KHR
#	define TINYGLTF_ANDROID_LOAD_FROM_ASSETS
#endif

#include "function/render/rhi/vulkan/vulkan_rhi.h"
#include "function/timer/timer.h"
#include "function/ui/canvas.h"
namespace qyhs
{
	extern std::unordered_set<std::string> g_editor_tick_component_types;
	extern bool g_is_editor_mode;
	
	class QyhsEngine
	{
	public:
		Canvas canvas;
	public:
		QyhsEngine();
		bool tick(double delta_time);
		void logicTick(double delta_time);
		void renderTick();
		void createWindow();
		virtual void update(float delta_time);
		bool shouldWindowClose();
		void startEngine(const std::string& config_file_path);
		virtual void initialize();
		double caculateDeltaTime();
		void setConfigFile(std::string file_path) { engine_config_file = file_path; }
		void run();
		virtual void run2();
		virtual void render();
		bool is_window_active{ true };
		void setWindow(platform::WindowType hwnd);
		void compose(CommandList& cmd_list);
		void showInformation();
		virtual void initialize2();
		virtual void activatePath(RenderPath * render_path);
		inline RenderPath* getActivePath() { return active_path; }
	protected:
		RenderPath* active_path;
	private:
		platform::WindowType window;
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
		struct InfoDisplayer
		{
			bool active = false;
			bool watermark = true;
			bool fpsinfo = false;
			bool device_name = false;
			bool resolution = false;
			bool logical_size = false;
			bool colorspace = false;
			bool heap_allocation_counter = false;
			bool pipeline_count = false;
			bool pipeline_creation = false;
			bool vram_usage = false;
			int size = 16;
			bool colorgrading_helper = false;
			
		} info_display;

		Timer timer;
		float target_frame_rate{ 60.0f };
	protected:
		std::string engine_config_file;
		SwapChain swapchain;
	private:
		double current_time;
		bool initialized{ false };
		bool frame_rate_lock{ false };
		std::shared_ptr<RHI> m_rhi = std::make_shared<VulkanRHI>();
	};
}