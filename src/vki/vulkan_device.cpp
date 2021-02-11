#include "vulkan_device.h"

namespace vki
{
void Device::init(const DeviceInitInfo& info)
{
    /*------------------------------------------------------------------*/
    // Pick physical device:

    auto x = info.required_extensions.size();

}

void Device::pick_physical_device()
{
    // assert(vulkan_instance);
    // assert(vulkan_surface);

    // /*------------------------------------------------------------------*/
    // // Get all available physical devices:

    // std::vector<VkPhysicalDevice> physical_devices;

    // uint32_t physical_device_count = 0;
    // vkEnumeratePhysicalDevices(vulkan_instance, &physical_device_count, nullptr);
    // physical_devices.resize(physical_device_count);
    // vkEnumeratePhysicalDevices(vulkan_instance, &physical_device_count, physical_devices.data());

    // if (physical_device_count == 0)
    //     THROW_ERROR("gpu with vulkan support could not be found");

    // /*------------------------------------------------------------------*/
    // // Check availability of required extensions:

    // auto supports_all_required_extensions = [&](const VkPhysicalDevice device)
    // {
    //     std::vector<VkExtensionProperties> available_extensions;
    //     uint32_t available_extension_count = 0;
    //     vkEnumerateDeviceExtensionProperties(device, nullptr, &available_extension_count, nullptr);
    //     available_extensions.resize(available_extension_count);
    //     vkEnumerateDeviceExtensionProperties(device, nullptr, &available_extension_count, available_extensions.data());

    //     for (const char* required_extension : REQUIRED_DEVICE_EXTENSIONS)
    //     {
    //         bool extension_found = false;
    //         for (const auto& available_extension : available_extensions)
    //         {
    //             if (strcmp(required_extension, available_extension.extensionName) == 0)
    //             {
    //                 extension_found = true;
    //                 break;
    //             }
    //         }
    //         if (!extension_found)
    //         {
    //             if (config.vulkan_debug >= VulkanDebug::On)
    //                 LOG_WARNING("gpu does not support the following extension: {}", required_extension);
    //             return false;
    //         }
    //     }
    //     return true;
    // };

    // /*------------------------------------------------------------------*/
    // // Filter and rate available devices:

    // std::multimap<int, VkPhysicalDevice> device_ratings;
    // for (const auto& physical_device : physical_devices)
    // {
    //     int rating = 0; // TODO: gpu rating. For now, any gpu with minimum requirements is picked at random.
    //     bool supports_minimal_requirements = true;

    //     VkPhysicalDeviceProperties device_properties;
    //     vkGetPhysicalDeviceProperties(physical_device, &device_properties);
    //     if (config.vulkan_debug >= VulkanDebug::On)
    //         LOG_INFO("scoring gpu: {}", device_properties.deviceName);

    //     VkPhysicalDeviceFeatures device_features;
    //     vkGetPhysicalDeviceFeatures(physical_device, &device_features);
    //     if (!device_features.samplerAnisotropy)
    //     {
    //         supports_minimal_requirements = false;
    //         if (config.vulkan_debug >= VulkanDebug::On)
    //             LOG_WARNING("gpu does not support anisotropic sampling");
    //     }

    //     VulkanQueueFamilyIndices queue_family_indices;
    //     if (!queue_family_indices.poll_from_device(physical_device, vulkan_surface))
    //     {
    //         supports_minimal_requirements = false;
    //         if (config.vulkan_debug >= VulkanDebug::On)
    //             LOG_WARNING("gpu does not support all required queue families");
    //     }

    //     VulkanSwapChainDetails swap_chain_details;
    //     if (!swap_chain_details.poll_from_device(physical_device, vulkan_surface))
    //     {
    //         supports_minimal_requirements = false;
    //         if (config.vulkan_debug >= VulkanDebug::On)
    //             LOG_WARNING("gpu does not support minimal swap chain requirements");
    //     }

    //     if (!supports_all_required_extensions(physical_device))
    //     {
    //         supports_minimal_requirements = false;
    //         if (config.vulkan_debug >= VulkanDebug::On)
    //             LOG_WARNING("gpu does not support all required extensions");
    //     }

    //     if (supports_minimal_requirements)
    //         device_ratings.insert(std::make_pair(rating, physical_device));
    // }

    // if (device_ratings.empty())
    //     THROW_ERROR("gpu with minimal requirements could not be found");

    // /*------------------------------------------------------------------*/
    // // Pick the optimal:

    // const VkPhysicalDevice optimal_device = device_ratings.rbegin()->second; // Last element, highest rating.

    // VkPhysicalDeviceProperties device_properties;
    // vkGetPhysicalDeviceProperties(optimal_device, &device_properties);
    // LOG_INFO("picked gpu: {}", device_properties.deviceName);

    // vulkan_physical_device = optimal_device;
    // msaa_samples = get_max_sample_count();
}
}