#pragma once

#include <vector>
#include <string>

#include <vulkan/vulkan.hpp>

#include "config.h"

namespace vki
{
struct InstanceInitInfo
{
    std::string                 application_name    = nullptr;
    uint32_t                    application_version = 0;
    VulkanDebug                 debug               = VulkanDebug::Off; // If debug is enabled, instance will be created with validation layers and debug-utils extension enabled (see also: vulkan_debug.h).
    std::vector<const char*>    required_extensions;
};

vk::UniqueInstance create_instance(const InstanceInitInfo& info);
}