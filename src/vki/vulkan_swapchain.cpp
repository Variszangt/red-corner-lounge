#include "vulkan_swapchain.h"

#include "utility.h"
#include "vulkan_assist.h"

namespace vki
{
SwapchainWrapper create_swapchain(
    const DeviceWrapper&    device_wrapper,
    const vk::SurfaceKHR    surface,
    vk::Extent2D            extent,
    vk::SwapchainKHR        old_swapchain)
{
    auto device         = device_wrapper.device.get();
    auto command_pool   = device_wrapper.command_pools.graphics.get();
    auto graphics_queue = device_wrapper.queues.graphics;

    assert(device);
    assert(command_pool);
    assert(graphics_queue);

    /*------------------------------------------------------------------*/
    // Surface:

    const auto physical_device              = device_wrapper.physical_device;
    const auto surface_capabilities         = physical_device.getSurfaceCapabilitiesKHR(surface);
    const auto surface_formats              = physical_device.getSurfaceFormatsKHR(surface);
    const auto surface_presentation_modes   = physical_device.getSurfacePresentModesKHR(surface);

    // Extent:
    const auto min_extent = surface_capabilities.minImageExtent;
    const auto max_extent = surface_capabilities.maxImageExtent;
    if (!assure_bounds(extent.width, min_extent.width, max_extent.width) |
        !assure_bounds(extent.height, min_extent.height, max_extent.height))
        LOG_WARNING("swapchain extent had to be adjusted");

    // Format:
    vk::SurfaceFormatKHR surface_format = surface_formats.front();
    bool preferred_format_found = false;
    for (const auto& available_format : surface_formats)
    {
        if (available_format.format == vk::Format::eB8G8R8A8Srgb &&
            available_format.colorSpace == vk::ColorSpaceKHR::eVkColorspaceSrgbNonlinear)
        {
            surface_format = available_format;
            preferred_format_found = true;
        }
    }
    if (!preferred_format_found)
        LOG_WARNING("preferred vulkan surface format not found; using first available format instead");

    // Presentation mode:
    vk::PresentModeKHR presentation_mode = vk::PresentModeKHR::eFifo;

    // Image count (+1 to assure we always have a free image to render to):
    uint32_t image_count = surface_capabilities.minImageCount + 1;
    if (surface_capabilities.maxImageCount != 0 && surface_capabilities.maxImageCount < image_count)
        image_count = surface_capabilities.maxImageCount;

    /*------------------------------------------------------------------*/
    // Other

    // Same graphics/presentation queue:
    const auto image_sharing_mode = vk::SharingMode::eExclusive;

    // Drawing straight to image:
    const auto image_usage = vk::ImageUsageFlagBits::eColorAttachment;

    // Default (no) pre-transformations:
    const auto pre_transform = surface_capabilities.currentTransform;

    // Disable window blending:
    const auto alpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;

    // Enable clipping:
    const auto clipped = VK_TRUE;

    /*------------------------------------------------------------------*/
    // Create:

    const vk::SwapchainCreateInfoKHR createinfo {
        .surface                = surface,
        .minImageCount          = image_count,
        .imageFormat            = surface_format.format,
        .imageColorSpace        = surface_format.colorSpace,
        .imageExtent            = extent,
        .imageArrayLayers       = 1,
        .imageUsage             = image_usage,
        .imageSharingMode       = image_sharing_mode,
        .queueFamilyIndexCount  = 0,
        .pQueueFamilyIndices    = nullptr,
        .preTransform           = pre_transform,
        .compositeAlpha         = alpha,
        .presentMode            = presentation_mode,
        .clipped                = clipped,
        .oldSwapchain           = old_swapchain,
    };
    auto swapchain = device.createSwapchainKHRUnique(createinfo);

    /*------------------------------------------------------------------*/
    // Retrieve images:

    auto images = device.getSwapchainImagesKHR(swapchain.get());

    std::vector<vk::UniqueImageView> image_views;
    image_views.reserve(images.size());
    for (const auto image : images)
    {
        vk::ImageViewCreateInfo view_createinfo {
            .image              = image,
            .viewType           = vk::ImageViewType::e2D,
            .format             = surface_format.format,
            .subresourceRange   = create_ISR(vk::ImageAspectFlagBits::eColor),
        };
        image_views.push_back(device.createImageViewUnique(view_createinfo));
    }

    /*------------------------------------------------------------------*/
    // Return:

    return SwapchainWrapper {
        .swapchain   = std::move(swapchain),
        .format      = std::move(surface_format.format),
        .colorspace  = std::move(surface_format.colorSpace),
        .extent      = std::move(extent),
        .images      = std::move(images),
        .image_views = std::move(image_views),
    };
}
}