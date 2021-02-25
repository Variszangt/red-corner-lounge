#pragma once

#include <vulkan/vulkan.hpp>

#include "vulkan_device.h"

namespace vki
{
struct SwapchainWrapper
{
    vk::UniqueSwapchainKHR  swapchain;
    vk::Format              format;
    vk::ColorSpaceKHR       colorspace;
    vk::Extent2D            extent;
    
    std::vector<vk::Image>              images;
    std::vector<vk::UniqueImageView>    image_views;
    
    vk::SwapchainKHR get() { return swapchain.get(); }
};

SwapchainWrapper create_swapchain(
    const DeviceWrapper&    device_wrapper,
    const vk::SurfaceKHR    surface,
    vk::Extent2D            extent,
    vk::SwapchainKHR        old_swapchain = vk::SwapchainKHR {});

}