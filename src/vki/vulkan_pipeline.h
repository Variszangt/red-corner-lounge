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
PipelineWrapper create_world_pipeline(const DeviceWrapper& device_wrapper);
}