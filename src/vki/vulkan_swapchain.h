#pragma once

#include "vulkan/vulkan.hpp"
#include "vki/vulkan_device.h"

namespace vki
{
struct SwapchainWrapper
{
    vk::UniqueSwapchainKHR  swapchain;
    vk::Format              format;
    vk::ColorSpaceKHR       colorspace;
    std::vector<vk::Image>  images;

    vk::SwapchainKHR get() { return swapchain.get(); }
};

SwapchainWrapper create_swapchain(
    const DeviceWrapper&    device_wrapper,
    const vk::SurfaceKHR    surface,
    vk::Extent2D            extent,
    vk::SwapchainKHR        old_swapchain = vk::SwapchainKHR {});

}