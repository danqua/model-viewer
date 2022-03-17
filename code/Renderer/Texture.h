#pragma once
#include "Common/Types.h"
#include <glad/glad.h>

namespace Harm
{
    class Texture
    {
    public:
        Texture(const void* data, s32 width, s32 height);
        ~Texture();
        void Bind(s32 slot = 0) const;
        void Unbind() const;
    private:
        GLuint m_Handle;
    };
}