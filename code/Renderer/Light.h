#pragma once
#include "Common/Types.h"

namespace Harm
{
    struct Light
    {
        glm::vec3 color = glm::vec3(1.0f);
        f32 ambientIntensity = 0.3f;
        f32 diffuseIntensity = 1.0f;
    };

    struct DirectionalLight : Light
    {
        glm::vec3 direction;
    };

    struct PointLight : Light
    {
        glm::vec3 position;
        f32 constant = 1.0f;
        f32 linear = 0.35f;
        f32 quadratic = 0.44f;
    };
}