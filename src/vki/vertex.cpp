#include "vertex.h"

namespace vki
{
vk::VertexInputBindingDescription Vertex::get_binding_description()
{
    return vk::VertexInputBindingDescription {
        .binding    = 0,
        .stride     = sizeof(Vertex),
        .inputRate  = vk::VertexInputRate::eVertex,
    };
}

std::vector<vk::VertexInputAttributeDescription> Vertex::get_attribute_descriptions()
{
    return {
        vk::VertexInputAttributeDescription { // [0] -> position description
            .location   = 0,
            .binding    = 0,
            .format     = vk::Format::eR32G32B32Sfloat,
            .offset     = offsetof(Vertex, position)
        },
        vk::VertexInputAttributeDescription { // [1] -> color description
            .location   = 1,
            .binding    = 0,
            .format     = vk::Format::eR32G32B32Sfloat,
            .offset     = offsetof(Vertex, color)
        }
    };
}

bool Vertex::operator==(const Vertex& other) const
{
    return position == other.position && color == other.color;
}

}

// const std::vector<Vertex> vertices {
//     { { -0.5f,  -0.5f,  +0.5f }, { 0.7f, 0.2f, 0.2f }, { 0.f, 0.f } }, // top
//     { { +0.5f,  -0.5f,  +0.5f }, { 0.7f, 0.2f, 0.2f }, { 1.f, 0.f } },
//     { { +0.5f,  +0.5f,  +0.5f }, { 0.7f, 0.2f, 0.2f }, { 1.f, 1.f } },
//     { { -0.5f,  +0.5f,  +0.5f }, { 0.7f, 0.2f, 0.2f }, { 0.f, 1.f } },

//     { { -0.5f,  -0.5f,  -0.5f }, { 0.6f, 0.1f, 0.3f }, { 0.f, 0.f } }, // bottom
//     { { -0.5f,  +0.5f,  -0.5f }, { 0.6f, 0.1f, 0.3f }, { 0.f, 1.f } },
//     { { +0.5f,  +0.5f,  -0.5f }, { 0.6f, 0.1f, 0.3f }, { 1.f, 1.f } },
//     { { +0.5f,  -0.5f,  -0.5f }, { 0.6f, 0.1f, 0.3f }, { 1.f, 0.f } },

//     { { -0.5f,  -0.5f,  +0.5f }, { 0.2f, 0.5f, 0.4f }, { 0.f, 0.f } }, // left
//     { { -0.5f,  +0.5f,  +0.5f }, { 0.2f, 0.5f, 0.4f }, { 1.f, 0.f } },
//     { { -0.5f,  +0.5f,  -0.5f }, { 0.2f, 0.5f, 0.4f }, { 1.f, 1.f } },
//     { { -0.5f,  -0.5f,  -0.5f }, { 0.2f, 0.5f, 0.4f }, { 0.f, 1.f } },

//     { { +0.5f,  -0.5f,  +0.5f }, { 0.5f, 0.1f, 0.5f }, { 0.f, 0.f } }, // right
//     { { +0.5f,  -0.5f,  -0.5f }, { 0.5f, 0.1f, 0.5f }, { 0.f, 1.f } },
//     { { +0.5f,  +0.5f,  -0.5f }, { 0.5f, 0.1f, 0.5f }, { 1.f, 1.f } },
//     { { +0.5f,  +0.5f,  +0.5f }, { 0.5f, 0.1f, 0.5f }, { 1.f, 0.f } },

//     { { -0.5f,  -0.5f,  +0.5f }, { 0.6f, 0.5f, 0.1f }, { 0.f, 0.f } }, // back
//     { { -0.5f,  -0.5f,  -0.5f }, { 0.6f, 0.5f, 0.1f }, { 0.f, 1.f } },
//     { { +0.5f,  -0.5f,  -0.5f }, { 0.6f, 0.5f, 0.1f }, { 1.f, 1.f } },
//     { { +0.5f,  -0.5f,  +0.5f }, { 0.6f, 0.5f, 0.1f }, { 1.f, 0.f } },

//     { { -0.5f,  +0.5f,  +0.5f }, { 0.1f, 0.5f, 0.1f }, { 0.f, 0.f } }, // front
//     { { +0.5f,  +0.5f,  +0.5f }, { 0.1f, 0.5f, 0.1f }, { 1.f, 0.f } },
//     { { +0.5f,  +0.5f,  -0.5f }, { 0.1f, 0.5f, 0.1f }, { 1.f, 1.f } },
//     { { -0.5f,  +0.5f,  -0.5f }, { 0.1f, 0.5f, 0.1f }, { 0.f, 1.f } },
// };

// const std::vector<uint16_t> indices {
//     0, 1, 2, 2, 3, 0,           // top
//     4, 5, 6, 6, 7, 4,           // bottom
//     8, 9, 10, 10, 11, 8,        // left
//     12, 13, 14, 14, 15, 12,     // right
//     16, 17, 18, 18, 19, 16,     // back
//     20, 21, 22, 22, 23, 20,     // front
// };