#include "vulkan_interface.h"

#include "vulkan_debug.h"

/*------------------------------------------------------------------*/
// Default dispatcher:

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

void init_default_dispatcher()
{
    // 1. Initialize default dispatcher with non-dependent function pointers:
    static vk::DynamicLoader dl;
    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

    // 2. Default dispatcher must be initialized with an instance as soon as one is created!
    // 3. Default dispatcher must be initialized with a device as soon as one is created!
}

/*------------------------------------------------------------------*/

void Vulkan::init(const VulkanInitInfo& init_info)
{
    init_default_dispatcher();

    /*------------------------------------------------------------------*/
    // Create instance:

    const auto application_version = VK_MAKE_VERSION(
        std::get<0>(init_info.application_version),
        std::get<1>(init_info.application_version),
        std::get<2>(init_info.application_version)
    );

    std::vector<const char*> required_instance_extensions;
    for (auto cstr : vkfw::getRequiredInstanceExtensions())
        required_instance_extensions.emplace_back(cstr);

    const vki::InstanceCreateInfo instance_create_info {
        .application_name       = init_info.application_name,
        .application_version    = application_version,
        .required_extensions    = required_instance_extensions,
        .debug                  = init_info.config.vulkan_debug,
    };
    instance = vki::create_instance(instance_create_info);
    VULKAN_HPP_DEFAULT_DISPATCHER.init(instance.get());

    /*------------------------------------------------------------------*/
    // Create debug messenger:

    if (init_info.config.vulkan_debug >= VulkanDebug::On)
    {
        const auto debug_messenger_createinfo = vki::generate_debug_messenger_createinfo(init_info.config.vulkan_debug);
        debug_messenger = instance->createDebugUtilsMessengerEXTUnique(debug_messenger_createinfo);
    }

    /*------------------------------------------------------------------*/
    // Create surface:

    surface = vkfw::createWindowSurfaceUnique(instance.get(), init_info.window);

    /*------------------------------------------------------------------*/
    // Create (physical/logical) device:

    const vk::PhysicalDeviceFeatures required_device_features {
        .sampleRateShading = VK_TRUE,
        .samplerAnisotropy = VK_TRUE,
    };
    
    const std::vector<const char*>required_device_extensions {

    };
    
    const vki::DeviceCreateInfo device_create_info {
        .instance            = instance.get(),
        .surface             = surface.get(),
        .required_extensions = required_device_extensions,
        .required_features   = required_device_features,
        .debug               = init_info.config.vulkan_debug,
    };
    std::tie(physical_device, device, device_info) = vki::create_device(device_create_info);

    /*------------------------------------------------------------------*/
    // Create queue handles! / Surface capabilities details etc
}