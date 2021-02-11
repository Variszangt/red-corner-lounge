#pragma once

#include <vulkan/vulkan.hpp>

#include "error.h"

// TODO: DEVICEEES!
namespace vki
{
/*------------------------------------------------------------------*/
// DeviceInitInfo:

struct DeviceInitInfo
{
    vk::Instance instance;
    vk::SurfaceKHR surface;
    std::vector<std::string> required_extensions;
};

/*------------------------------------------------------------------*/
// Device:

class Device
{
public:
    void init(const DeviceInitInfo& info);

    auto operator->() { return logical_device.get(); }
    operator auto() { return logical_device.get(); }

private:
    void pick_physical_device();

private:
    vk::PhysicalDevice physical_device;
    vk::UniqueDevice logical_device;

    vk::PhysicalDeviceProperties properties;
    vk::PhysicalDeviceMemoryProperties memory_properties;
    std::vector<vk::QueueFamilyProperties> queue_family_properties;
    struct
    {
        uint32_t graphics;
        uint32_t transfer;
    } queue_family_indices;

    vk::PhysicalDeviceFeatures features;
    vk::PhysicalDeviceFeatures enabled_features;
    std::vector<std::string> supported_extension_names;

    vk::UniqueCommandPool graphics_command_pool;
    vk::UniqueCommandPool transfer_command_pool;

    uint32_t get_queue_family_index(const vk::QueueFlags required_flags) const
    {
        const uint32_t queue_count = static_cast<uint32_t>(queue_family_properties.size());

        // Find dedicated compute queue:
        if (required_flags & vk::QueueFlagBits::eCompute)
        {
            for (uint32_t i = 0; i < queue_count; ++i)
            {
                const auto flags = queue_family_properties[i].queueFlags;
                if ((flags & required_flags) && (!(flags & vk::QueueFlagBits::eGraphics)))
                    return i;
            }
        }

        // Find dedicated transfer queue:
        if (required_flags & vk::QueueFlagBits::eTransfer)
        {
            for (uint32_t i = 0; i < queue_count; ++i)
            {
                const auto flags = queue_family_properties[i].queueFlags;
                if ((flags & required_flags) &&
                    !(flags & vk::QueueFlagBits::eGraphics) &&
                    !(flags & vk::QueueFlagBits::eCompute))
                    return i;
            }
        }

        // Otherwise, return any queue with all required flags:
        for (uint32_t i = 0; i < queue_count; ++i)
        {
            const auto flags = queue_family_properties[i].queueFlags;
            if (flags & required_flags)
                return i;
        }

        THROW_ERROR("required queue family could not be found: {}", static_cast<uint32_t>(required_flags));
    }
};
}