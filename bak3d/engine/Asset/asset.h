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

#pragma once

#include <string>

#include "Core/bak3d_object.h"

/*
 * Abstract class for setting up assets loaded from files such as shaders, textures, models, etc.
 * It contains general data about file naming and identification.
 */
class Asset : public Bak3DObject
{
public:
    Asset() = default;
    Asset(const std::string& path, const std::string& asset_name) : Bak3DObject(asset_name), m_path(path)
    {
        m_directory = m_path.substr(0, m_path.find_last_of('/'));
        m_file_name = path.substr(m_path.find_last_of('/') + 1);
        m_object_name = asset_name;
    }
    
    virtual ~Asset() = 0;

    std::string get_path() const { return m_path; }
    std::string get_directory() const { return m_directory; }
    std::string get_file_name() const { return m_file_name; }

    void set_path(const std::string& path) { m_path = path; }
    void set_directory(const std::string& directory) { m_directory = directory; }
    void set_file_name(const std::string& file_name) { m_file_name = file_name; }
protected:
    std::string m_path;
    std::string m_directory;
    std::string m_file_name;
};

inline Asset::~Asset() = default;