#include <glad.h>
#include <iostream>

#include "stb_image.h"
#include "texture.h"
#include "file_loader.h"

using namespace std;

Texture2D::Texture2D(const string& path, const string& file_name, aiTextureType type, TextureUseType textureUse, bool verbose)
    :
    Asset(path, file_name, 0),
    m_texture_type(type),
    m_texture_use_type(textureUse)
{
    if (verbose)
    {
        cout << "Loading texture file: " << m_file_name << '\n';
    }

    glGenTextures(1, &m_id);

    if (const auto data = stbi_load(m_path.c_str(), &m_width, &m_height, &m_nb_color_channels, 0))
    {
        GLenum format = m_nb_color_channels == 3 ? GL_RGB : GL_RGBA;

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
        cout << "ERROR::PARTICLEGENERATOR::FILE_NOT_SUCCESSFULLY_READ: Texture failed to load at path: " << path << '\n';
        stbi_image_free(data);
    }
}

void Texture2D::bind(int unit_slot) const
{
    glActiveTexture(GL_TEXTURE0 + unit_slot);
    glBindTexture(GL_TEXTURE_2D, m_id);
}

void Texture2D::unbind()
{
    glActiveTexture(GL_TEXTURE0);
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