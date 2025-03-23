// =====================================================================================
// Texture2D class was made by George Mavroeidis as a utility class for texture loading.
// It contains a set of data that are loaded in the constructor of the class and stored
// within the declared object. It can be used by all objects that load texture files
// =====================================================================================

#pragma once

#ifndef TEXTURE_H
#define TEXTURE_H

#include <assimp/material.h>

#include "asset.h"

enum class TextureUseType
{
    Model,
    Particle
};

// Texture2D is able to store and configure a texture in OpenGL.
// It also hosts utility functions for easy management.
class Texture2D : public Asset
{
public:
    Texture2D() = default;
    Texture2D(const std::string& path, const std::string& file_name, aiTextureType type, TextureUseType textureUse, bool verbose = true);
    ~Texture2D() override = default;
    void bind() const; // binds the texture as the current active GL_TEXTURE_2D texture object
    
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