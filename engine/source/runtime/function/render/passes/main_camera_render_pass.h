#pragma once
#include "function/render/render_pass.h"
#include "function/render/render_common.h"
#include "ui_pass.h"
#include "combine_ui_pass.h"
namespace QYHS
{
	static float cubeVertices[] = {
        // positions          // normals
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    struct MainCameraPassInitInfo :public RenderPassInitInfo
    {

    };

	class MainCameraRenderPass :public RenderPass
	{
	public:
	enum RenderPipelineType
	{
		render_pipeline_type_mesh_global_buffer = 0,
		render_pipeline_type_skybox = 1,
		render_pipeline_type_axis = 2,
		render_pipeline_type_count
	};

	enum DescriptorSetLayoutType
	{
		global_mesh=0,
        per_mesh,
		mesh_per_material,
		skybox,
		axis,
		descriptor_set_layout_type_count
	};

	
		void draw(UIPass & ui_pass,CombineUIPass & combine_ui_pass);
		void setupRenderPass();
		void setupAttachments();
		void setupRenderPipelines();
		void setupDescriptorSetLayout();
		void setupDescriptorSets();
		virtual void initialize(RenderPassInitInfo * info) override final;
		void setupSwapChainFrameBuffers();
		void drawMesh();
		void drawSkyBox();
		void drawMSAA();
		void setupGlobalModelDescriptorSet();
		void setupSkyboxDescriptorSet();
		void setAxisDescriptorSet();
        void updateAfterRecreateSwapChain();
        void setVisibleAxis(bool state);
        void drawAxis();
        VkRenderPass &getRenderPass() { return m_framebuffer.render_pass; }
        
		virtual void prepareData(std::shared_ptr<RenderResourceBase> resource) override;
		MeshPerFrameStorageBufferObject m_mesh_perframe_storage_buffer_object;
        VkBuffer skybox_cube_vertex_buffer;
        VkDeviceMemory skybox_cube_vertex_buffer_memory;
		size_t m_selected_axis = 3;
		AxisStorageBufferObject m_axis_storage_buffer_object;
	private:
		std::vector<VkFramebuffer> m_swapchain_framebuffers;
        bool m_axis_show{ false };
	};
}