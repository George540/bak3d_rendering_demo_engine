#include <glad.h>
#include <iostream>

#include "stb_image.h"
#include "texture.h"
#include "file_loader.h"

using namespace std;

Texture2D::Texture2D(const std::string& path, aiTextureType type, TextureUseType textureUse, bool verbose)
    :
    m_texture_type(type),
    m_texture_use_type(textureUse)
{
    m_path = path;
    m_directory = m_path.substr(0, m_path.find('.'));
    m_file_name = m_path.substr(m_path.find('.') + 1);
    m_asset_name = m_file_name.substr(0, m_file_name.find('.'));
    
    if (verbose)
    {
        std::cout << "Loading texture file: " << m_file_name << std::endl;
    }

    glGenTextures(1, &m_id);
    
    const auto data = stbi_load(m_path.c_str(), &m_width, &m_height, &m_nb_color_channels, 0);

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

        glBindTexture(GL_TEXTURE_2D, m_id);
        glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, data);  // NOLINT(bugprone-narrowing-conversions)
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_id, 0);
        glBindTexture(GL_TEXTURE_2D, 0);

        stbi_image_free(data);
    }
    else
    {
        cout << "ERROR::PARTICLEGENERATOR::FILE_NOT_SUCCESSFULLY_READ: Texture failed to load at path: " << path << endl;
        stbi_image_free(data);
    }
}

Texture2D::~Texture2D()
{
    //glBindTexture(GL_TEXTURE_2D, 0);
    //glDeleteTextures(1, &m_ID);
}

void Texture2D::bind() const
{
    glBindTexture(GL_TEXTURE_2D, m_id);
}

void Texture2D::unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

bool Texture2D::operator==(const Texture2D& other) const
{
    return m_id == other.m_id
        && m_file_name == other.m_file_name
        && m_texture_type == other.m_texture_type
        && m_texture_use_type == other.m_texture_use_type;
}

bool Texture2D::operator!=(const Texture2D& other) const
{
    return m_id != other.m_id
        || m_file_name != other.m_file_name
        || m_texture_type != other.m_texture_type
        || m_texture_use_type != other.m_texture_use_type;
}
