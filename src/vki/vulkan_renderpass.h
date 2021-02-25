#pragma once

#include <vulkan/vulkan.hpp>

#include "vulkan_device.h"

namespace vki
{
enum class RenderPassType
{
    ColorAndDepthStencil,
};

vk::UniqueRenderPass create_renderpass(
    const DeviceWrapper&    device_wrapper,
    const vk::Format        color_format,
    const vk::Format        depth_stencil_format,
    const RenderPassType    /*type*/);

}