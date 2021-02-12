#pragma once

#include <string>

#include <vkfw/vkfw.hpp>
#include <vulkan/vulkan.hpp>

#include "config.h"
#include "vki/vulkan_instance.h"
#include "vki/vulkan_device.h"

/*------------------------------------------------------------------*/
// All Vulkan-related code shall be written in the vki ("vulkan interface") subfolder/namespace. This header, in turn, serves as the interface to any such code - it is the only Vulkan header that application code should ever include.

/*------------------------------------------------------------------*/
// VulkanInitInfo:

struct VulkanInitInfo
{
    const Config& config;
    const std::string application_name;
    const std::tuple<int, int, int> application_version; // <major, minor, patch>
    const vkfw::Window& window;
};

/*------------------------------------------------------------------*/
// Vulkan:

class Vulkan
{
public:
    void init(const VulkanInitInfo& init_info);

private:
    vk::UniqueInstance instance;
    vk::UniqueDebugUtilsMessengerEXT debug_messenger;
    vk::UniqueSurfaceKHR surface;

    vk::PhysicalDevice physical_device;
    vk::UniqueDevice device;
    vki::DeviceInfo device_info;
};