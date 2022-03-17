#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Harm::Image::Image(const std::string& filename)
{
    m_Pixels = stbi_load(filename.c_str(), &m_Width, &m_Height, &m_Components, STBI_rgb_alpha);

    if (!m_Pixels)
    {
        m_Width = 1;
        m_Height = 1;
        m_Components = 4;
        m_Pixels = new u8[4];
        m_Pixels[0] = 255;
        m_Pixels[1] = 255;
        m_Pixels[2] = 255;
        m_Pixels[3] = 255;
    }
}

Harm::Image::~Image()
{
    stbi_image_free(m_Pixels);
}
