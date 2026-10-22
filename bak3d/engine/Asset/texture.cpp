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
            B3D_LOG_ERROR("Invalid texture format for texture %s with ID %d", file_name.c_str(), m_texture_id); 
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
    // Grid cell (column, row) for each GL face: +X, -X, +Y, -Y, +Z, -Z
    struct Layout { int columns; int rows; array<pair<int, int>, 6> cells; };

    static constexpr Layout HORIZONTAL_CROSS = {.columns = 4, .rows = 3, .cells = {{ {2,1}, {0,1}, {1,0}, {1,2}, {1,1}, {3,1} }} };
    static constexpr Layout VERTICAL_CROSS   = {.columns = 3, .rows = 4, .cells = {{ {2,1}, {0,1}, {1,0}, {1,2}, {1,1}, {1,3} }} };
    static constexpr Layout HORIZONTAL_STRIP = {.columns = 6, .rows = 1, .cells = {{ {0,0}, {1,0}, {2,0}, {3,0}, {4,0}, {5,0} }} };
    static constexpr Layout VERTICAL_STRIP   = {.columns = 1, .rows = 6, .cells = {{ {0,0}, {0,1}, {0,2}, {0,3}, {0,4}, {0,5} }} };

    // Load the single image
    stbi_set_flip_vertically_on_load(false);

    int width = 0, height = 0, channels = 0;
    unsigned char* source = stbi_load(path.c_str(), &width, &height, &channels, 3); // force RGB

    stbi_set_flip_vertically_on_load(true); // restore engine-wide default

    if (!source)
    {
        B3D_LOG_ERROR("Failed to load cubemap image: %s", file_name.c_str());
        return; // m_texture_id stays 0
    }

    // Detect layout from aspect ratio
    const Layout* layout = nullptr;
    if (width * 3 == height * 4)      layout = &HORIZONTAL_CROSS;
    else if (width * 4 == height * 3) layout = &VERTICAL_CROSS;
    else if (width == height * 6)     layout = &HORIZONTAL_STRIP;
    else if (height == width * 6)     layout = &VERTICAL_STRIP;

    if (!layout || width % layout->columns != 0 || height % layout->rows != 0)
    {
        B3D_LOG_ERROR("Unsupported cubemap layout (%dx%d) for %s. Expected 4:3, 3:4, 6:1 or 1:6.", width, height, file_name.c_str());
        stbi_image_free(source);
        return;
    }

    const int face_size = width / layout->columns;
    if (face_size != height / layout->rows)
    {
        B3D_LOG_ERROR("Cubemap cells are not square in %s.", file_name.c_str());
        stbi_image_free(source);
        return;
    }

    // Slice into 6 tightly packed RGB face buffers
    const size_t row_bytes = static_cast<size_t>(face_size) * 3;
    for (size_t face = 0; face < 6; ++face)
    {
        const auto [column, row] = layout->cells[face];
        cubemap_image_data.faces[face].resize(row_bytes * face_size);

        for (int y = 0; y < face_size; ++y)
        {
            const unsigned char* src = source + (static_cast<size_t>(row * face_size + y) * width + static_cast<size_t>(column) * face_size) * 3;
            memcpy(cubemap_image_data.faces[face].data() + y * row_bytes, src, row_bytes);
        }
    }

    cubemap_image_data.width = face_size;  // face size, not atlas size
    cubemap_image_data.height = face_size;
    stbi_image_free(source);

    if (!cubemap_image_data.is_valid())
    {
        B3D_LOG_ERROR("Cubemap failed to load from: %s", path.c_str());
        return;
    }

    m_width = cubemap_image_data.width;
    m_height = cubemap_image_data.height;

    // Cubemap upload
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
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS); // global state, no visible seams on face edges

    glObjectLabel(GL_TEXTURE, m_texture_id, -1, file_name.c_str());
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    // 2D thumbnail from the +Z face for editor pickers
    const size_t preview_row_bytes = static_cast<size_t>(m_width) * 3;
    vector<unsigned char> preview(preview_row_bytes * m_height);
    for (int y = 0; y < m_height; ++y)
    {
        memcpy(preview.data() + y * preview_row_bytes, cubemap_image_data.faces[4].data() + (m_height - 1 - y) * preview_row_bytes, preview_row_bytes);
    }

    GLuint thumbnail = 0;
    glGenTextures(1, &thumbnail);
    glBindTexture(GL_TEXTURE_2D, thumbnail);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, preview.data());
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    set_thumbnail_id(thumbnail);

    B3D_LOG_INFO("Cubemap '%s' loaded (%dx%d per face), texture ID: %d", file_name.c_str(), m_width, m_height, m_texture_id);
}

Texture2D::~Texture2D()
{
    glDeleteTextures(1, &m_texture_id);
	if (is_cubemap() && m_thumbnail_id != 0)
	{
		glDeleteTextures(1, &m_thumbnail_id);
	}
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