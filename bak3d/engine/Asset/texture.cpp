/* ===========================================================================
The MIT License (MIT)

Copyright (c) 2022-2026 George Mavroeidis - GeoGraphics

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
=========================================================================== */

// =====================================================================================
// Texture2D class was made by George Mavroeidis as a utility class for texture loading.
// It contains a set of data that are loaded in the constructor of the class and stored
// within the declared object. It can be used by all objects that load texture files
// =====================================================================================

#include <glad.h>
#include <iostream>

#include "stb_image.h"
#include "texture.h"

#include <cassert>
#include <unordered_map>

#include "Core/logger.h"

using namespace std;

Texture2D::Texture2D(const string& path, const string& file_name, const bool is_editor)
    : Asset(path, file_name)
    , m_is_editor(is_editor)
{
    // Bypass gen texture ID with unique ID
    glGenTextures(1, &m_texture_id);

    if (const auto data = stbi_load(m_path.c_str(), &m_width, &m_height, &m_nb_color_channels, 0))
    {
        GLenum format;
        GLenum internal_format;
        
        switch (m_nb_color_channels)
        {
        case 1: 
            format = GL_RED;  
            internal_format = GL_RED; 
            break;
        case 3: 
            format = GL_RGB;  
            // Use GL_SRGB to enable hardware gamma correction for color maps
            internal_format = GL_SRGB; 
            break;
        case 4: 
            format = GL_RGBA; 
            internal_format = GL_SRGB_ALPHA; 
            break;
        default: 
            B3D_LOG_ERROR("Invalid texture format for texture %s with ID %d", file_name, m_texture_id); 
            stbi_image_free(data);
            return;
        }

        glBindTexture(GL_TEXTURE_2D, m_texture_id);
        glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, data);  // NOLINT(bugprone-narrowing-conversions)
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glBindTexture(GL_TEXTURE_2D, 0);

        stbi_image_free(data);

        B3D_LOG_INFO("Texture %s loaded with texture ID: %d", m_object_name.c_str(), m_texture_id);
    }
    else
    {
        B3D_LOG_ERROR("Texture failed to load at path: %s", path.c_str());
        stbi_image_free(data);
    }
}

Texture2D::Texture2D(const std::string& path, const std::string& file_name, CubemapImageData cubemap_image_data)
    : Asset(path, file_name)
    , m_target(GL_TEXTURE_CUBE_MAP)
    , m_nb_color_channels(3)
    , m_is_editor(false)
{
    m_nb_color_channels = 3;
    m_is_editor = true;
    m_target = GL_TEXTURE_CUBE_MAP;

    // Order must match GL_TEXTURE_CUBE_MAP_POSITIVE_X + i
    static constexpr array<const char*, 6> FACE_NAMES = { "right", "left", "top", "bottom", "front", "back" };
    static constexpr array<const char*, 2> EXTENSIONS = { ".png", ".jpg" };

    bool success = true;
    stbi_set_flip_vertically_on_load(false); // cubemaps use a top-left origin

    for (size_t face = 0; face < 6 && success; ++face)
    {
        std::string face_path;
        for (const char* extension : EXTENSIONS)
        {
            if (auto candidate = path + "/" + (string(FACE_NAMES[face]) + extension); filesystem::exists(candidate))
            {
                face_path = candidate;
                break;
            }
        }

        if (face_path.empty())
        {
            B3D_LOG_ERROR("Cubemap face '%s' not found in %s", FACE_NAMES[face], path.c_str());
            success = false;
            break;
        }

        int width = 0, height = 0, channels = 0;
        unsigned char* data = stbi_load(face_path.c_str(), &width, &height, &channels, 3); // force RGB
        if (!data)
        {
            B3D_LOG_ERROR("Failed to load cubemap face: %s", face_path.c_str());
            success = false;
            break;
        }

        if (width != height || (face > 0 && (width != cubemap_image_data.width || height != cubemap_image_data.height)))
        {
            B3D_LOG_ERROR("Cubemap face %s must be square and match other faces.", face_path.c_str());
            stbi_image_free(data);
            success = false;
            break;
        }

        cubemap_image_data.width = width;
        cubemap_image_data.height = height;
        cubemap_image_data.faces[face].assign(data, data + static_cast<size_t>(width) * height * 3);
        stbi_image_free(data);
    }

    stbi_set_flip_vertically_on_load(true); // restore engine-wide default

    if (!success || !cubemap_image_data.is_valid())
    {
        B3D_LOG_ERROR("Cubemap failed to load from: %s", path.c_str());
        return; // m_texture_id stays 0
    }

    m_width = cubemap_image_data.width;
    m_height = cubemap_image_data.height;

    glGenTextures(1, &m_texture_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture_id);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // tightly packed RGB rows
    for (GLuint face = 0; face < 6; ++face)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_RGB8, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, cubemap_image_data.faces[face].data());
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS); // no visible seams on face edges (globally)

    glObjectLabel(GL_TEXTURE, m_texture_id, -1, file_name.c_str());
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    B3D_LOG_INFO("Cubemap '%s' loaded (%dx%d per face), texture ID: %d", file_name.c_str(), m_width, m_height, m_texture_id);
}

Texture2D::~Texture2D()
{
    glDeleteTextures(1, &m_texture_id);
}

void Texture2D::bind(const int unit_slot) const
{
    assert(m_texture_id && "Binding an invalid texture");
    glActiveTexture(GL_TEXTURE0 + unit_slot);
    glBindTexture(m_target, m_texture_id);
}

void Texture2D::unbind(const int unit_slot, const GLenum target)
{
    glActiveTexture(GL_TEXTURE0 + unit_slot);
    glBindTexture(target, 0);
}

void Texture2D::unbind_all(const int slot_count)
{
    for (int i = 0; i < slot_count; i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

bool Texture2D::operator==(const Texture2D& other) const
{
    return m_object_id == other.m_object_id
        && m_file_name == other.m_file_name;
}

bool Texture2D::operator!=(const Texture2D& other) const
{
    return m_object_id != other.m_object_id
        || m_file_name != other.m_file_name;
}