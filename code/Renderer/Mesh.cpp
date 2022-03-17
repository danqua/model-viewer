#include "Mesh.h"

Harm::Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<u32>& indices, s32 materialIndex) :
    m_Vao(0),
    m_Vbo(0),
    m_Ibo(0),
    m_NumVertices(static_cast<u32>(vertices.size())),
    m_NumIndices(static_cast<u32>(indices.size())),
    m_MaterialIndex(materialIndex)
{
    glGenVertexArrays(1, &m_Vao);
    glBindVertexArray(m_Vao);

    glGenBuffers(1, &m_Vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(Vertex) * vertices.size()), vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, uv));

    if (!indices.empty())
    {
        glGenBuffers(1, &m_Ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(u32) * indices.size()), indices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Harm::Mesh::~Mesh()
{
    glDeleteBuffers(1, &m_Vbo);
    glDeleteBuffers(1, &m_Ibo);
    glDeleteVertexArrays(1, &m_Vao);
}

void Harm::Mesh::Draw() const
{
    glBindVertexArray(m_Vao);

    if (m_NumIndices > 0)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ibo);
        glDrawElements(GL_TRIANGLES, m_NumIndices, GL_UNSIGNED_INT, 0);
    }
    else
    {
        glDrawArrays(GL_TRIANGLES, 0, m_NumVertices);
    }
}
