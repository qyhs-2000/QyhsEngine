#pragma once
#include "function/render/render_pass.h"


namespace  qyhs
{
    struct PickPassInitInfo:public RenderPassInitInfo
    {
        VkDescriptorSetLayout* per_mesh_layout;
    };

    class PickPass:public RenderPass
    {
    public:
        virtual void initialize(RenderPassInitInfo * info) override;
        void setupAttachments();
        void setupRenderPass();
        void setupDescriptorSetLayout();
        void setupDescriptorSet();
        void setupFrameBuffer();
        void setupPipeline();
        void recreateFrameBuffer();
        virtual void prepareData(std::shared_ptr<RenderResourceBase> resource) override;
        uint32_t pick(const Vector2 & pick_uv);    //return picked mesh_id by pick_uv
        MeshInefficientPickPerFrameStorageBuffer mesh_inefficient_pick_per_frame_storage_buffer;
        VkDescriptorSetLayout* per_mesh_descriptor_set_layout;
    private:
    };
}
