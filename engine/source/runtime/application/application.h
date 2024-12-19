#pragma once
#include <Windows.h>
#include <core/utils/timer.h>
#include <memory>
#include <function/render/rhi/rhi.h>
#include <function/render/ui/canvas.h>
namespace QYHS
{
	class EventManager;
	class RenderPath;

	class Application
	{
	public:
		virtual void run();
		void setWindow(HWND hwnd);
		virtual void update(float delta_time);
		virtual void render();
		virtual void initialize();
		virtual void compose(CommandList cmd);
		Timer timer;
		RenderPath* getActiveRenderPath() { return current_active_path; }
		Canvas canvas;
		SwapChain swap_chain;
	private:
		HWND m_hwnd;
		unsigned int target_frame_rate{ 60 };
		bool frame_rate_lock{ false };
		bool initialized{ false };
		float delta_time{ 0.0f };
		RenderPath* current_active_path{ nullptr };
		std::shared_ptr<EventManager> event_manager;
		std::shared_ptr<RHI> m_rhi;
	};
}