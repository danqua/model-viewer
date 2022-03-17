#include "Skybox.h"
#include "Image.h"

Harm::Skybox::Skybox(const std::string& filepath)
{
    m_Model = std::make_shared<Model>("models/cube.fbx");
    m_Shader = std::make_shared<Shader>("shaders/cubemap.glsl");
    Image images[] = {
        filepath + "posx.png",
        filepath + "negx.png",
        filepath + "posy.png",
        filepath + "negy.png",
        filepath + "posz.png",
        filepath + "negz.png"
    };

    glGenTextures(1, &m_Cubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_Cubemap);
    glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, GL_RGB8, images->GetWidth(), images->GetHeight());

    for (u32 i = 0; i < 6; i++)
        glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 0, 0, images->GetWidth(), images->GetHeight(), GL_RGBA, GL_UNSIGNED_BYTE, images[i].GetPixels());

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

Harm::Skybox::~Skybox()
{
    glDeleteTextures(1, &m_Cubemap);
}

void Harm::Skybox::SetProjectionViewMatrix(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix)
{
    m_Shader->Bind();
    m_Shader->SetUniformMat4(m_Shader->GetUniformLocation("ProjectionMatrix"), projectionMatrix);
    m_Shader->SetUniformMat4(m_Shader->GetUniformLocation("ViewMatrix"), viewMatrix);
}

void Harm::Skybox::Draw() const
{
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_Cubemap);
    m_Shader->Bind();
    m_Model->Draw();
    glEnable(GL_CULL_FACE);
    glDepthMask(GL_TRUE);
}
