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

#pragma once

#ifndef TEXTURE_H
#define TEXTURE_H

#include <assimp/material.h>

#include "Loader/asset.h"

enum class TextureUseType
{
    Model,
    Particle
};

/*
 * Class that stores texture data in memory loaded from an image asset on disk.
 * Used to bind textures in the GPU when called, integrates with assimp structure as well.
 * @todo: Consider bindless rendering for textures
 */
class Texture2D : public Asset
{
public:
    Texture2D() = default;
    Texture2D(const std::string& path, const std::string& file_name, aiTextureType type, TextureUseType textureUse, bool verbose = true);
    ~Texture2D() override = default;
    void bind(int unit_slot) const; // binds the texture as the current active GL_TEXTURE_2D texture object
    
    static void unbind();
    
    int get_texture_model_index() const { return m_texture_model_index; }
    void set_texture_model_index(int texture_index) { m_texture_model_index = texture_index; }
    
    aiTextureType get_texture_type() const { return m_texture_type; }
    TextureUseType get_texture_use_type() const { return m_texture_use_type; }
    int get_width() const { return m_width; }
    int get_height() const { return m_height; }
    int get_nb_color_channels() const { return m_nb_color_channels; }

    bool operator==(const Texture2D& other) const;
    bool operator!=(const Texture2D& other) const;
private:
    int m_texture_model_index;
    aiTextureType m_texture_type;
    TextureUseType m_texture_use_type;
    int m_width, m_height; // texture image dimensions: width and height of loaded image in pixels
    int m_nb_color_channels; // the number of color components in the loaded image
};

#endif