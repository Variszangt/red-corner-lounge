#pragma once

#include <vulkan/vulkan.hpp>

#include "vertex.h"
#include "vulkan_device.h"

namespace vki
{
struct PipelineWrapper
{
    vk::UniquePipeline              pipeline;
    vk::UniqueDescriptorSetLayout   descriptor_set_layout;
    vk::UniqueRenderPass            renderpass;
};

PipelineWrapper create_world_pipeline(
    const DeviceWrapper&    device_wrapper,
    const vk::Format        color_format,
    const vk::Format        depth_stencil_format,
    const vk::Extent2D      initial_extent);
}