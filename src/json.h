#pragma once

#include <nlohmann/json.hpp>
#include <fmt/format.h>

#include "log.h"

/*------------------------------------------------------------------*/
// Based on NLOHMANN_JSON_TO and NLOHMANN_JSON_FROM, these alternative macros immediately catch any exceptions and do not treat missing values as errors (missing values will retain their original value).

#define NLOHMANN_JSON_TO_NOEXCEPT(v1)                                                 \
    try                                                                               \
    {                                                                                 \
        nlohmann_json_j[#v1] = nlohmann_json_t.v1;                                    \
    }                                                                                 \
    catch (const std::exception& e)                                                   \
    {                                                                                 \
        LOG_ERROR("'{}' could not be serialized; json exception: {}", #v1, e.what()); \
    }

#define NLOHMANN_JSON_FROM_NOEXCEPT(v1)                                               \
    try                                                                               \
    {                                                                                 \
        nlohmann_json_t.v1 = nlohmann_json_j.value(#v1, nlohmann_json_t.v1);          \
    }                                                                                 \
    catch (const std::exception& e)                                                   \
    {                                                                                 \
        LOG_ERROR("'{}' could not be serialized; json exception: {}", #v1, e.what()); \
    }

/*------------------------------------------------------------------*/
// Based on NLOHMANN_DEFINE_TYPE_INTRUSIVE, this macro defines type serialization methods that log errors instead of throwing them.

#define DEFINE_JSON_SERIALIZABLE(Type, ...) \
    friend void to_json(nlohmann::json& nlohmann_json_j, const Type& nlohmann_json_t) { NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_TO_NOEXCEPT, __VA_ARGS__)) } \
    friend void from_json(const nlohmann::json& nlohmann_json_j, Type& nlohmann_json_t) { NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_FROM_NOEXCEPT, __VA_ARGS__)) }