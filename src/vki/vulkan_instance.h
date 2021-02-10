#pragma once

#include <vector>
#include <string>
#include <span>

#include <vulkan/vulkan.hpp>

#include "config.h"

/*------------------------------------------------------------------*/

struct VulkanInstance
{
    vk::UniqueInstance instance;
    vk::UniqueDebugUtilsMessengerEXT debug_messenger;
    std::vector<vk::ExtensionProperties> available_extensions;

    operator vk::Instance() { return instance.get(); }
};

struct VulkanInstanceCreateInfo
{
    std::string                 application_name    = nullptr;
    uint32_t                    application_version = 0;
    VulkanDebug                 debug               = VulkanDebug::Off; // If debug is enabled, instance will be created with validation layers and debug-utils extension/messenger (see also: vulkan_debug.h).
    std::span<const char*>      required_extensions;
};

VulkanInstance create_vulkan_instance(const VulkanInstanceCreateInfo& info);