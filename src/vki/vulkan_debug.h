#pragma once

#include <vulkan/vulkan.hpp>

#include "config.h"
#include "vulkan_device.h"

namespace vki
{
/*------------------------------------------------------------------*/
// DebugMessenger:

// A Vulkan debug messenger created from the returned structure will log messages according to the VulkanDebug enum.
vk::DebugUtilsMessengerCreateInfoEXT generate_debug_messenger_createinfo(const VulkanDebug level);

/*------------------------------------------------------------------*/
// Object naming:

template<typename VkHppHandle>
inline void set_object_name(
    const DeviceWrapper&    device_wrapper,
    const VkHppHandle       object_handle,
    const std::string&      object_name)
{
    if (!device_wrapper.debug_utils)
        return;
    
    auto device = device_wrapper.get();
    assert(device);
    
    const auto object_type = VkHppHandle::objectType;
    assert(object_type != vk::ObjectType::eUnknown);

    const auto raw_handle = reinterpret_cast<uint64_t>(static_cast<typename VkHppHandle::CType>(object_handle));

    const vk::DebugUtilsObjectNameInfoEXT name_info {
        .objectType     = object_type,
        .objectHandle   = raw_handle,
        .pObjectName    = object_name.c_str(),
    };
    device.setDebugUtilsObjectNameEXT(name_info);
}
}
