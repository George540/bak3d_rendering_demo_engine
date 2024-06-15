// =====================================================================================
// FileLoader class was made by George Mavroeidis as a utility class for file loading.
// It contains a lot of utility functions for loading files such as shaders, models,
// textures, and generally folders and directories in order to get content to the engine.
// =====================================================================================

#ifndef FILELOADER_H
#define FILELOADER_H

#include <vector>
#include <string>
#include <unordered_map>

enum FileType 
{
	png,
	jpg,
	vs,
	fs,
	obj,
	mtl,
	fbx
};

/**
 * \brief Class is static, so it's globally used and there is only one copy of it
 */
class FileLoader
{
private:
public:
	static std::unordered_map<FileType, std::string> enum_to_string_map;

	static std::vector<std::string> get_directories(const std::filesystem::path& path);
	static std::vector<std::string> get_files_by_type(const std::filesystem::path& path, FileType type);
	static std::string get_name_from_filename(const std::string filename);
	static std::string enum_to_string(FileType type);
	static std::vector<char*> get_vector_items_to_array(const std::vector<std::string>& vector);
};

#endif