#pragma once

#include <vulkan/vulkan.hpp>

#include "config.h"

namespace vki
{
struct DeviceWrapper
{
    vk::PhysicalDevice physical_device;
    vk::UniqueDevice device;
    
    vk::PhysicalDeviceFeatures enabled_features;
    vk::PhysicalDeviceProperties properties;
    vk::PhysicalDeviceMemoryProperties memory_properties;
    
    struct QueueFamilyIndices
    {
        uint32_t graphics   = 0u;
        uint32_t transfer   = 0u;
        uint32_t compute    = 0u;
    } queue_family_indices;
    
    struct Queues
    {
        vk::Queue graphics;
        vk::Queue transfer;
        vk::Queue compute;
    } queues;

    struct CommandPools
    {
        vk::UniqueCommandPool graphics;
        vk::UniqueCommandPool transfer;
        vk::UniqueCommandPool compute;
    } command_pools;
    
    // Returns the index of a memoryType which has all required memory properties. Additionally, only memoryTypes with indices allowed by the filter are returned (each i-th bit of the filter specifies a memory type index).
    uint32_t get_memory_type_index(const uint32_t index_filter, const vk::MemoryPropertyFlags required_properties) const;
};

struct DeviceCreateInfo
{
    vk::Instance                instance;
    vk::SurfaceKHR              surface;
    std::vector<const char*>    required_extensions;
    vk::PhysicalDeviceFeatures  required_features; // Verifying each feature's availability is hardcoded into the pick_physical_device() function - when adding a new feature, make sure to modify it accordingly!
    VulkanDebug                 debug; // If debug is enabled, additional messages will be logged during the device selection process.
};

DeviceWrapper create_device(const DeviceCreateInfo& createinfo);
}    