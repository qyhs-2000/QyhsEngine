#include "render_pipeline_base.h"

void QYHS::RenderPipelineBase::prepareDataForRenderPass(std::shared_ptr<RenderResourceBase> resource)
{
	m_main_camera_pass->prepareData(resource);
	m_pick_pass->prepareData(resource);
}
