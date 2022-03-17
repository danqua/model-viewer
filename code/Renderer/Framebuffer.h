#pragma once
#include "Common/Types.h"
#include <glad/glad.h>

namespace Harm
{
    class Framebuffer
    {
    public:
        Framebuffer(s32 width, s32 height);
        ~Framebuffer();
        void Bind() const;
        void Unbind() const;
        void BindColorTexture(s32 slot = 0) const;
        void BindDepthTexture(s32 slot = 0) const;
        inline s32 GetWidth() const { return m_Width; }
        inline s32 GetHeight() const { return m_Height; }
    private:
        GLuint m_Handle;
        GLuint m_ColorTexture;
        GLuint m_DepthTexture;
        s32 m_Width;
        s32 m_Height;
    };
}