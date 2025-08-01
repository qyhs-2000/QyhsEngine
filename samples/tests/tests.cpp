#include "tests.h"
#include <iostream>
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_win32.h"

#include "core/utils/model_importer_gltf.h"
namespace qyhs
{
	void Tests::initialize2()
	{
		QyhsEngine::initialize2();

		//renderer.init(canvas);
		renderer.setWindow(window);
		renderer.load();
		activatePath(&renderer);
	}

	void TestRenderer::load()
	{
		
		gui::Gui& gui = GetGUI();

		test_selector.create("TestSelector");
		test_selector.setText("Demo: ");
		test_selector.setSize(Vector2(140, 20));
		test_selector.setPosition(Vector2(50, 220));
		test_selector.addItem("HelloTriangle",TestType::HELLOTRIANGLE);
		test_selector.addItem("CesiumMan",TestType::CesiumMan);
		test_selector.addItem("ImGui", TestType::ImGui);
		test_selector.onSelect([=](gui::EventArgs args){
			switch (args.userdata)
			{
			case HELLOTRIANGLE:
			{
				import_model_gltf(&scene, std::string("E://GithubClone//WickedEngine//Content//models//white_triangle.gltf"));
			}
			break;
			case CesiumMan:
			{
				import_model_gltf(&scene, "D://vs_project//QyhsEngine//engine//source//runtime//resource//model//CesiumMan//glTF-Embedded//CesiumMan_test.gltf");
			}
			break;
			case ImGui:
			{
				std::cout << "Hello ImGui" << std::endl;
				//setupImguiContext();
			}
			break;
			default:
				break;
			}
			});
		test_selector.setSelected(2);
		gui.addWeights(&test_selector);
		
		RenderPath3D::load();
	}

	void TestRenderer::render()
	{
		RenderPath3D::render();
		
	}

	void TestRenderer::start()
	{
		RenderPath3D::start();
		
	}

	static int ImGui_ImplWin32_CreateVkSurface(ImGuiViewport* viewport, ImU64 vk_instance, const void* vk_allocator, ImU64* out_vk_surface)
	{
		VkWin32SurfaceCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo.hwnd = (HWND)viewport->PlatformHandleRaw;
		createInfo.hinstance = ::GetModuleHandle(nullptr);
		return (int)vkCreateWin32SurfaceKHR((VkInstance)vk_instance, &createInfo, (VkAllocationCallbacks*)vk_allocator, (VkSurfaceKHR*)out_vk_surface);
	}

	void TestRenderer::setupImguiContext()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		// Setup Platform/Renderer backends
		ImGui_ImplWin32_Init(m_window);
		ImGui::GetPlatformIO().Platform_CreateVkSurface = ImGui_ImplWin32_CreateVkSurface;

		VulkanRHI* rhi = static_cast<VulkanRHI*>(rhi::getRHI());

		
		rhi->initImgui();

		
	}

}