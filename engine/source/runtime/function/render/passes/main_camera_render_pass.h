#pragma once
#include "function/render/render_pass.h"
#include "function/render/render_common.h"
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
    static float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    //static float skyboxVertices[] = {
    //    // positions          
    //    0.f,  .0f, -.0f,
    //    -.0f, -.0f, -.0f,
    //     0.0f, -0.0f, -0.0f,
    //     0.0f, -0.0f, -0.0f,
    //     0.0f,  0.0f, -0.0f,
    //    -0.0f,  0.0f, -0.0f,

    //    0.f,  .0f, -.0f,
    //    -.0f, -.0f, -.0f,
    //     0.0f, -0.0f, -0.0f,
    //     0.0f, -0.0f, -0.0f,
    //     0.0f,  0.0f, -0.0f,
    //    -0.0f,  0.0f, -0.0f,

    //     0.f,  .0f, -.0f,
    //    -.0f, -.0f, -.0f,
    //     0.0f, -0.0f, -0.0f,
    //     0.0f, -0.0f, -0.0f,
    //     0.0f,  0.0f, -0.0f,
    //    -0.0f,  0.0f, -0.0f,

    //    0.f,  .0f, -.0f,
    //    -.0f, -.0f, -.0f,
    //     0.0f, -0.0f, -0.0f,
    //     0.0f, -0.0f, -0.0f,
    //     0.0f,  0.0f, -0.0f,
    //    -0.0f,  0.0f, -0.0f,

    //    0.f,  .0f, -.0f,
    //    -.0f, -.0f, -.0f,
    //     0.0f, -0.0f, -0.0f,
    //     0.0f, -0.0f, -0.0f,
    //     0.0f,  0.0f, -0.0f,
    //    -0.0f,  0.0f, -0.0f,

    //    0.f,  .0f, -.0f,
    //    -.0f, -.0f, -.0f,
    //     0.0f, -0.0f, -0.0f,
    //     0.0f, -0.0f, -0.0f,
    //     0.0f,  0.0f, -0.0f,
    //    -0.0f,  0.0f, -0.0f
    //};

	class MainCameraRenderPass :public RenderPass
	{
	public:
	enum RenderPipelineType
	{
		render_pipeline_type_mesh_global_buffer = 0,
		render_pipeline_type_skybox = 1,
		render_pipeline_type_count
	};

	enum DescriptorSetLayoutType
	{
		global_mesh=0,
		mesh_per_material,
		skybox,
		descriptor_set_layout_type_count
	};

	enum MainCameraPassAttachment
	{
		main_camera_pass_base_color_attachment = 0,
		main_camera_pass_depth_attachment ,
		//main_camera_pass_skybox_attachment,
	    main_camera_pass_swap_chain_image_attachment ,
		main_camera_pass_attachment_count 
	};
		void draw();
		void setupRenderPass();
		void setupAttachments();
		void setupRenderPipelines();
		void setupDescriptorSetLayout();
		void setupDescriptorSets();
		virtual void initialize() override final;
		void setupSwapChainFrameBuffers();
		void drawMesh();
		void drawSkyBox();
		void drawMSAA();
		void setupGlobalModelDescriptorSet();
		void setupSkyboxDescriptorSet();
        void setupSkyboxCubeBuffer();
		virtual void prepareData(std::shared_ptr<RenderResourceBase> resource) override;
		MeshPerFrameStorageBufferObject m_mesh_perframe_storage_buffer_object;
        VkBuffer skybox_cube_vertex_buffer;
        VkDeviceMemory skybox_cube_vertex_buffer_memory;
	private:
		std::vector<VkFramebuffer> m_swapchain_framebuffers;
	
	};
}