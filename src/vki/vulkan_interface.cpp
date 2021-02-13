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

    std::vector<const char*> required_instance_extensions {

    };
    for (auto cstr : vkfw::getRequiredInstanceExtensions())
        required_instance_extensions.emplace_back(cstr);

    const vki::InstanceCreateInfo instance_createinfo {
        .application_name       = init_info.application_name,
        .application_version    = application_version,
        .required_extensions    = required_instance_extensions,
        .debug                  = init_info.config.vulkan_debug,
    };
    instance = vki::create_instance(instance_createinfo);
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

    const std::vector<const char*>required_device_extensions {

    };

    const vk::PhysicalDeviceFeatures required_device_features {
        .sampleRateShading = VK_TRUE,
        .samplerAnisotropy = VK_TRUE,
    };

    const vki::DeviceCreateInfo device_createinfo {
        .instance            = instance.get(),
        .surface             = surface.get(),
        .required_extensions = required_device_extensions,
        .required_features   = required_device_features,
        .debug               = init_info.config.vulkan_debug,
    };
    std::tie(physical_device, device, device_info) = vki::create_device(device_createinfo);

    // Get queue handles:
    queues.graphics = device->getQueue(device_info.queue_family_indices.graphics, 0);
    queues.transfer = device->getQueue(device_info.queue_family_indices.transfer, 0);
    queues.compute  = device->getQueue(device_info.queue_family_indices.compute, 0);

    /*------------------------------------------------------------------*/
    // Command pools:

    { // Graphics
        const vk::CommandPoolCreateInfo command_pool_createinfo {
            .queueFamilyIndex = device_info.queue_family_indices.graphics
        };
        command_pools.graphics = device->createCommandPoolUnique(command_pool_createinfo);
    }
    
    { // Transfer
        const vk::CommandPoolCreateInfo command_pool_createinfo {
            .queueFamilyIndex = device_info.queue_family_indices.transfer
        };
        command_pools.transfer = device->createCommandPoolUnique(command_pool_createinfo);
    }
    
    { // Compute
        const vk::CommandPoolCreateInfo command_pool_createinfo {
            .queueFamilyIndex = device_info.queue_family_indices.compute
        };
        command_pools.compute = device->createCommandPoolUnique(command_pool_createinfo);
    }
    
    /*------------------------------------------------------------------*/
    // 
    
}