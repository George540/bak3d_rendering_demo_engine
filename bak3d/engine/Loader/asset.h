// =====================================================================================
// Asset class was made by George Mavroeidis as an abstract class for loaded assets
// such as shaders, textures, models, etc. It contains general data about file naming
// and identification. It can be used for all content created outside of this program.
// =====================================================================================

#pragma once

#include <string>
#include <glad/glad.h> 

class Asset
{
public:
    Asset() = default;
    Asset(const std::string& path, const std::string& asset_name, const GLuint id) : m_path(path), m_id(id)
    {
        m_id = 0;
        m_path = path;
        m_directory = m_path.substr(0, m_path.find_last_of('/'));
        m_file_name = path.substr(m_path.find_last_of('/') + 1);
        m_asset_name = asset_name;
    }
    
    virtual ~Asset() = 0;
    [[nodiscard]] std::string get_path() const { return m_path; }
    [[nodiscard]] std::string get_directory() const { return m_directory; }
    [[nodiscard]] std::string get_file_name() const { return m_file_name; }
    [[nodiscard]] std::string get_asset_name() const { return m_asset_name; }
    [[nodiscard]] GLuint get_asset_id() const { return m_id; }

    void set_path(const std::string& path) { m_path = path; }
    void set_directory(const std::string& directory) { m_directory = directory; }
    void set_file_name(const std::string& file_name) { m_file_name = file_name; }
    void set_asset_name(const std::string& asset_name) { m_asset_name = asset_name; }
    void set_asset_id(const GLuint id) { m_id = id; }
protected:
    std::string m_path;
    std::string m_directory;
    std::string m_file_name;
    std::string m_asset_name;
    GLuint m_id;
};

inline Asset::~Asset() = default;