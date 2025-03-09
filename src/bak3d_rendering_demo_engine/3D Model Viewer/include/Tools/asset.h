// =====================================================================================
// Asset class was made by George Mavroeidis as an abstract class for loaded assets
// such as shaders, textures, models, etc. It contains general data about file naming
// and identification. It can be used for all content created outside of this program.
// =====================================================================================

#pragma once

#include <string>

class Asset
{
public:
    Asset() = default;
    Asset(std::string path, std::string file_name, int id) : m_path(std::move(path)), m_file_name(std::move(file_name)), m_id(id) {}
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
    void set_id(const int id) { m_id = id; }
protected:
    std::string m_path;
    std::string m_directory;
    std::string m_file_name;
    std::string m_asset_name;
    GLuint m_id;
};
