cmake_minimum_required(VERSION 3.18.0)
set(TARGET_NAME "red-corner-lounge")
project(${TARGET_NAME})

add_executable(${TARGET_NAME}
    src/main.cpp

    src/log.h
    src/log.cpp
    src/config.h
    src/config.cpp
    src/utility.h
    src/utility.cpp
    src/app.h
    src/vulkan_interface.h
    
    src/error.h
    src/json.h
)

# Local include folder:
target_include_directories(${TARGET_NAME} PRIVATE src/)

# Use C++20:
target_compile_features(${TARGET_NAME} PRIVATE cxx_std_20)

# Enforce warnings:
if(MSVC)
  target_compile_options(${TARGET_NAME} PRIVATE /W4 /WX)
endif()

# Disable tests:
# target_compile_definitions(${TARGET_NAME} PRIVATE "DOCTEST_CONFIG_DISABLE")

# /*------------------------------------------------------------------*/
# External dependencies:

# Vulkan
find_package(Vulkan REQUIRED)
target_include_directories(${TARGET_NAME} PRIVATE ${Vulkan_INCLUDE_DIR})
target_link_libraries(${TARGET_NAME} PRIVATE ${Vulkan_LIBRARIES})

# GLFW
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${TARGET_NAME} PRIVATE glfw)
# VKFW (GLFW Modern Cpp binding)
target_include_directories(${TARGET_NAME} PRIVATE external/vkfw/include)

# GLM
find_package(glm CONFIG REQUIRED)
target_link_libraries(${TARGET_NAME} PRIVATE glm)

# doctest
find_package(doctest CONFIG REQUIRED)
target_link_libraries(${TARGET_NAME} PRIVATE doctest::doctest)

# spdlog
find_package(spdlog CONFIG REQUIRED)
target_link_libraries(${TARGET_NAME} PRIVATE spdlog::spdlog)

# fmt
find_package(fmt CONFIG REQUIRED)
target_link_libraries(${TARGET_NAME} PRIVATE fmt::fmt)

# nlohmann_json
find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(${TARGET_NAME} PRIVATE nlohmann_json nlohmann_json::nlohmann_json)
