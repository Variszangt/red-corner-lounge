#include "vulkan_instance.h"

#include "utility.h"
#include "error.h"
#include "vulkan_debug.h"

namespace vki
{
/*------------------------------------------------------------------*/
// Constants:

const char* DEBUG_UTILS_EXTENSION_NAME = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;

/*------------------------------------------------------------------*/

vk::UniqueInstance create_instance(const InstanceInitInfo& info)
{
    /*------------------------------------------------------------------*/
    // ApplicationInfo:

    const vk::ApplicationInfo application_info {
        .pApplicationName   = info.application_name.c_str(),
        .applicationVersion = info.application_version,
        .pEngineName        = "myEngine",
        .engineVersion      = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion         = VK_API_VERSION_1_2,
    };

    /*------------------------------------------------------------------*/
    // Layers:

    const auto available_layers = vk::enumerateInstanceLayerProperties();

    std::vector<const char*> required_layers;
    if (info.debug >= VulkanDebug::On)
        required_layers.emplace_back("VK_LAYER_KHRONOS_validation");
    const uint32_t required_layer_count = static_cast<uint32_t>(required_layers.size());

    // Verify availability of required layers:
    if (required_layer_count)
    {
        for (const auto& required_layer_name : required_layers)
        {
            bool found = false;
            for (const auto& available_layer : available_layers)
            {
                if (std::strcmp(available_layer.layerName, required_layer_name) == 0)
                {
                    found = true;
                    break;
                }
            }
            if (!found)
                THROW_ERROR("required layer not available: {}", required_layer_name);
        }
    }

    /*------------------------------------------------------------------*/
    // Extensions:

    const auto available_extensions = vk::enumerateInstanceExtensionProperties();

    auto required_extensions = info.required_extensions;
    if (info.debug >= VulkanDebug::On &&
        !contains(required_extensions, DEBUG_UTILS_EXTENSION_NAME))
        required_extensions.emplace_back(DEBUG_UTILS_EXTENSION_NAME);

    const auto required_extensions_data = required_extensions.data();
    const uint32_t required_extension_count = static_cast<uint32_t>(required_extensions.size());

    // Verify availability of required extensions:
    if (required_extension_count)
    {
        for (const auto& required_extension_name : info.required_extensions)
        {
            bool found = false;
            for (const auto& available_extension : available_extensions)
            {
                if (std::strcmp(available_extension.extensionName, required_extension_name) == 0)
                {
                    found = true;
                    break;
                }
            }
            if (!found)
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
        .ppEnabledExtensionNames = required_extensions_data,
    };

    /*------------------------------------------------------------------*/
    // Debug messenger createinfo:

    const auto debug_messenger_createinfo = generate_debug_messenger_createinfo(info.debug);

    /*------------------------------------------------------------------*/
    // Extended createinfo (pNext chain):

    vk::StructureChain<vk::InstanceCreateInfo, vk::DebugUtilsMessengerCreateInfoEXT> extended_createinfo {
        instance_createinfo,
        debug_messenger_createinfo,
    };

    if (info.debug == VulkanDebug::Off)
        extended_createinfo.unlink<vk::DebugUtilsMessengerCreateInfoEXT>();

    /*------------------------------------------------------------------*/
    // Create instance:

    return vk::createInstanceUnique(extended_createinfo.get<vk::InstanceCreateInfo>());
}
}