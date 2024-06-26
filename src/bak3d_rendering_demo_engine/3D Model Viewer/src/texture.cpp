#include <iostream>

#include "stb_image.h"
#include "texture.h"
#include "file_loader.h"

using namespace std;

Texture2D::Texture2D() : m_filepath(""), m_texture_type(aiTextureType_DIFFUSE) {}

Texture2D::Texture2D(const std::string& path, aiTextureType type)
    :
    m_filepath(path),
    m_texture_type(type)
{
    m_filename = FileLoader::get_filename_from_path(m_filepath);
    std::cout << "Loading texture file: " << m_filename << std::endl;

    glGenTextures(1, &m_ID);
    
    const auto data = stbi_load(m_filepath.c_str(), &m_width, &m_height, &m_nb_color_channels, 0);

    if (data)
    {
        GLenum format;
        switch (m_nb_color_channels)
        {
        case 1:
            format = GL_RED;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        default:
            format = GL_RGB;
            break;
        }

        glBindTexture(GL_TEXTURE_2D, m_ID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, data);  // NOLINT(bugprone-narrowing-conversions)
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ID, 0);
        glBindTexture(GL_TEXTURE_2D, 0);

        stbi_image_free(data);
    }
    else
    {
        cout << "ERROR::PARTICLEGENERATOR::FILE_NOT_SUCCESSFULLY_READ: Texture failed to load at path: " << path << endl;
        stbi_image_free(data);
    }
}

void Texture2D::bind() const
{
    glBindTexture(GL_TEXTURE_2D, m_ID);
}

bool Texture2D::operator==(const Texture2D& other)
{
    return m_ID == other.m_ID;
}

bool Texture2D::operator!=(const Texture2D& other)
{
    return m_ID != other.m_ID;
}