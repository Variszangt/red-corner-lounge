#include "vulkan_renderpass.h"

namespace vki
{
vk::UniqueRenderPass create_renderpass(
    const DeviceWrapper&    device_wrapper,
    const vk::Format        color_format,
    const vk::Format        depth_stencil_format,
    const RenderPassType    /*type*/)
{
    auto device = device_wrapper.get();
    assert(device);

    /*------------------------------------------------------------------*/
    // Attachments:

    // Color:
    const vk::AttachmentDescription color_attachment {
        .format         = color_format,
        .samples        = vk::SampleCountFlagBits::e1,
        .loadOp         = vk::AttachmentLoadOp::eClear,
        .storeOp        = vk::AttachmentStoreOp::eStore,
        .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .initialLayout  = vk::ImageLayout::eUndefined,
        .finalLayout    = vk::ImageLayout::ePresentSrcKHR,
    };

    // Depth-Stencil:
    const vk::AttachmentDescription depth_stencil_attachment {
        .format         = depth_stencil_format,
        .samples        = vk::SampleCountFlagBits::e1,
        .loadOp         = vk::AttachmentLoadOp::eClear,
        .storeOp        = vk::AttachmentStoreOp::eStore,
        .stencilLoadOp  = vk::AttachmentLoadOp::eClear,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .initialLayout  = vk::ImageLayout::eUndefined,
        .finalLayout    = vk::ImageLayout::eDepthStencilAttachmentOptimal,
    };

    const std::vector<vk::AttachmentDescription> attachments {
        color_attachment,
        depth_stencil_attachment,
    };

    /*------------------------------------------------------------------*/
    // References:

    const vk::AttachmentReference color_reference {
        .attachment = 0,
        .layout     = vk::ImageLayout::eColorAttachmentOptimal,
    };

    const vk::AttachmentReference depth_stencil_reference = {
        .attachment = 1,
        .layout     = vk::ImageLayout::eDepthStencilAttachmentOptimal,
    };

    /*------------------------------------------------------------------*/
    // Subpasses:

    const vk::SubpassDescription main_subpass {
        .pipelineBindPoint          = vk::PipelineBindPoint::eGraphics,
        .colorAttachmentCount       = 1,
        .pColorAttachments          = &color_reference,
        .pDepthStencilAttachment    = &depth_stencil_reference,
    };

    const std::vector<vk::SubpassDescription> subpasses {
        main_subpass,
    };

    /*------------------------------------------------------------------*/
    // Subpass dependencies:

    const vk::SubpassDependency enter_main_subpass {
        .srcSubpass         = VK_SUBPASS_EXTERNAL,
        .dstSubpass         = {},
        .srcStageMask       = vk::PipelineStageFlagBits::eAllCommands,
        .dstStageMask       = vk::PipelineStageFlagBits::eColorAttachmentOutput,
        .srcAccessMask      = vk::AccessFlagBits::eMemoryRead,
        .dstAccessMask      = vk::AccessFlagBits::eColorAttachmentWrite,
        .dependencyFlags    = vk::DependencyFlagBits::eByRegion,
    };

    const vk::SubpassDependency exit_main_subpass {
        .srcSubpass         = {},
        .dstSubpass         = VK_SUBPASS_EXTERNAL,
        .srcStageMask       = vk::PipelineStageFlagBits::eColorAttachmentOutput,
        .dstStageMask       = vk::PipelineStageFlagBits::eBottomOfPipe,
        .srcAccessMask      = vk::AccessFlagBits::eColorAttachmentWrite,
        .dstAccessMask      = vk::AccessFlagBits::eMemoryRead,
        .dependencyFlags    = vk::DependencyFlagBits::eByRegion,
    };

    const std::vector<vk::SubpassDependency> subpass_dependencies {
        enter_main_subpass,
        exit_main_subpass,
    };

    /*------------------------------------------------------------------*/
    // Create:

    const vk::RenderPassCreateInfo createinfo {
        .attachmentCount    = static_cast<uint32_t>(attachments.size()),
        .pAttachments       = attachments.data(),
        .subpassCount       = static_cast<uint32_t>(subpasses.size()),
        .pSubpasses         = subpasses.data(),
        .dependencyCount    = static_cast<uint32_t>(subpass_dependencies.size()),
        .pDependencies      = subpass_dependencies.data(),
    };
    return device.createRenderPassUnique(createinfo);
}

}