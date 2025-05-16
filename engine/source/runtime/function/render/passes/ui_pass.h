#pragma once
#include "function/render/render_pass.h"
#include "function/render/ui/window_ui.h"

namespace qyhs
{
	struct UIPassInitInfo :public RenderPassInitInfo
	{
		VkRenderPass* render_pass;
	};

	class UIPass:public RenderPass
	{
	public:

		virtual void initialize(RenderPassInitInfo * info) override;
		void draw();
		void initializeUIRenderBackend(WindowUI* window_ui);
		void uploadFonts();
	private:
		WindowUI* m_window_ui;
	};
}
