#include "vulkan.h"

using namespace vki;

/*------------------------------------------------------------------*/
// Default dispatch loader:

// DispatchLoaderDynamic definition:
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

// Initializes default dispatch loader with instance-independent function pointers.
void init_dispatch_loader()
{
    static vk::DynamicLoader dl;
    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
}

// Initializes default dispatch loader with instance-dependent function pointers.
void init_dispatch_loader_for_instance_functions(vk::Instance instance)
{
    VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);
}

// Initializes default dispatch loader with device-dependent function pointers.
void init_dispatch_loader_for_device_functions(vk::Device device)
{
    VULKAN_HPP_DEFAULT_DISPATCHER.init(device);
}

/*------------------------------------------------------------------*/

void Vulkan::init(const VulkanInfo& info)
{
    init_dispatch_loader();

    /*------------------------------------------------------------------*/
    // Instance:

    const auto application_version = VK_MAKE_VERSION(
        std::get<0>(info.application_version),
        std::get<1>(info.application_version),
        std::get<2>(info.application_version)
    );

    const vki::InstanceInfo instance_info {
        .application_name       = info.application_name,
        .application_version    = application_version,
        .debug                  = info.config.vulkan_debug,
    };
    instance.init(instance_info);
    init_dispatch_loader_for_instance_functions(instance);
}