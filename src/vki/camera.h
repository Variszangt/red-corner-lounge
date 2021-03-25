#pragma once

#include "glm.h"

namespace vki
{
struct Camera
{
    void set_extent(float width, float height)
    {
        extent = { width, height };
        aspect_ratio = width / height;
    }

    glm::mat4 get_view() const
    {
        return glm::lookAt(eye, target, up);
    }
    
    glm::mat4 get_projection() const
    {
        auto projection = glm::perspective(fov, aspect_ratio, 0.1f, 10.f);
        if (flip_y)
            projection[1][1] *= -1;
        
        return projection;
    }

private:
    glm::vec2 extent        = {};
    float     aspect_ratio  = {};

    glm::vec3 eye       = glm::vec3(1.f, 1.f, 1.f);
    glm::vec3 target    = glm::vec3(0.f, 0.f, 0.f);
    glm::vec3 up        = glm::vec3(0.f, 0.f, 1.f);

    float fov = glm::radians(90.f);

    bool flip_y = true;
};
}