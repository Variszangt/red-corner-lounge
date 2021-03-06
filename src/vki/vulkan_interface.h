#pragma once

#include <string>

#include <vkfw/vkfw.hpp>
#include <vulkan/vulkan.hpp>

#include "config.h"
#include "vulkan_device.h"
#include "vulkan_swapchain.h"
#include "vulkan_pipeline.h"
#include "camera.h"

/*------------------------------------------------------------------*/
// All Vulkan-related code shall be written in the vki ("vulkan interface") subfolder/namespace. This header, in turn, serves as the interface to any such code - it is the only Vulkan header that application code should ever include.

/*------------------------------------------------------------------*/
// VulkanRendererInitInfo:

struct VulkanRendererInitInfo
{
    const Config&                   config;
    const std::string               application_name;
    const std::tuple<int, int, int> application_version; // <major, minor, patch>
    const vkfw::Window&             window;
};

/*------------------------------------------------------------------*/
// Vulkan:

class VulkanRenderer
{
public:
    void init(const VulkanRendererInitInfo& init_info);
    void on_resize(const size_t width, const size_t height);

    void update(float elapsed_time);
    
private:
    vk::UniqueInstance                  instance;
    vk::UniqueDebugUtilsMessengerEXT    debug_messenger;
    vk::UniqueSurfaceKHR                surface;

    vki::DeviceWrapper      device_wrapper;
    vki::SwapchainWrapper   swapchain_wrapper;

    vki::PipelineWrapper world_pipeline;
    vki::Camera camera;
};