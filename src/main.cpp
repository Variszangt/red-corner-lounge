#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

#include "log.h"

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
        init_logger();

        LOG_INFO("HELLO, WORLD!");
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("uncaught exception: {}", e.what());
        exit_code = EXIT_FAILURE;
    }

    return exit_code;
}