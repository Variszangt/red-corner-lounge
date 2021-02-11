#pragma once

#include <vulkan/vulkan.hpp>

#include "config.h"

namespace vki
{
// A Vulkan debug messenger created from the returned structure will log messages according to the VulkanDebug enum.
vk::DebugUtilsMessengerCreateInfoEXT generate_debug_messenger_createinfo(const VulkanDebug level);
}
