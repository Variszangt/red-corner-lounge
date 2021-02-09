#pragma once

#include <string>

#include <vkfw/vkfw.hpp>
#include <vulkan/vulkan.hpp>

#include "config.h"

/*------------------------------------------------------------------*/

struct VulkanInitInfo
{
    const Config& config;
    const std::string application_name;
    const std::tuple<int, int, int> application_version;
    const vkfw::Window& window;
};

/*------------------------------------------------------------------*/

class VulkanInterface
{
public:
    VulkanInterface()
    {

    }

    void init(const VulkanInitInfo& info)
    {
        /*------------------------------------------------------------------*/
        // Instance:

        const auto application_name = info.application_name.c_str();
        const auto application_version = VK_MAKE_VERSION(
            std::get<0>(info.application_version),
            std::get<1>(info.application_version),
            std::get<2>(info.application_version)
        );
        create_instance(application_name, application_version);

        /*------------------------------------------------------------------*/
        // SurfaceKHR:

        //surface = vkfw::createWindowSurfaceUnique(instance.get(), info.window);
    }

private:
    void create_instance(const char* application_name, const uint32_t application_version)
    {
        // /*------------------------------------------------------------------*/
        // // ApplicationInfo:

        vk::ApplicationInfo application_info;
        application_info.pApplicationName   = application_name;
        application_info.applicationVersion = application_version;
        // application_info.pEngineName        = "myEngine";
        // application_info.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
        // application_info.apiVersion         = VK_API_VERSION_1_2;

        // /*------------------------------------------------------------------*/
        // // Extensions and validation layers:

        // const auto extensions = get_required_vulkan_extensions();
        // const auto extension_data = extensions.empty() ?  nullptr : extensions.data();
        // const uint32_t extension_count = static_cast<uint32_t>(extensions.size());

        // const auto validation_layers = get_required_vulkan_validation_layers();
        // const auto validation_layer_data = validation_layers.empty() ? nullptr : validation_layers.data();
        // const uint32_t validation_layer_count = static_cast<uint32_t>(validation_layers.size());

        // /*------------------------------------------------------------------*/
        // // Debug Messenger (for VkCreateInstance and VkDestroyInstance only):

        // VkDebugUtilsMessengerCreateInfoEXT debug_messenger_createinfo;
        // if (config.vulkan_debug >= VulkanDebug::On)
        //     debug_messenger_createinfo = generate_vulkan_debug_messenger_createinfo(config.vulkan_debug);

        // /*------------------------------------------------------------------*/
        // // Extended CreateInfo (pNext chain):

        // void* extended_createinfo = nullptr;
        // if (config.vulkan_debug >= VulkanDebug::On)
        //     extended_createinfo = &debug_messenger_createinfo;

        // /*------------------------------------------------------------------*/
        // // Create:

        // vk::InstanceCreateInfo instance_createinfo;
        // instance_createinfo.pApplicationInfo        = &application_info;
        // instance_createinfo.pNext                   = extended_createinfo;
        // instance_createinfo.pApplicationInfo        = &application_info;
        // instance_createinfo.enabledLayerCount       = validation_layer_count;
        // instance_createinfo.ppEnabledLayerNames     = validation_layer_data;
        // instance_createinfo.enabledExtensionCount   = extension_count;
        // instance_createinfo.ppEnabledExtensionNames = extension_data;

        // instance = vk::createInstanceUnique(instance_createinfo);
    }

private:
    vk::UniqueInstance instance;
    vk::UniqueSurfaceKHR surface;
};