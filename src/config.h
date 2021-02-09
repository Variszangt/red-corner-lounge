#pragma once

#include <string>

#include "json.h"

/*------------------------------------------------------------------*/
// Config enums

enum class VulkanDebug
{
    Off,        // Disable Vulkan debug messages.
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
    void load(const std::string& filename);
    void save(const std::string& filename);

    int window_width                        = 800;
    int window_height                       = 800;
    std::string window_title                = "Vulkan!";

    VulkanDebug vulkan_debug                = VulkanDebug::Off;

private:
    DEFINE_JSON_SERIALIZABLE(Config,
        window_width,
        window_height,
        window_title,
        vulkan_debug);
};