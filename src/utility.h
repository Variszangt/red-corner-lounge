#pragma once

#include <algorithm>
#include <vector>
#include <ctime>

/*------------------------------------------------------------------*/
// Thread-safe localtime:

[[nodiscard]]
inline std::tm localtime_xp(std::time_t time)
{
    std::tm local_time {};
#if defined(__unix__)
    localtime_r(&time, &local_time);
#elif defined(_MSC_VER)
    localtime_s(&local_time, &time);
#else
    static std::mutex mtx;
    std::lock_guard<std::mutex> lock(mtx);
    local_time = *std::localtime(&time);
#endif
    return local_time;
}

/*------------------------------------------------------------------*/
// container helper functions:

template<typename AC, typename Key>
concept AssociativeContainer = requires(AC container, Key key)
{
    { container.find(key) }->std::same_as<typename AC::const_iterator>;
    { container.cbegin() }->std::same_as<typename AC::const_iterator>;
    { container.cend() }->std::same_as<typename AC::const_iterator>;
}&& std::three_way_comparable<Key>;

template<typename Key, AssociativeContainer<Key> AC>
[[nodiscard]] inline bool contains(const AC& container, const Key& key)
{
    return container.find(key) != container.cend();
};

template<typename T>
requires std::equality_comparable<T>
[[nodiscard]] inline bool contains(const std::vector<T>& vector, const T& t)
{
    return std::find(vector.cbegin(), vector.cend(), t) != vector.cend();
}

template<>
[[nodiscard]] inline bool contains(const std::vector<const char*>& vector, const char* const& cstr)
{
    auto comparator = [cstr](const auto& cstr2)
    {
        return std::strcmp(cstr, cstr2) == 0;
    };
    return std::find_if(vector.cbegin(), vector.cend(), comparator) != vector.cend();
}

/*------------------------------------------------------------------*/
// Bounds checking:

// If x is within [min:max] bounds, returns true;
// otherwise sets x to min/max accordingly and returns false.
template<typename Num>
inline bool assure_bounds(Num& x, const Num min, const Num max)
{
    if (x < min)
    {
        x = min;
        return false;
    }
    else if (x > max)
    {
        x = max;
        return false;
    }
    else
        return true;
}

// If x == num, returns true;
// otherwise sets x to num and returns false.
template<typename Num>
inline bool assure_equals(Num& x, const Num num)
{
    if (x == num)
        return true;
    else
    {
        x = num;
        return false;
    }
}

// If x <= max, returns true;
// otherwise sets x to max and returns false.
template<typename Num>
inline bool assure_less_than_or_equal_to(Num& x, const Num max)
{
    if (x <= max)
        return true;
    else
    {
        x = max;
        return false;
    }
}

// If x >= min, returns true;
// otherwise sets x to min and returns false.
template<typename Num>
inline bool assure_greater_than_or_equal_to(Num& x, const Num min)
{
    if (x >= min)
        return true;
    else
    {
        x = min;
        return false;
    }
}