#include "vulkan_debug.h"

#include "log.h"

namespace vki
{
// Callback function for Vulkan debug messenger. Logs the Vulkan message as a trace/warning/error according to its severity.
VKAPI_ATTR VkBool32 VKAPI_CALL log_vulkan_validation_message(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT /* message_type */,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* /* user_data */)
{
    switch (message_severity)
    {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        LOG_TRACE(callback_data->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        LOG_WARNING(callback_data->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        LOG_ERROR(callback_data->pMessage);
        break;
    default:
        LOG_ERROR("uknown switch case: {}", message_severity);
        break;
    }

    return VK_FALSE;
};

vk::DebugUtilsMessengerCreateInfoEXT generate_debug_messenger_createinfo(const VulkanDebug level)
{
    using SeverityFlags  = vk::DebugUtilsMessageSeverityFlagBitsEXT;
    using TypeFlags      = vk::DebugUtilsMessageTypeFlagBitsEXT;

    if (level == VulkanDebug::Off)
        return {};

    vk::DebugUtilsMessageSeverityFlagsEXT severity = SeverityFlags::eWarning | SeverityFlags::eError;
    if (level == VulkanDebug::Verbose)
        severity |= SeverityFlags::eVerbose | SeverityFlags::eInfo;

    const vk::DebugUtilsMessageTypeFlagsEXT type =
        TypeFlags::eGeneral | TypeFlags::eValidation | TypeFlags::ePerformance;

    return vk::DebugUtilsMessengerCreateInfoEXT {
        .messageSeverity  = severity,
        .messageType      = type,
        .pfnUserCallback  = log_vulkan_validation_message,
    };
}
}