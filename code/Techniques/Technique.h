#pragma once
#include <glad/glad.h>
#include <unordered_map>
#include <string>

#include "Renderer/Camera.h"
#include "Renderer/Shader.h"
#include "Renderer/Light.h"
#include "Renderer/Material.h"
#include <memory>
#include <vector>

namespace Harm
{
    class Technique
    {
    public:
        virtual ~Technique() = default;
        void Enable() const;
        inline std::shared_ptr<Shader> GetShader() const { return m_Shader; }

        void SetProjectionAndViewMatrix(const glm::mat4& projection, const glm::mat4& view) const;
    protected:
        std::shared_ptr<Shader> m_Shader;
    };

    class LightTechnique : public Technique
    {
    public:
        LightTechnique();

        void SetDirectionalLight(const DirectionalLight& directionalLight) const;
        void SetMaterial(const Material& material) const;
        void SetPointLights(const std::vector<PointLight>& pointLights) const;
    };

    class DepthTechnique : public Technique
    {
    public:
        DepthTechnique();
    };
}