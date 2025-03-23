#include <assimp/postprocess.h>
#define STB_IMAGE_IMPLEMENTATION    
#include "Tools/stb_image.h"

#include <iostream>
#include <ostream>
#include <filesystem>

#include "model.h"
#include "event_manager.h"
#include "file_loader.h"
#include "resource_manager.h"
#include "user_interface.h"

using namespace std;

Model::Model(const string& path, const std::string& file_name, int index) :
	m_combo_index(index),
	m_camera(nullptr),
	m_light(nullptr)
{
	m_path = path;
	m_directory = m_path.substr(0, m_path.find_last_of('/'));
	m_file_name = file_name;
	m_asset_name = m_file_name.substr(0, m_file_name.find('.'));
	
	m_toggle_shaders[0] = new Shader(*ResourceManager::get_shader("ModelShader"));
	m_toggle_shaders[1] = new Shader(*ResourceManager::get_shader("DissectShader"));
	load_model(path);
}

Model::~Model()
{
	// Free texture data
	textures_loaded.clear();
	cout << "Texture data of model " << m_file_name << " have been cleared" << '\n';
	
	// Safely dereference camera and light addresses from pointers
	m_camera = nullptr;
	m_light = nullptr;
	
	meshes.clear();
	cout << "Model " << m_file_name <<" mesh data have been safely deleted" << '\n';
}

void Model::set_camera_and_light(Camera& camera, Light& light)
{
	m_camera = &camera;
	m_light = &light;
	for (auto mesh : meshes)
	{
		mesh->set_camera(camera);
	}
}

void Model::draw() const
{
	if (!m_is_visible || !m_camera || !m_light) return;
	
	for (auto& mesh : meshes)
	{
		if (m_toggle_shaders[m_current_shader_index] != mesh->get_current_shader())
		{
			mesh->set_current_shader(m_toggle_shaders[m_current_shader_index]);
		}

		if (m_current_shader_index == 0
			&& UserInterface::is_full_render_selected
			&& !UserInterface::is_diffuse_render_selected
			&& !UserInterface::is_specular_selected
			&& !UserInterface::is_normal_map_selected)
		{
			update_light_properties();
			update_material_properties();
		}
		else
		{
			update_breakdown_shader();
		}

		mesh->draw();
	}
}

void Model::update_light_properties() const
{
	if (!m_light || !m_camera) return;
	
	const auto light = m_light->get_light_properties();

	auto current_shader = m_toggle_shaders[m_current_shader_index];

	// VERTEX
	current_shader->set_vec3("viewPos", m_camera->get_camera_position());
	current_shader->set_vec3("lightPos", light.position);

	// FRAGMENT
	current_shader->set_vec3("light.position", light.position);
	current_shader->set_vec3("light.diffuse", light.diffuse);
	current_shader->set_vec3("light.specular", light.specular);
	current_shader->set_vec3("light.ambient", light.ambient);
	current_shader->set_float("light.intensity", light.intensity);
}

void Model::update_material_properties() const
{
	auto current_shader = m_toggle_shaders[m_current_shader_index];

	// FRAGMENT MATERIAL
	current_shader->set_float("material.ambient", 0.5f);
	current_shader->set_float("material.shininess", UserInterface::shininess);
	current_shader->set_bool("materialSettings.useDiffuseTexture", EventManager::get_using_diffuse_texture());
	current_shader->set_bool("materialSettings.useSpecularTexture", EventManager::get_using_specular_texture());
	current_shader->set_bool("materialSettings.useNormalsTexture", EventManager::get_using_normal_maps());
	current_shader->set_bool("material.gamma", UserInterface::is_gamma_enabled);
}

void Model::update_breakdown_shader() const
{
	int texture_id;
	if (UserInterface::is_diffuse_render_selected)
	{
		texture_id = ResourceManager::get_texture(textures_loaded[0])->get_asset_id();
	}
	else if (UserInterface::is_specular_selected)
	{
		texture_id = ResourceManager::get_texture(textures_loaded[1])->get_asset_id();
	}
	else if (UserInterface::is_normal_map_selected)
	{
		texture_id = ResourceManager::get_texture(textures_loaded[2])->get_asset_id();;
	}
	else
	{
		texture_id = ResourceManager::get_texture(textures_loaded[0])->get_asset_id();
	}
	m_toggle_shaders[m_current_shader_index]->set_int("textureSampler", texture_id);
}

void Model::load_model(string const& path)
{
	// read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) //if it's not zero
	{
		cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
		return;
	}

	// retrieve the directory path of the filepath
	std::cout << "Loading model with name: " << m_file_name << std::endl;

	// process ASSIMP's root node recursively
	process_node(scene->mRootNode, scene);
}

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void Model::process_node(aiNode* node, const aiScene* scene)
{
	// process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		// the node object only contains indices to index the actual objects in the scene.
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh * mesh = scene->mMeshes[node->mMeshes[i]];

		m_num_vertices += mesh->mNumVertices;
		m_num_faces += mesh->mNumFaces;

		auto mesh_object = process_mesh(mesh, scene);
		mesh_object->set_current_shader(m_toggle_shaders[0]);
		meshes.push_back(mesh_object);
	}

	// after we've processed all the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		process_node(node->mChildren[i], scene);
	}
}

