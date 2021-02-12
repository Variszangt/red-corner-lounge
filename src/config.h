#pragma once

#include <string>

#include "json.h"

/*------------------------------------------------------------------*/
// Config enums

enum class VulkanDebug
{
    Off = 0,    // Disable Vulkan debug messages.
    On,         // Enable Vulkan Warning and Error messages.
    Verbose,    // Enable Vulkan Warning and Error messages + Info and Verbose messages.
};

NLOHMANN_JSON_SERIALIZE_ENUM(VulkanDebug, {
    { VulkanDebug::Off,     "off" },
    { VulkanDebug::On,      "on" },
    { VulkanDebug::Verbose, "verbose" },
})

/*------------------------------------------------------------------*/
// Config:

struct Config
{
    int window_width                        = 800;
    int window_height                       = 800;
    VulkanDebug vulkan_debug                = VulkanDebug::Off;

    void load(const std::string& filename);
    void save(const std::string& filename);

private:
    DEFINE_JSON_SERIALIZABLE(Config,
        window_width,
        window_height,
        vulkan_debug);
};