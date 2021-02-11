#pragma once

#include <fmt/format.h>
#include <exception>

template<typename T, size_t S>
inline constexpr size_t get_filename_offset(const T (& str)[S], size_t i = S - 1)
{
    return (str[i] == '/' || str[i] == '\\') ? i + 1 : (i > 0 ? get_filename_offset(str, i - 1) : 0);
}

// __FILE__ without path
#define FILENAME &(__FILE__[get_filename_offset(__FILE__)])

#define LOCATION fmt::format("{}:{}, {}", FILENAME, __LINE__, __FUNCTION__)

// Throws std::runtime_error using a formatted string as message. Additionally, the location of the throw will be appended to the message.
#define THROW_ERROR(format_str, ...) throw std::runtime_error(fmt::format(format_str, __VA_ARGS__) + " (thrown from: " + LOCATION + ')')