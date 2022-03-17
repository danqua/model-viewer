#pragma once
#include "Common/Types.h"
#include <string>

namespace Harm
{
    class Image
    {
    public:
        Image(const std::string& filename);
        ~Image();
        inline s32 GetWidth() const { return m_Width; }
        inline s32 GetHeight() const { return m_Height; }
        inline s32 GetComponents() const { return m_Components; }
        inline u8* GetPixels() const { return m_Pixels; }
    private:
        s32 m_Width;
        s32 m_Height;
        s32 m_Components;
        u8* m_Pixels;
    };
}