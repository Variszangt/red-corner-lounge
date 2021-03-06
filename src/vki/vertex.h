#pragma once

#include <vector>
#include <vulkan/vulkan.hpp>

#include "glm.h"

namespace vki
{
/*------------------------------------------------------------------*/
// Vertex:

struct Vertex
{
    glm::vec3 position;
    glm::vec3 color;

    static vk::VertexInputBindingDescription get_binding_description();
    static std::vector<vk::VertexInputAttributeDescription> get_attribute_descriptions();

    bool operator==(const Vertex& other) const;
};
}

// Hash for Vertex:
namespace std
{
template<> struct hash<::vki::Vertex>
{
    size_t operator()(const ::vki::Vertex& vertex) const
    {
        return ((hash<glm::vec3>()(vertex.position) ^
            (hash<glm::vec3>()(vertex.color) << 1)) >> 1);
    }
};
}