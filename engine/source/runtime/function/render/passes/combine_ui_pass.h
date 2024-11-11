#pragma once
#include "function/render/render_pass.h"
namespace QYHS
{
	struct CombineUIPassInitInfo:public RenderPassInitInfo
	{
		VkRenderPass *render_pass;
		VkImageView* scene_input_attachment;
		VkImageView* ui_input_attachment;
	};

	//combine scene attachment and ui attachment
	class CombineUIPass:public RenderPass
	{
	public:
		virtual void initialize(RenderPassInitInfo * info) override;
		void setupDescriptorSetLayout();
		void setupPipelines();
		void setupDescriptorSet();
		void draw();
		void updateAfterRecreateSwapChain(VkImageView* input_color_attachment, VkImageView* inpput_ui_attachment);
	private:
		VkImageView* m_scene_attachment;
		VkImageView* m_ui_attachment;
	};
}