Mesh* Model::process_mesh(aiMesh* mesh, const aiScene* scene)
{
	// data to fill
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<string> textures;

	// walk through each of the mesh's vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex{};
		glm::vec3 vector; // temp glm::vec3 vector for parsing data to Assimp using the vertex payload

		// positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.position = vector;

		// normals
		if (mesh->HasNormals())
		{
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.normal = vector;
		}

		// texture coordinates
		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			glm::vec2 vec;
			
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.tex_coords = vec;

			// tangent
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.tangent = vector;

			// bitangent
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.bitangent = vector;
		}
		else
		{
			vertex.tex_coords = glm::vec2(0.0f, 0.0f);
		}

		if (scene->mNumMaterials > mesh->mMaterialIndex)
		{
			const auto& mat = scene->mMaterials[mesh->mMaterialIndex];
			aiColor4D diffuse;
			aiColor4D specular;
			aiColor4D normals;

			// Check if diffuse texture exists
			if (AI_SUCCESS == aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &diffuse))
			{
				vertex.color = glm::vec4(diffuse.r, diffuse.g, diffuse.b, diffuse.a);
			}
			else
			{
				vertex.color = glm::vec4(1.0f, 0.75f, 0.796f, 1.0f);
			}

			//vertex.useDiffuseTexture = mat->GetTextureCount(aiTextureType_DIFFUSE) > 0;
			//EventManager::is_using_diffuse_texture = vertex.useDiffuseTexture;

			// Check if specular texture exists
			if (AI_SUCCESS == aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, &specular))
			{
				vertex.color = glm::vec4(specular.r, specular.g, specular.b, specular.a);
			}

			vertex.useSpecularTexture = mat->GetTextureCount(aiTextureType_SPECULAR) > 0;
			EventManager::is_using_specular_texture = vertex.useSpecularTexture;

			// Check if normals texture exists
			vertex.useNormalsTexture = mat->GetTextureCount(aiTextureType_HEIGHT) > 0;
			EventManager::is_using_normals_texture = vertex.useNormalsTexture;
		}

		vertices.push_back(vertex);
	}

	// now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		auto face = mesh->mFaces[i];
		// If the face is made up of three vertices only, it's a triangle
		m_num_triangles += face.mNumIndices == 3 ? 1 : 0;

		// retrieve all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < face.mNumIndices; ++j)
		{
			indices.push_back(face.mIndices[j]);
			GLuint v1 = face.mIndices[j];
			GLuint v2 = face.mIndices[(j + 1) % face.mNumIndices];
			m_unique_edges.insert(Edge(v1, v2));
		}
	}

	// process materials
	auto material = scene->mMaterials[mesh->mMaterialIndex];
	// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
	// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
	// Same applies to other texture as the following list summarizes:
	// diffuse: texture_diffuseN
	// specular: texture_specularN
	// normal: texture_normalN

	// 1. diffuse maps
	auto diffuse_maps = load_material_textures(material, aiTextureType_DIFFUSE);
	textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());
	// 2. specular maps
	auto specular_maps = load_material_textures(material, aiTextureType_SPECULAR);
	textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());
	// 3. normal maps
	auto normal_maps = load_material_textures(material, aiTextureType_HEIGHT);
	textures.insert(textures.end(), normal_maps.begin(), normal_maps.end());
	// 4. height maps
	auto height_maps = load_material_textures(material, aiTextureType_AMBIENT);
	textures.insert(textures.end(), height_maps.begin(), height_maps.end());

	// return a mesh object created from the extracted mesh data
	return new Mesh(vertices, indices, textures);
}

vector<string> Model::load_material_textures(aiMaterial* mat, aiTextureType type)
{
	vector<string> textures;
	int texture_index = 0;
	
	for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i)
	{
		aiString filename;
		mat->GetTexture(type, i, &filename);
		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (auto& text_loaded : textures_loaded)
		{
			auto file_model_name = string(filename.C_Str());
			file_model_name = file_model_name.substr(0, file_model_name.find('.'));
			file_model_name = format("{}.{}",m_asset_name, file_model_name);
			if (std::strcmp(text_loaded.c_str(), file_model_name.c_str()) == 0)
			{
				textures.push_back(text_loaded);
				skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{   // if texture hasn't been loaded already, load it
			string path = m_directory + '/' + filename.C_Str();
			string texture_file_name = string(filename.C_Str());
			Texture2D* texture = new Texture2D(path, texture_file_name, type, TextureUseType::Model);
			texture->set_texture_model_index(texture_index++);
			auto texture_name = texture_file_name.substr(0, texture_file_name.find('.'));
			auto texture_key = format("{}.{}",m_asset_name, texture_name);
			ResourceManager::add_texture(texture_key, texture);
			textures.push_back(texture_key);
			textures_loaded.push_back(texture_key); // store it as texture loaded for entire model, to ensure we won't unnecessarily load duplicate textures.
		}
	}
	return textures;
}
