#include "vulkan_interface.h"

#include "vulkan_debug.h"

#include "vulkan_debug.h"
#include "vulkan_instance.h"
#include "vulkan_renderpass.h"

using namespace vki;

/*------------------------------------------------------------------*/
// Default dispatcher:

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

void init_default_dispatcher()
{
    // 1. Initialize default dispatcher with non-dependent function pointers:
    static vk::DynamicLoader dl;
    auto vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

    // 2. VULKAN_HPP_DEFAULT_DISPATCHER.init(instance) called in vulkan_instance.cpp
    // 3. VULKAN_HPP_DEFAULT_DISPATCHER.init(device) called in vulkan_device.cpp
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

    const InstanceCreateInfo instance_createinfo {
        .application_name       = init_info.application_name,
        .application_version    = application_version,
        .required_extensions    = required_instance_extensions,
        .debug                  = init_info.config.vulkan_debug,
    };
    instance = create_instance(instance_createinfo);

    /*------------------------------------------------------------------*/
    // Create debug messenger:

    if (init_info.config.vulkan_debug >= VulkanDebug::On)
    {
        const auto debug_messenger_createinfo = generate_debug_messenger_createinfo(init_info.config.vulkan_debug);
        debug_messenger = instance.get().createDebugUtilsMessengerEXTUnique(debug_messenger_createinfo);
    }

    /*------------------------------------------------------------------*/
    // Create surface:

    surface = vkfw::createWindowSurfaceUnique(instance.get(), init_info.window);
    
    /*------------------------------------------------------------------*/
    // Create device:

    const std::vector<const char*> required_device_extensions {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    const vk::PhysicalDeviceFeatures required_device_features {
        .sampleRateShading = VK_TRUE,
        .samplerAnisotropy = VK_TRUE,
    };
    const DeviceCreateInfo device_createinfo {
        .instance            = instance.get(),
        .surface             = surface.get(),
        .required_extensions = required_device_extensions,
        .required_features   = required_device_features,
        .debug_utils         = init_info.config.vulkan_debug >= VulkanDebug::On,
    };
    device_wrapper = create_device(device_createinfo);
    set_object_name(device_wrapper, device_wrapper.get(), "MainDevice");

    /*------------------------------------------------------------------*/
    // Create swapchain:

    auto [width, height] = init_info.window.getSize();
    on_resize(width, height);

    /*------------------------------------------------------------------*/
    // Pipelines:

    world_pipeline = create_world_pipeline(
        device_wrapper,
        swapchain_wrapper.format,
        vk::Format::eD32Sfloat,
        swapchain_wrapper.extent);
}

void Vulkan::on_resize(const size_t width, const size_t height)
{
    assert(device_wrapper.get());
    assert(surface.get());

    swapchain_wrapper = create_swapchain(
        device_wrapper,
        surface.get(),
        vk::Extent2D { static_cast<uint32_t>(width), static_cast<uint32_t>(height) },
        swapchain_wrapper.get()
    );
}