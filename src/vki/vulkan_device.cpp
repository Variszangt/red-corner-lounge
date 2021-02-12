#include "vulkan_device.h"

#include <set>

#include "error.h"

namespace vki
{
uint32_t get_queue_family_index(const vk::PhysicalDevice device, vk::QueueFlags required_flags)
{
    const auto queue_family_properties = device.getQueueFamilyProperties();
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

vk::PhysicalDevice pick_physical_device(const DeviceCreateInfo& createinfo)
{
    assert(createinfo.instance);
    assert(createinfo.surface);

    /*------------------------------------------------------------------*/
    // Get all available physical devices:

    const auto physical_devices = createinfo.instance.enumeratePhysicalDevices();

    if (physical_devices.empty())
        THROW_ERROR("gpu with vulkan support could not be found");

    /*------------------------------------------------------------------*/
    // Check availability of required extensions:

    auto supports_all_required_extensions = [&](const vk::PhysicalDevice device)
    {
        const auto available_extensions = device.enumerateDeviceExtensionProperties();

        for (const char* required_extension : createinfo.required_extensions)
        {
            bool extension_found = false;
            for (const auto& available_extension : available_extensions)
            {
                if (std::strcmp(required_extension, available_extension.extensionName) == 0)
                {
                    extension_found = true;
                    break;
                }
            }
            if (!extension_found)
            {
                if (createinfo.debug >= VulkanDebug::On)
                    LOG_WARNING("gpu does not support the following extension: {}", required_extension);
                return false;
            }
        }
        return true;
    };

    /*------------------------------------------------------------------*/
    // Filter and rate available devices:

    std::multimap<int, vk::PhysicalDevice> device_ratings;
    for (const auto& device : physical_devices)
    {
        int rating = 0; // TODO: Currently we pick a random device that supports ALL required features/extensions. For better support of older hardware, we should instead consider a rating system that would allow for subpar devices to be picked (in case no better alternatives are present).
        bool supports_minimal_requirements = true;

        const auto device_properties = device.getProperties();
        const auto device_features   = device.getFeatures();

        if (createinfo.debug >= VulkanDebug::On)
            LOG_INFO("rating gpu: {}", device_properties.deviceName);

        if (!supports_all_required_extensions(device))
        {
            supports_minimal_requirements = false;
            if (createinfo.debug >= VulkanDebug::On)
                LOG_WARNING("gpu does not support all required extensions");
        }

        if (createinfo.required_features.samplerAnisotropy && !device_features.samplerAnisotropy)
        {
            supports_minimal_requirements = false;
            if (createinfo.debug >= VulkanDebug::On)
                LOG_WARNING("gpu does not support anisotropic sampling");
        }
        
        if (createinfo.required_features.sampleRateShading && !device_features.sampleRateShading)
        {
            supports_minimal_requirements = false;
            if (createinfo.debug >= VulkanDebug::On)
                LOG_WARNING("gpu does not support sample rate shading");
        }

        if (supports_minimal_requirements)
            device_ratings.insert(std::make_pair(rating, device));
    }

    if (device_ratings.empty())
        THROW_ERROR("gpu with minimal requirements could not be found");

    /*------------------------------------------------------------------*/
    // Pick the optimal:

    const auto optimal_device = device_ratings.rbegin()->second; // Last element, highest rating.
    LOG_INFO("picked gpu: {}", optimal_device.getProperties().deviceName);
    return optimal_device;
}

std::tuple<vk::PhysicalDevice, vk::UniqueDevice, DeviceInfo> create_device(const DeviceCreateInfo& createinfo)
{
    DeviceInfo device_info;

    /*------------------------------------------------------------------*/
    // Pick physical device:

    vk::PhysicalDevice physical_device = pick_physical_device(createinfo);

    device_info.enabled_features  = createinfo.required_features;
    device_info.properties        = physical_device.getProperties();
    device_info.memory_properties = physical_device.getMemoryProperties();
    
    /*------------------------------------------------------------------*/
    // Prepare queues:

    device_info.queue_family_indices.graphics = get_queue_family_index(physical_device, vk::QueueFlagBits::eGraphics);
    device_info.queue_family_indices.transfer = get_queue_family_index(physical_device, vk::QueueFlagBits::eTransfer);
    device_info.queue_family_indices.compute  = get_queue_family_index(physical_device, vk::QueueFlagBits::eCompute);

    const std::set<uint32_t> unique_queue_family_indices = { // Filter unique indices, as some may overlap.
        device_info.queue_family_indices.graphics,
        device_info.queue_family_indices.transfer,
        device_info.queue_family_indices.compute,
    };

    std::vector<vk::DeviceQueueCreateInfo> queue_createinfos;
    const float queue_family_priority = 1.f;
    for (const auto& index : unique_queue_family_indices)
    {
        queue_createinfos.emplace_back(vk::DeviceQueueCreateInfo {
            .queueFamilyIndex   = index,
            .queueCount         = 1,
            .pQueuePriorities   = &queue_family_priority,
        });
    }

    /*------------------------------------------------------------------*/
    // Create logical device:

    const vk::DeviceCreateInfo logical_device_createinfo {
        .queueCreateInfoCount       = static_cast<uint32_t>(queue_createinfos.size()),
        .pQueueCreateInfos          = queue_createinfos.data(),
        .enabledExtensionCount      = static_cast<uint32_t>(createinfo.required_extensions.size()),
        .ppEnabledExtensionNames    = createinfo.required_extensions.data(),
        .pEnabledFeatures           = &createinfo.required_features,
    };
    vk::UniqueDevice logical_device = physical_device.createDeviceUnique(logical_device_createinfo);

    /*------------------------------------------------------------------*/
    // Return:
    
    return { std::move(physical_device), std::move(logical_device), std::move(device_info) };
};
}