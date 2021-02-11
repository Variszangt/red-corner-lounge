#pragma once

#include <string>

#include <vkfw/vkfw.hpp>
#include <vulkan/vulkan.hpp>

#include "config.h"
#include "vki/vulkan_instance.h"

namespace vki
{
struct VulkanInfo
{
    const Config& config;
    const std::string application_name;
    const std::tuple<int, int, int> application_version;
    const vkfw::Window& window;
};

class Vulkan
{
public:
    void init(const VulkanInfo& info);

private:
    Instance instance;
};
}