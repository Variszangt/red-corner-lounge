#include "app.h"

/*------------------------------------------------------------------*/
// Constants:

const std::string CONFIG_FILENAME = "config.json";

const std::string TITLE = "red-corner-lounge.";
const std::tuple VERSION = std::make_tuple(2021, 2, 9);
const std::string VERSION_STR = fmt::format(
    "{:04}.{:02}.{:02}",
    std::get<0>(VERSION),
    std::get<1>(VERSION),
    std::get<2>(VERSION)
);

/*------------------------------------------------------------------*/

int App::run()
{
    int exit_code = EXIT_SUCCESS;

    /*------------------------------------------------------------------*/
    // Startup/Initialization:
    try
    {
        config.load(CONFIG_FILENAME);

        create_window();

        const VulkanRendererInitInfo vulkan_renderer_init_info {
            .config                 = config,
            .application_name       = TITLE,
            .application_version    = VERSION,
            .window                 = window.get()
        };
        vulkan_renderer.init(vulkan_renderer_init_info);
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("startup exception: {}", e.what());
        return EXIT_FAILURE;
        // In case of startup exceptions, exit immediately.
    }

    /*------------------------------------------------------------------*/
    // Main-loop:
    try
    {
        main_loop();
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("main-loop exception: {}", e.what());
        exit_code = EXIT_FAILURE;
        // In case of main-loop exceptions, continue to the standard termination block in order to attempt a save.
    }

    /*------------------------------------------------------------------*/
    // Termination (save and cleanup):
    try
    {
        config.save(CONFIG_FILENAME);
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("termination exception: {}", e.what());
        exit_code = EXIT_FAILURE;
    }

    return exit_code;
}

void App::create_window()
{
    // Init GLFW:
    glfw_instance = vkfw::initUnique();

    // Window properties:
    const size_t window_width  = config.window_width;
    const size_t window_height = config.window_height;
    const std::string window_title = fmt::format("{} (v{})", TITLE, VERSION_STR);

    vkfw::WindowHints hints;
    hints.clientAPI = vkfw::ClientAPI::eNone;

    // Create:
    window = vkfw::createWindowUnique(window_width, window_height, window_title.c_str(), hints);

    // Window callbacks:
    window->callbacks()->on_window_resize =
        [this](vkfw::DynamicCallbackStorage::window_type, const size_t width, const size_t height)
    {
        this->on_resize(static_cast<int>(width), static_cast<int>(height));
    };
}

using Clock = std::conditional<
    std::chrono::high_resolution_clock::is_steady,
    std::chrono::high_resolution_clock,
    std::chrono::steady_clock >::type;
using Time = std::chrono::duration<float, std::deca>;

void App::main_loop()
{
    Clock clock;
    Time previous = clock.now().time_since_epoch();
    
    while (!window->shouldClose())
    {
        vkfw::pollEvents();
        if (window->getKey(vkfw::Key::eEscape))
            window->setShouldClose(true);

        Time current = clock.now().time_since_epoch();
        const float elapsed_time = (current - previous).count();
        previous = current;

        vulkan_renderer.update(elapsed_time);
    };
}

void App::on_resize(const int width, const int height)
{
    LOG_INFO("window resized: ({}; {})", width, height);

    config.window_width  = width;
    config.window_height = height;
}