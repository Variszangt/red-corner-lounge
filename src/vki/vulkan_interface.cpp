#include "vulkan_interface.h"

#include "vulkan_debug.h"

using namespace vki;

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

void Vulkan::init(const VulkanInitInfo& info)
{
    init_default_dispatcher();

    /*------------------------------------------------------------------*/
    // Create instance:

    const auto application_version = VK_MAKE_VERSION(
        std::get<0>(info.application_version),
        std::get<1>(info.application_version),
        std::get<2>(info.application_version)
    );

    std::vector<const char*> required_instance_extensions;
    for (auto cstr : vkfw::getRequiredInstanceExtensions())
        required_instance_extensions.emplace_back(cstr);

    const vki::InstanceInitInfo instance_init_info {
        .application_name       = info.application_name,
        .application_version    = application_version,
        .debug                  = info.config.vulkan_debug,
        .required_extensions    = required_instance_extensions,
    };
    instance = create_instance(instance_init_info);
    VULKAN_HPP_DEFAULT_DISPATCHER.init(instance.get());

    /*------------------------------------------------------------------*/
    // Create debug messenger:

    if (info.config.vulkan_debug >= VulkanDebug::On)
    {
        const auto debug_messenger_createinfo = generate_debug_messenger_createinfo(info.config.vulkan_debug);
        debug_messenger = instance->createDebugUtilsMessengerEXTUnique(debug_messenger_createinfo);
    }

    /*------------------------------------------------------------------*/
    // Create surface:

    surface = vkfw::createWindowSurfaceUnique(instance.get(), info.window);

    /*------------------------------------------------------------------*/
    // Create device:

    const vki::DeviceInitInfo device_init_info {
        .instance   = instance.get(),
        .surface    = surface.get(),
    };
    // TODO: Create physical device/ logical device, at same time? Also DeviceProperties struct?

    /*------------------------------------------------------------------*/
    // ...
}