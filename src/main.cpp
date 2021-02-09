#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

#include "log.h"
#include <vkfw/vkfw.hpp>
#include <vulkan/vulkan.hpp>

#if defined(_MSC_VER)
#include <Windows.h>
#endif

/*------------------------------------------------------------------*/

int main(int argc, char** argv)
{
    /*------------------------------------------------------------------*/
    // doctest:

#ifndef DOCTEST_CONFIG_DISABLE
    doctest::Context context;
    context.applyCommandLine(argc, argv);
    context.setOption("no-breaks", true);
    const int res = context.run();
    if (context.shouldExit())
        return res;
#endif

    /*------------------------------------------------------------------*/
    // Enable wide character output for Windows consoles:

#if defined(_MSC_VER)
    SetConsoleOutputCP(CP_UTF8);
#endif

    /*------------------------------------------------------------------*/
    // main:

    int exit_code = 0;
    try
    {
        vkfw::init();

        vk::Extent2D resolution;
        resolution.width = 700;
        resolution.height = 600;

        vkfw::WindowHints hints;
        hints.resizable = false;

        init_logger();
        vkfw::Window window = vkfw::createWindow(resolution.width, resolution.height, "Hello, world!", hints);
        while (!window.shouldClose())
        {
            vkfw::pollEvents();
            if (window.getKey(vkfw::Key::eEscape))
                window.setShouldClose(true);
        };

        LOG_INFO("HELLO, WORLD!");
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("uncaught exception: {}", e.what());
        exit_code = EXIT_FAILURE;
    }

    return exit_code;
}