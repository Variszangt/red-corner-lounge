#pragma once

#include <fmt/format.h>
#include <vkfw/vkfw.hpp>

#include "log.h"
#include "config.h"
#include "vki/vulkan_interface.h"

/*------------------------------------------------------------------*/
// App:

class App
{
public:
    int run();

private:
    void create_window();

    void main_loop();

    void on_resize(const int width, const int height);

private:
    Config config;
    vkfw::UniqueInstance glfw_instance;
    vkfw::UniqueWindow window;
    Vulkan vulkan;
};