#include "technique.h"


void Harm::Technique::Enable() const
{
    m_Shader->Bind();
}


void Harm::Technique::SetProjectionAndViewMatrix(const glm::mat4& projection, const glm::mat4& view) const
{
    m_Shader->SetUniformMat4(m_Shader->GetUniformLocation("ProjectionMatrix"), projection);
    m_Shader->SetUniformMat4(m_Shader->GetUniformLocation("ViewMatrix"), view);
}

Harm::LightTechnique::LightTechnique()
{    
    m_Shader = std::make_shared<Shader>("shaders/default.glsl");
}

void Harm::LightTechnique::SetDirectionalLight(const DirectionalLight& directionalLight) const
{
    m_Shader->SetUniformVec3(m_Shader->GetUniformLocation("DirectionalLight.Light.Color"), directionalLight.color);
    m_Shader->SetUniformFloat(m_Shader->GetUniformLocation("DirectionalLight.Light.AmbientIntensity"), directionalLight.ambientIntensity);
    m_Shader->SetUniformFloat(m_Shader->GetUniformLocation("DirectionalLight.Light.DiffuseIntensity"), directionalLight.diffuseIntensity);
    m_Shader->SetUniformVec3(m_Shader->GetUniformLocation("DirectionalLight.Direction"), directionalLight.direction);
}

void Harm::LightTechnique::SetMaterial(const Material& material) const
{
    m_Shader->SetUniformVec3(m_Shader->GetUniformLocation("Material.AmbientColor"), material.ambientColor);
    m_Shader->SetUniformVec3(m_Shader->GetUniformLocation("Material.DiffuseColor"), material.diffuseColor);
    m_Shader->SetUniformVec3(m_Shader->GetUniformLocation("Material.SpecularColor"), material.specularColor);
    m_Shader->SetUniformFloat(m_Shader->GetUniformLocation("Material.Shininess"), material.shininess);

    if (material.diffuseTexture)
    {
        m_Shader->SetUniformInt(m_Shader->GetUniformLocation("DiffuseTexture"), 0);
        material.diffuseTexture->Bind();
    }
}

void Harm::LightTechnique::SetPointLights(const std::vector<PointLight>& pointLights) const
{
    m_Shader->SetUniformInt(m_Shader->GetUniformLocation("NumPointLights"), static_cast<s32>(pointLights.size()));
    for (size_t i = 0; i < pointLights.size(); i++)
    {
        m_Shader->SetUniformVec3(m_Shader->GetUniformLocation("PointLights[" + std::to_string(i) + "].Light.Color"), pointLights[i].color);
        m_Shader->SetUniformFloat(m_Shader->GetUniformLocation("PointLights[" + std::to_string(i) + "].Light.AmbientIntensity"), pointLights[i].ambientIntensity);
        m_Shader->SetUniformFloat(m_Shader->GetUniformLocation("PointLights[" + std::to_string(i) + "].Light.DiffuseIntensity"), pointLights[i].diffuseIntensity);
        m_Shader->SetUniformVec3(m_Shader->GetUniformLocation("PointLights[" + std::to_string(i) + "].Position"), pointLights[i].position);
        m_Shader->SetUniformFloat(m_Shader->GetUniformLocation("PointLights[" + std::to_string(i) + "].Constant"), pointLights[i].constant);
        m_Shader->SetUniformFloat(m_Shader->GetUniformLocation("PointLights[" + std::to_string(i) + "].Linear"), pointLights[i].linear);
        m_Shader->SetUniformFloat(m_Shader->GetUniformLocation("PointLights[" + std::to_string(i) + "].Quadratic"), pointLights[i].quadratic);
    }
}

Harm::DepthTechnique::DepthTechnique()
{
    m_Shader = std::make_shared<Shader>("shaders/depth.glsl");
}