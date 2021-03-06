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
    src/app.cpp
  
    src/vki/camera.h
    src/vki/camera.cpp
    src/vki/vertex.h
    src/vki/vertex.cpp
    src/vki/vulkan_assist.h
    src/vki/vulkan_assist.cpp
    src/vki/vulkan_debug.h
    src/vki/vulkan_debug.cpp
    src/vki/vulkan_instance.h
    src/vki/vulkan_instance.cpp
    src/vki/vulkan_device.h
    src/vki/vulkan_device.cpp
    src/vki/vulkan_swapchain.h
    src/vki/vulkan_swapchain.cpp
    src/vki/vulkan_renderpass.h
    src/vki/vulkan_renderpass.cpp
    src/vki/vulkan_pipeline.h
    src/vki/vulkan_pipeline.cpp
    src/vki/vulkan_interface.h
    src/vki/vulkan_interface.cpp
    
    src/error.h
    src/json.h
    src/glm.h
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
add_compile_definitions(VULKAN_HPP_NO_STRUCT_CONSTRUCTORS)
add_compile_definitions(VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1)

# GLFW
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${TARGET_NAME} PRIVATE glfw)
# VKFW (GLFW Modern Cpp binding)
target_include_directories(${TARGET_NAME} PRIVATE external/vkfw/include)

# stb
find_path(STB_INCLUDE_DIRS "stb.h")
target_include_directories(${TARGET_NAME} PRIVATE ${STB_INCLUDE_DIRS})

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
