#include "render_pipeline_base.h"
#include "passes/ui_pass.h"
void QYHS::RenderPipelineBase::prepareDataForRenderPass(std::shared_ptr<RenderResourceBase> resource)
{
	m_main_camera_pass->prepareData(resource);
	m_pick_pass->prepareData(resource);
}

void QYHS::RenderPipelineBase::initializeUIRenderBackend(WindowUI* window_ui)
{
	static_cast<UIPass*>(m_ui_pass.get())->initializeUIRenderBackend(window_ui);
}
