#pragma once

#include <spdlog/spdlog.h>

/*------------------------------------------------------------------*/
// Macros:

#define LOG_TRACE       spdlog::trace
#define LOG_DEBUG       spdlog::debug
#define LOG_INFO        spdlog::info
#define LOG_WARNING     SPDLOG_WARN
#define LOG_ERROR       SPDLOG_ERROR

#define LOG_WARNING_WITHOUT_SOURCE_LOCATION spdlog::warn
#define LOG_ERROR_WITHOUT_SOURCE_LOCATION spdlog::error

/*------------------------------------------------------------------*/
// Logger initialization:

// Directs spdlog to log to both the console and an automatically generated file.
void init_logger();