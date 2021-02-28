#pragma once

#include <vulkan/vulkan.hpp>

#include "config.h"

namespace vki
{
extern bool DEBUG_UTILS_ENABLED; // Disabled by default; enabled by Instance if it uses the DebugUtils extension. 

/*------------------------------------------------------------------*/
// DebugMessenger:

// A Vulkan debug messenger created from the returned structure will log messages according to the VulkanDebug enum.
vk::DebugUtilsMessengerCreateInfoEXT generate_debug_messenger_createinfo(const VulkanDebug level);

/*------------------------------------------------------------------*/
// Object naming:


// For any Vulkan object that will be named/tagged by DebugUtils, it must have its handle-type mapped to a corresponding vk::ObjectType by using the following MAP_VULKAN_OBJECT_TYPE macro.
template<typename H>
struct VulkanObjectTypeMap { static const vk::ObjectType value = vk::ObjectType::eUnknown; };
#define MAP_VULKAN_OBJECT_TYPE(VkHppHandle, object_type_enum) \
template<> struct VulkanObjectTypeMap<VkHppHandle> { static const vk::ObjectType value = object_type_enum; };

MAP_VULKAN_OBJECT_TYPE(vk::Instance, vk::ObjectType::eInstance);
MAP_VULKAN_OBJECT_TYPE(vk::Device, vk::ObjectType::eDevice);
MAP_VULKAN_OBJECT_TYPE(vk::SwapchainKHR, vk::ObjectType::eSwapchainKHR);
MAP_VULKAN_OBJECT_TYPE(vk::RenderPass, vk::ObjectType::eRenderPass);
MAP_VULKAN_OBJECT_TYPE(vk::Pipeline, vk::ObjectType::ePipeline);

template<typename VkHppHandle>
uint64_t get_raw_handle(const VkHppHandle h)
{
    return reinterpret_cast<uint64_t>(static_cast<typename VkHppHandle::CType>(h));
}

template<typename VkHppHandle>
void set_object_name(
    const vk::Device    device,
    const VkHppHandle   object_handle,
    const char*         object_name)
{
    assert(DEBUG_UTILS_ENABLED);
    
    const auto object_type = VulkanObjectTypeMap<VkHppHandle>::value;
    assert(object_type != vk::ObjectType::eUnknown);

    const auto raw_handle = get_raw_handle(object_handle);

    const vk::DebugUtilsObjectNameInfoEXT name_info {
        .objectType     = object_type,
        .objectHandle   = raw_handle,
        .pObjectName    = object_name,
    };
    device.setDebugUtilsObjectNameEXT(name_info);
}
}
