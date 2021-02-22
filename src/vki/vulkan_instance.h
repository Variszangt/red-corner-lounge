#pragma once

#include <vector>
#include <string>

#include <vulkan/vulkan.hpp>

#include "config.h"

namespace vki
{
struct InstanceCreateInfo
{
    std::string                 application_name;
    uint32_t                    application_version;
    std::vector<const char*>    required_extensions;
    VulkanDebug                 debug; // If debug is enabled, instance will be created with validation layers and debug-utils extension enabled (see also: vulkan_debug.h).
};
vk::UniqueInstance create_instance(const InstanceCreateInfo& ci);
}