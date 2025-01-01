#include "ui_pass.h"

#include <imgui/imgui.h>

#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_vulkan.h"

namespace QYHS
{
    void UIPass::initialize(RenderPassInitInfo* info)
    {
        RenderPass::initialize(info);
        m_framebuffer.render_pass = *static_cast<UIPassInitInfo*>(info)->render_pass;
    }

    void UIPass::draw()
    {
        if(!m_window_ui) return;
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        m_window_ui->preRender();

        m_vulkan_rhi->beginEvent(m_vulkan_rhi->getCurrentCommandBuffer(),"ImGui");
        ImGui::Render();
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(),m_vulkan_rhi->getCurrentCommandBuffer());

        m_vulkan_rhi->endEvent(m_vulkan_rhi->getCurrentCommandBuffer());
    }
    void UIPass::initializeUIRenderBackend(WindowUI* window_ui)
    {
        m_window_ui = window_ui;
        
        ImGui_ImplGlfw_InitForVulkan(std::static_pointer_cast<VulkanRHI>(m_rhi)->m_window, true);
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance                  = std::static_pointer_cast<VulkanRHI>(m_rhi)->instance;
        init_info.PhysicalDevice            = std::static_pointer_cast<VulkanRHI>(m_rhi)->physical_device;
        init_info.Device                    = std::static_pointer_cast<VulkanRHI>(m_rhi)->m_device;
        init_info.QueueFamily               = m_vulkan_rhi->findQueueFamilies(m_vulkan_rhi->getPhysicalDevice()).graphics_family.value();
        init_info.Queue                     = m_vulkan_rhi->getGraphicsQueue();
        init_info.DescriptorPool            = std::static_pointer_cast<VulkanRHI>(m_rhi)->m_descriptor_pool;
        init_info.Subpass                   = main_camera_subpass_ui;

        // may be different from the real swapchain image count
        // see ImGui_ImplVulkanH_GetMinImageCountFromPresentMode
        init_info.MinImageCount = 3;
        init_info.ImageCount    = 3;
        ImGui_ImplVulkan_Init(&init_info, m_framebuffer.render_pass);

        uploadFonts();
    }

    void UIPass::uploadFonts()
    {
        VkCommandBufferAllocateInfo allocinfo = {};
        allocinfo.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocinfo.level                       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocinfo.commandPool                 = m_vulkan_rhi->command_pool;
        allocinfo.commandBufferCount          = 1;

        VkCommandBuffer* commandbuffer = new VkCommandBuffer();
        if (VK_SUCCESS != m_vulkan_rhi->allocatecommandbuffers(&allocinfo, commandbuffer))
        {
            throw std::runtime_error("failed to allocate command buffers!");
        }

        VkCommandBufferBeginInfo begininfo = {};
        begininfo.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begininfo.flags                    = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        if (VK_SUCCESS != m_vulkan_rhi->beginCommandBuffer(commandbuffer, &begininfo))
        {
            throw std::runtime_error("could not create one-time command buffer!");
        }
        ImGui_ImplVulkan_CreateFontsTexture(*commandbuffer);

        if (VK_SUCCESS != m_vulkan_rhi->endCommandBuffer(commandbuffer))
        {
            throw std::runtime_error("failed to record command buffer!");
        }

        VkSubmitInfo submitinfo {};
        submitinfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitinfo.commandBufferCount = 1;
        submitinfo.pCommandBuffers    = commandbuffer;

        m_vulkan_rhi->queueSubmit(m_vulkan_rhi->getGraphicsQueue(), 1, &submitinfo, VK_NULL_HANDLE);
        m_vulkan_rhi->queueWaitIdle(m_vulkan_rhi->getGraphicsQueue());

        m_vulkan_rhi->freeCommandBuffers(m_vulkan_rhi->command_pool, 1, commandbuffer);
        ImGui_ImplVulkan_DestroyFontUploadObjects();
        
    }
}
