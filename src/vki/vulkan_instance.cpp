#include "vulkan_instance.h"

#include "utility.h"
#include "error.h"
#include "vulkan_debug.h"
#include "vulkan_assist.h"

namespace vki
{
/*------------------------------------------------------------------*/
// Constants:

extern const uint32_t MIN_VULKAN_API_VERSION = VK_API_VERSION_1_2;
const char* DEBUG_UTILS_EXTENSION_NAME = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;

/*------------------------------------------------------------------*/

vk::UniqueInstance create_instance(const InstanceCreateInfo& createinfo)
{
    /*------------------------------------------------------------------*/
    // ApplicationInfo:

    const vk::ApplicationInfo application_info {
        .pApplicationName   = createinfo.application_name.c_str(),
        .applicationVersion = createinfo.application_version,
        .pEngineName        = "myEngine",
        .engineVersion      = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion         = MIN_VULKAN_API_VERSION,
    };

    auto instance_version = vk::enumerateInstanceVersion();
    LOG_INFO("expecting minimum API version: {}", get_version_string(MIN_VULKAN_API_VERSION));
    LOG_INFO("vulkan header version: {}", get_version_string(VK_HEADER_VERSION_COMPLETE));
    LOG_INFO("vulkan instance version: {}", get_version_string(instance_version));
    if (instance_version < MIN_VULKAN_API_VERSION)
        THROW_ERROR("minimum vulkan instance version ({}) not installed; check your vulkan runtimes",
        get_version_string(MIN_VULKAN_API_VERSION));

    /*------------------------------------------------------------------*/
    // Layers:

    const auto available_layers = vk::enumerateInstanceLayerProperties();

    std::vector<const char*> required_layers;
    if (createinfo.debug >= VulkanDebug::On)
        required_layers.emplace_back("VK_LAYER_KHRONOS_validation");
    const uint32_t required_layer_count = static_cast<uint32_t>(required_layers.size());

    // Verify availability of required layers:
    if (required_layer_count)
    {
        for (const auto& required_layer_name : required_layers)
        {
            bool layer_found = false;
            for (const auto& available_layer : available_layers)
            {
                if (std::strcmp(available_layer.layerName, required_layer_name) == 0)
                {
                    layer_found = true;
                    break;
                }
            }
            if (!layer_found)
                THROW_ERROR("required layer not available: {}", required_layer_name);
        }
    }

    /*------------------------------------------------------------------*/
    // Extensions:

    const auto available_extensions = vk::enumerateInstanceExtensionProperties();

    auto required_extensions = createinfo.required_extensions;
    if (createinfo.debug >= VulkanDebug::On &&
        !contains(required_extensions, DEBUG_UTILS_EXTENSION_NAME))
        required_extensions.emplace_back(DEBUG_UTILS_EXTENSION_NAME);
    const uint32_t required_extension_count = static_cast<uint32_t>(required_extensions.size());

    // Verify availability of required extensions:
    if (required_extension_count)
    {
        for (const auto& required_extension_name : required_extensions)
        {
            bool extension_found = false;
            for (const auto& available_extension : available_extensions)
            {
                if (std::strcmp(available_extension.extensionName, required_extension_name) == 0)
                {
                    extension_found = true;
                    break;
                }
            }
            if (!extension_found)
                THROW_ERROR("required extension not available: {}", required_extension_name);
        }
    }

    /*------------------------------------------------------------------*/
    // Instance createinfo:

    const vk::InstanceCreateInfo instance_createinfo {
        .pApplicationInfo        = &application_info,
        .enabledLayerCount       = required_layer_count,
        .ppEnabledLayerNames     = required_layers.data(),
        .enabledExtensionCount   = required_extension_count,
        .ppEnabledExtensionNames = required_extensions.data(),
    };

    /*------------------------------------------------------------------*/
    // Debug messenger createinfo:

    const auto debug_messenger_createinfo = generate_debug_messenger_createinfo(createinfo.debug);
    
    /*------------------------------------------------------------------*/
    // Extended createinfo (pNext chain):

    vk::StructureChain<vk::InstanceCreateInfo, vk::DebugUtilsMessengerCreateInfoEXT> extended_createinfo {
        instance_createinfo,
        debug_messenger_createinfo,
    };

    if (createinfo.debug == VulkanDebug::Off)
        extended_createinfo.unlink<vk::DebugUtilsMessengerCreateInfoEXT>();

    /*------------------------------------------------------------------*/
    // Create instance:

    auto instance = vk::createInstanceUnique(extended_createinfo.get<vk::InstanceCreateInfo>());
    VULKAN_HPP_DEFAULT_DISPATCHER.init(instance.get());

    return std::move(instance);
}
}