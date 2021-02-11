#pragma once

#include <vector>
#include <string>
#include <span>

#include <vulkan/vulkan.hpp>

#include "config.h"

namespace vki
{
struct InstanceInfo
{
    std::string                 application_name    = nullptr;
    uint32_t                    application_version = 0;
    VulkanDebug                 debug               = VulkanDebug::Off; // If debug is enabled, instance will be created with validation layers and debug-utils extension (see also: vulkan_debug.h).
    std::span<const char*>      required_extensions;
};

class Instance
{
public:
    void init(const InstanceInfo& info);

    vk::Instance operator->() { return instance.get(); }
    vk::Instance operator*() { return instance.get(); }
    operator vk::Instance() { return instance.get(); }

private:
    vk::UniqueInstance instance;
    vk::UniqueDebugUtilsMessengerEXT debug_messenger;
};
}