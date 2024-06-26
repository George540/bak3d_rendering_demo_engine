#include <iostream>

#include "file_loader.h"

using namespace std;

vector<string> FileLoader::get_directories(const filesystem::path& path)
{
	vector<string> directories;

	try
	{
		if (filesystem::exists(path) && filesystem::is_directory(path))
		{
			for (const auto& entry : filesystem::directory_iterator(path))
			{
				if (filesystem::is_directory(entry.path()))
				{
					directories.push_back(entry.path().generic_string());
				}
			}
		}
	}
	catch (const filesystem::filesystem_error& e)
	{
		cerr << "ERROR::FILELOADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << endl;
	}

	return directories;
}

std::vector<std::string> FileLoader::get_files_by_type(const std::filesystem::path& path, FileType type)
{
	vector<string> files;

	try
	{
		if (filesystem::exists(path) && filesystem::is_directory(path))
		{
			for (const auto& entry : filesystem::recursive_directory_iterator(path))
			{
				if (filesystem::is_regular_file(entry.path()) && entry.path().extension() == enum_to_string(type))
				{
					auto filename = entry.path().filename().generic_string();
					files.push_back(filename);
				}
			}
		}
	}
	catch (const filesystem::filesystem_error& e)
	{
		cerr << "ERROR::FILELOADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << endl;
	}

	return files;
}

list<pair<string, string>> FileLoader::get_files_by_type_with_path(const filesystem::path& path, FileType type)
{
	list<pair<string, string>> files_list;
	try
	{
		if (filesystem::exists(path) && filesystem::is_directory(path))
		{
			for (const auto& entry : filesystem::recursive_directory_iterator(path))
			{
				if (filesystem::is_regular_file(entry.path()) && entry.path().extension() == enum_to_string(type))
				{
					auto path = entry.path();
					auto filepath = path.generic_string();
					auto filename = path.filename().generic_string();
					files_list.push_back(make_pair(filename, filepath));
				}
			}
		}
	}
	catch (const filesystem::filesystem_error& e)
	{
		cerr << "ERROR::FILELOADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << endl;
	}

	return files_list;
}

std::string FileLoader::get_filename_from_path(const std::filesystem::path& path)
{
	return path.filename().generic_string();
}

std::string FileLoader::get_filename_from_path(const std::string path)
{
	return filesystem::absolute(path).filename().generic_string();
}

string FileLoader::get_name_from_filename(const string filename)
{
	if (filename.empty())
	{
		cout << "Could not get name from file. No filename was given" << endl;
		return filename;
	}
	string name = filename.substr(0, filename.find_last_of('.'));
	transform(name.begin(), name.end(), name.begin(), ::tolower);
	name[0] = std::toupper(name[0]);

	return name;
}

string FileLoader::enum_to_string(FileType type)
{
	switch (type)
	{
		case png: return ".png";
		case jpg: return ".jpg";
		case vs: return ".vs";
		case fs: return ".fs";
		case obj: return ".obj";
		case mtl: return ".mtl";
		case fbx: return ".fbx";
		default: return ".obj";
	}
}

vector<char*> FileLoader::get_vector_items_to_array(const list<pair<string, string>> list_items)
{
	vector<string> vector_items;
	for (const auto& item : list_items)
	{
		vector_items.push_back(item.first);
	}

	vector<char*> c_str_items;
	c_str_items.reserve(vector_items.size());

	for (const auto& item : vector_items)
	{
		char* c_str = new char[item.size() + 1]; // +1 for null terminator
		strcpy_s(c_str, item.size() + 1, item.c_str());
		c_str_items.push_back(c_str);
	}

	return c_str_items;
}