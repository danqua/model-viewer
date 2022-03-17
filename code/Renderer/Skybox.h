#pragma once
#include "Common/Types.h"
#include "Shader.h"
#include "Model.h"
#include <string>
#include <glad/glad.h>

namespace Harm
{
    class Skybox
    {
    public:
        Skybox(const std::string& filepath);
        ~Skybox();

        void SetProjectionViewMatrix(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix);
        void Draw() const;

    private:
        GLuint m_Cubemap;
        std::shared_ptr<Shader> m_Shader;
        std::shared_ptr<Model> m_Model;
    };
}