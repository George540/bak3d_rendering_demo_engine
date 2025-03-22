// =====================================================================================
// FileLoader class was made by George Mavroeidis as a utility class for file loading.
// It contains a lot of utility functions for loading files such as shaders, models,
// textures, and generally folders and directories in order to get content to the engine.
// =====================================================================================

#pragma once

#ifndef FILELOADER_H
#define FILELOADER_H

#include <vector>
#include <string>
#include <unordered_map>
#include <map>
#include <list>
#include <filesystem>

enum FileType 
{
	png,
	jpg,
	vs,
	vert,
	fs,
	frag,
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
	static std::list<std::pair<std::string, std::string>> get_files_by_type_with_path(const std::filesystem::path& path, FileType type);
	static std::list<std::pair<std::string, std::string>> get_files_by_types_with_path(const std::filesystem::path& path, const std::vector<FileType>& types);
	static std::string get_filename_from_path(const std::filesystem::path& path);
	static std::string get_filename_from_path(const std::string path);
	static std::string get_name_from_filename(const std::string filename);
	static std::string enum_to_string(FileType type);
	static std::vector<char*> get_vector_items_to_array(const std::list<std::pair<std::string, std::string>>, bool is_returning_path = true);
};

#endif