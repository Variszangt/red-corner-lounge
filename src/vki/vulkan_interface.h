#pragma once

#include <string>

#include <vkfw/vkfw.hpp>
#include <vulkan/vulkan.hpp>

#include "config.h"
#include "vki/vulkan_device.h"

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

        const auto required_instance_extensions = vkfw::getRequiredInstanceExtensions()
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


private:
    vk::UniqueInstance instance;
    VulkanDevice device;
    vk::UniqueSurfaceKHR surface;
};