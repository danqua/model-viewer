#include "Shader.h"

#include <fstream>
#include <istream>
#include <string>

std::string geometryShader;

std::pair<std::string, std::string> LoadShaderFiles(const std::string& filename)
{
    std::ifstream fs(filename, std::ios::binary);

    if (!fs.is_open())
        return {};

    std::string vertexCode;
    std::string fragmentCode;
    std::string line;
    std::string* currentCode = nullptr;

    geometryShader.clear();

    while (std::getline(fs, line))
    {
        if (line.find("#vertex") != std::string::npos)
            currentCode = &vertexCode;
        else if (line.find("#fragment") != std::string::npos)
            currentCode = &fragmentCode;
        else if (line.find("#geometry") != std::string::npos)
            currentCode = &geometryShader;
        else
            if (currentCode)
                *currentCode += line + '\n';
    }

    return {
        vertexCode,
        fragmentCode
    };
}

Harm::Shader::Shader(const std::string& vertexCode, const std::string& fragmentCode)
{
    m_Program = CreateShaderProgram(vertexCode.c_str(), fragmentCode.c_str());    
}

Harm::Shader::Shader(const std::string& filename)
{
    auto shaderCode = LoadShaderFiles(filename);
    m_Program = CreateShaderProgram(shaderCode.first.c_str(), shaderCode.second.c_str());
    QueryActiveUniforms();    
}

Harm::Shader::~Shader()
{
    glDeleteProgram(m_Program);
}

void Harm::Shader::Bind() const
{
    glUseProgram(m_Program);
}

void Harm::Shader::Unbind() const
{
    glUseProgram(0);
}

void Harm::Shader::SetUniformInt(s32 location, s32 value) const
{
    glUniform1i(location, value);
}

void Harm::Shader::SetUniformFloat(s32 location, f32 value) const
{
    glUniform1f(location, value);
}

void Harm::Shader::SetUniformVec2(s32 location, const glm::vec2& value) const
{
    glUniform2fv(location, 1, &value[0]);
}

void Harm::Shader::SetUniformVec3(s32 location, const glm::vec3& value) const
{
    glUniform3fv(location, 1, &value[0]);
}

void Harm::Shader::SetUniformVec4(s32 location, const glm::vec4& value) const
{
    glUniform4fv(location, 1, &value[0]);

}

void Harm::Shader::SetUniformMat4(s32 location, const glm::mat4& value) const
{
    glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
}

s32 Harm::Shader::GetUniformLocation(const std::string& name) const
{
    s32 loc = (s32)glGetUniformLocation(m_Program, name.c_str());
    return loc;
    //auto it = m_ActiveUniforms.find(name);
    //if (it != m_ActiveUniforms.end())
    //    return it->second.location;
    //return -1;

}

s32 Harm::Shader::GetAttributeLocation(const std::string& name) const
{
    return static_cast<s32>(glGetAttribLocation(m_Program, name.c_str()));
}

GLuint Harm::Shader::CreateShaderProgram(const char* vertexCode, const char* fragmentCode) const
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexCode, 0);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentCode, 0);
    glCompileShader(fragmentShader);


    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    GLuint geoShader = 0;
    
    if (!geometryShader.empty())
    {
        const char* code = geometryShader.c_str();

        geoShader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geoShader, 1, &code, 0);
        glCompileShader(geoShader);

        glAttachShader(program, geoShader);
    }
    
    
    glLinkProgram(program);

    GLint success = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success) {
        GLsizei ignored;
        char message[4096];
        glGetProgramInfoLog(program, sizeof(message), &ignored, message);
        printf("%s\n", message);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(geoShader);

    return program;
}

void Harm::Shader::QueryActiveUniforms()
{
   m_ActiveUniforms.clear();
   GLint numActiveUniforms = 0;
   glGetProgramiv(m_Program, GL_ACTIVE_UNIFORMS, &numActiveUniforms);
   m_ActiveUniforms.reserve(numActiveUniforms);
   for (GLint i = 0; i < numActiveUniforms; i++)
   {
       Uniform uniform;
       uniform.location = i;
       glGetActiveUniform(m_Program, i, sizeof(uniform.name), &uniform.length, &uniform.size, &uniform.type, uniform.name);
       m_ActiveUniforms.insert(std::make_pair(uniform.name, uniform));
   }
}
