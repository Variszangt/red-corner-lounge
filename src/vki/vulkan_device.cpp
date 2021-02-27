#include "vulkan_device.h"

#include <set>

#include "vulkan_debug.h"
#include "error.h"

namespace vki
{
uint32_t DeviceWrapper::get_memory_type_index(const uint32_t index_filter, const vk::MemoryPropertyFlags required_properties) const
{
    for (uint32_t i = 0; i != memory_properties.memoryTypeCount; ++i)
    {
        if (index_filter & (1 << i) &&
            (memory_properties.memoryTypes[i].propertyFlags & required_properties) == required_properties)
            return i;
    }

    THROW_ERROR("suitable memory type could not be found");
}

uint32_t get_queue_family_index(const vk::PhysicalDevice physical_device, vk::QueueFlags required_flags)
{
    const auto queue_family_properties = physical_device.getQueueFamilyProperties();
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

#define CHECK_FEATURE_SUPPORT(feature)                                                          \
    if (createinfo.required_features.feature && !available_features.feature)                    \
    {                                                                                           \
        if (createinfo.debug >= VulkanDebug::On)                                                \
            LOG_WARNING("gpu does not support {}", #feature);                                   \
        return false;                                                                           \
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
    // Check availability of required features:

    auto supports_all_required_features = [&](const vk::PhysicalDevice device)
    {
        const auto available_features = device.getFeatures();

        CHECK_FEATURE_SUPPORT(robustBufferAccess);
        CHECK_FEATURE_SUPPORT(fullDrawIndexUint32);
        CHECK_FEATURE_SUPPORT(imageCubeArray);
        CHECK_FEATURE_SUPPORT(independentBlend);
        CHECK_FEATURE_SUPPORT(geometryShader);
        CHECK_FEATURE_SUPPORT(tessellationShader);
        CHECK_FEATURE_SUPPORT(sampleRateShading);
        CHECK_FEATURE_SUPPORT(dualSrcBlend);
        CHECK_FEATURE_SUPPORT(logicOp);
        CHECK_FEATURE_SUPPORT(multiDrawIndirect);
        CHECK_FEATURE_SUPPORT(drawIndirectFirstInstance);
        CHECK_FEATURE_SUPPORT(depthClamp);
        CHECK_FEATURE_SUPPORT(depthBiasClamp);
        CHECK_FEATURE_SUPPORT(fillModeNonSolid);
        CHECK_FEATURE_SUPPORT(depthBounds);
        CHECK_FEATURE_SUPPORT(wideLines);
        CHECK_FEATURE_SUPPORT(largePoints);
        CHECK_FEATURE_SUPPORT(alphaToOne);
        CHECK_FEATURE_SUPPORT(multiViewport);
        CHECK_FEATURE_SUPPORT(samplerAnisotropy);
        CHECK_FEATURE_SUPPORT(textureCompressionETC2);
        CHECK_FEATURE_SUPPORT(textureCompressionASTC_LDR);
        CHECK_FEATURE_SUPPORT(textureCompressionBC);
        CHECK_FEATURE_SUPPORT(occlusionQueryPrecise);
        CHECK_FEATURE_SUPPORT(pipelineStatisticsQuery);
        CHECK_FEATURE_SUPPORT(vertexPipelineStoresAndAtomics);
        CHECK_FEATURE_SUPPORT(fragmentStoresAndAtomics);
        CHECK_FEATURE_SUPPORT(shaderTessellationAndGeometryPointSize);
        CHECK_FEATURE_SUPPORT(shaderImageGatherExtended);
        CHECK_FEATURE_SUPPORT(shaderStorageImageExtendedFormats);
        CHECK_FEATURE_SUPPORT(shaderStorageImageMultisample);
        CHECK_FEATURE_SUPPORT(shaderStorageImageReadWithoutFormat);
        CHECK_FEATURE_SUPPORT(shaderStorageImageWriteWithoutFormat);
        CHECK_FEATURE_SUPPORT(shaderUniformBufferArrayDynamicIndexing);
        CHECK_FEATURE_SUPPORT(shaderSampledImageArrayDynamicIndexing);
        CHECK_FEATURE_SUPPORT(shaderStorageBufferArrayDynamicIndexing);
        CHECK_FEATURE_SUPPORT(shaderStorageImageArrayDynamicIndexing);
        CHECK_FEATURE_SUPPORT(shaderClipDistance);
        CHECK_FEATURE_SUPPORT(shaderCullDistance);
        CHECK_FEATURE_SUPPORT(shaderFloat64);
        CHECK_FEATURE_SUPPORT(shaderInt64);
        CHECK_FEATURE_SUPPORT(shaderInt16);
        CHECK_FEATURE_SUPPORT(shaderResourceResidency);
        CHECK_FEATURE_SUPPORT(shaderResourceMinLod);
        CHECK_FEATURE_SUPPORT(sparseBinding);
        CHECK_FEATURE_SUPPORT(sparseResidencyBuffer);
        CHECK_FEATURE_SUPPORT(sparseResidencyImage2D);
        CHECK_FEATURE_SUPPORT(sparseResidencyImage3D);
        CHECK_FEATURE_SUPPORT(sparseResidency2Samples);
        CHECK_FEATURE_SUPPORT(sparseResidency4Samples);
        CHECK_FEATURE_SUPPORT(sparseResidency8Samples);
        CHECK_FEATURE_SUPPORT(sparseResidency16Samples);
        CHECK_FEATURE_SUPPORT(sparseResidencyAliased);
        CHECK_FEATURE_SUPPORT(variableMultisampleRate);
        CHECK_FEATURE_SUPPORT(inheritedQueries);

        return true;
    };

    /*------------------------------------------------------------------*/
    // Filter and rate available devices:

    std::multimap<int, vk::PhysicalDevice> device_ratings;
    for (const auto& device : physical_devices)
    {
        int rating = 0; // TODO: Currently we pick a random device that supports ALL required features/extensions. For better support of older hardware, we should instead consider a rating system that would allow for subpar devices to be picked.
        bool supports_minimal_requirements = true;

        const auto device_properties = device.getProperties();

        if (createinfo.debug >= VulkanDebug::On)
            LOG_INFO("rating gpu: {}", device_properties.deviceName);

        if (!supports_all_required_extensions(device))
        {
            supports_minimal_requirements = false;
            if (createinfo.debug >= VulkanDebug::On)
                LOG_WARNING("gpu does not support all required extensions");
        }

        if (!supports_all_required_features(device))
        {
            supports_minimal_requirements = false;
            if (createinfo.debug >= VulkanDebug::On)
                LOG_WARNING("gpu does not support all required features");
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

DeviceWrapper create_device(const DeviceCreateInfo& createinfo)
{
    /*------------------------------------------------------------------*/
    // Pick physical device:

    auto physical_device = pick_physical_device(createinfo);

    auto enabled_features  = createinfo.required_features;
    auto properties        = physical_device.getProperties();
    auto memory_properties = physical_device.getMemoryProperties();

    /*------------------------------------------------------------------*/
    // Get queue indices:

    DeviceWrapper::QueueFamilyIndices queue_family_indices {
        .graphics = get_queue_family_index(physical_device, vk::QueueFlagBits::eGraphics),
        .transfer = get_queue_family_index(physical_device, vk::QueueFlagBits::eTransfer),
        .compute  = get_queue_family_index(physical_device, vk::QueueFlagBits::eCompute),
    };

    if (!physical_device.getSurfaceSupportKHR(queue_family_indices.graphics, createinfo.surface))
        THROW_ERROR("graphics queue does not support presentation to surface");

    /*------------------------------------------------------------------*/
    // Prepare queue createinfos:

    const std::set<uint32_t> unique_indices = { // Filter only unique indices, as some may overlap.
        queue_family_indices.graphics,
        queue_family_indices.transfer,
        queue_family_indices.compute,
    };

    std::vector<vk::DeviceQueueCreateInfo> queue_createinfos;
    const float queue_family_priority = 1.f;
    for (const auto& index : unique_indices)
    {
        queue_createinfos.emplace_back(vk::DeviceQueueCreateInfo {
            .queueFamilyIndex   = index,
            .queueCount         = 1,
            .pQueuePriorities   = &queue_family_priority,
        });
    }

    /*------------------------------------------------------------------*/
    // Create device:

    const vk::DeviceCreateInfo device_createinfo {
        .queueCreateInfoCount       = static_cast<uint32_t>(queue_createinfos.size()),
        .pQueueCreateInfos          = queue_createinfos.data(),
        .enabledExtensionCount      = static_cast<uint32_t>(createinfo.required_extensions.size()),
        .ppEnabledExtensionNames    = createinfo.required_extensions.data(),
        .pEnabledFeatures           = &createinfo.required_features,
    };
    auto device = physical_device.createDeviceUnique(device_createinfo);
    VULKAN_HPP_DEFAULT_DISPATCHER.init(device.get());
    set_object_name(device.get(), device.get(), "Main Device");

    /*------------------------------------------------------------------*/
    // Get queue handles:

    DeviceWrapper::Queues queues {
        .graphics = device->getQueue(queue_family_indices.graphics, 0),
        .transfer = device->getQueue(queue_family_indices.transfer, 0),
        .compute  = device->getQueue(queue_family_indices.compute, 0),
    };

    /*------------------------------------------------------------------*/
    // Create command pools:

    auto create_command_pool = [&](const uint32_t index)
    {
        return device->createCommandPoolUnique(vk::CommandPoolCreateInfo { .queueFamilyIndex = index });
    };

    DeviceWrapper::CommandPools command_pools {
        .graphics = create_command_pool(queue_family_indices.graphics),
        .transfer = create_command_pool(queue_family_indices.graphics),
        .compute  = create_command_pool(queue_family_indices.graphics),
    };

    /*------------------------------------------------------------------*/
    // Return:

    return DeviceWrapper {
        .physical_device        = std::move(physical_device),
        .device                 = std::move(device),
        .enabled_features       = std::move(enabled_features),
        .properties             = std::move(properties),
        .memory_properties      = std::move(memory_properties),
        .queue_family_indices   = std::move(queue_family_indices),
        .queues                 = std::move(queues),
        .command_pools          = std::move(command_pools),
    };
};
}