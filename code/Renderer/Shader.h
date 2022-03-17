#pragma once
#include "Common/Types.h"
#include <glad/glad.h>
#include <string>
#include <unordered_map>

namespace Harm
{
    struct Uniform
    {
        GLchar name[128];
        GLint size;
        GLint location;
        GLsizei length;
        GLenum type;
    };

    class Shader
    {
    public:
        Shader(const std::string& filename);
        Shader(const std::string& vertexCode, const std::string& fragmentCode);
        ~Shader();
        void Bind() const;
        void Unbind() const;
        void SetUniformInt(s32 location, s32 value) const;
        void SetUniformFloat(s32 location, f32 value) const;
        void SetUniformVec2(s32 location, const glm::vec2& value) const;
        void SetUniformVec3(s32 location, const glm::vec3& value) const;
        void SetUniformVec4(s32 location, const glm::vec4& value) const;
        void SetUniformMat4(s32 location, const glm::mat4& value) const;
        s32 GetUniformLocation(const std::string& name) const;
        s32 GetAttributeLocation(const std::string& name) const;
    private:
        GLuint CreateShaderProgram(const char* vertexCode, const char* fragmentCode) const;
        void QueryActiveUniforms();
    private:
        GLuint m_Program;
        std::unordered_map<std::string, Uniform> m_ActiveUniforms;
    };
}