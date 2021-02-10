#pragma once

#include <vulkan/vulkan.hpp>

#include "error.h"

struct VulkanDevice
{
    VulkanDevice()
    {

    }
    
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


    operator vk::Device() { return logical_device.get(); };

    explicit VulkanDevice(vk::PhysicalDevice physical_device)
    {
        assert(physical_device);

        this->physical_device = physical_device;

        properties = physical_device.getProperties();
        memory_properties = physical_device.getMemoryProperties();
        queue_family_properties = physical_device.getQueueFamilyProperties();



    }

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