#pragma once
#include "Common/Types.h"
#include <glad/glad.h>
#include <vector>

namespace Harm
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;
    };

    class Mesh
    {
    public:
        Mesh(const std::vector<Vertex>& vertices, const std::vector<u32>& indices, s32 materialIndex);
        ~Mesh();
        void Draw() const;
        inline s32 GetMaterialIndex() const { return m_MaterialIndex; }
    private:
        GLuint m_Vao;
        GLuint m_Vbo;
        GLuint m_Ibo;
        u32 m_NumVertices;
        u32 m_NumIndices;
        s32 m_MaterialIndex;
    };
}