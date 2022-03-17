#pragma once
#include "Common/Types.h"
#include "Texture.h"
#include <memory>

namespace Harm
{
    struct Material
    {
        glm::vec3 ambientColor = glm::vec3(1.0f);
        glm::vec3 diffuseColor = glm::vec3(1.0f);
        glm::vec3 specularColor = glm::vec3(1.0f);

        std::shared_ptr<Texture> diffuseTexture;
        
        f32 shininess = 32.0f;
    };
}