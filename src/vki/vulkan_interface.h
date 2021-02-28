#pragma once

#include <string>

#include <vkfw/vkfw.hpp>
#include <vulkan/vulkan.hpp>

#include "config.h"
#include "vulkan_device.h"
#include "vulkan_swapchain.h"
#include "vulkan_pipeline.h"

/*------------------------------------------------------------------*/
// All Vulkan-related code shall be written in the vki ("vulkan interface") subfolder/namespace. This header, in turn, serves as the interface to any such code - it is the only Vulkan header that application code should ever include.

/*------------------------------------------------------------------*/
// VulkanInitInfo:

struct VulkanInitInfo
{
    const Config&                   config;
    const std::string               application_name;
    const std::tuple<int, int, int> application_version; // <major, minor, patch>
    const vkfw::Window&             window;
};

/*------------------------------------------------------------------*/
// Vulkan:

class Vulkan
{
public:
    void init(const VulkanInitInfo& init_info);
    void create_swapchain(const size_t width, const size_t height);

private:
    vk::UniqueInstance                  instance;
    vk::UniqueDebugUtilsMessengerEXT    debug_messenger;
    vk::UniqueSurfaceKHR                surface;

    vki::DeviceWrapper      device_wrapper;
    vki::SwapchainWrapper   swapchain_wrapper;

    vki::PipelineWrapper world_pipeline;
};