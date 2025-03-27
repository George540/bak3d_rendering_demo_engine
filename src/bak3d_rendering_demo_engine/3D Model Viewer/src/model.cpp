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
	
	load_model(path);

	ResourceManager::add_material(m_asset_name + ".model", new Material(ResourceManager::get_shader("ModelShader")));
	ResourceManager::add_material(m_asset_name + ".dissect", new Material(ResourceManager::get_shader("DissectShader")));

	set_current_material(m_asset_name + ".model");
}

Model::~Model()
{
	// Free texture data
	textures_cache.clear();
	cout << "Texture data of model " << m_file_name << " have been cleared" << '\n';
	
	// Safely dereference camera and light addresses from pointers
	m_camera = nullptr;
	m_light = nullptr;
	m_current_material = nullptr;
	
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
	if (!m_is_visible || !m_camera || !m_light || !m_current_material) return;

	if (UserInterface::is_full_render_selected
			&& !UserInterface::is_diffuse_render_selected
			&& !UserInterface::is_specular_selected
			&& !UserInterface::is_normal_map_selected)
	{
		// UPDATE MODEL MATERIAL
		update_light_properties();
		update_material_properties();
	}
	else
	{
		// UPDATE DISSECT MATERIAL
		update_breakdown_shader();
	}
	
	for (auto& mesh : meshes)
	{
		mesh->draw();
	}
}

void Model::update_light_properties() const
{
	if (!m_light || !m_camera) return;
	
	const auto light = m_light->get_light_properties();

	// VERTEX
	m_current_material->set_vec3("viewPos", m_camera->get_camera_position());
	m_current_material->set_vec3("lightPos", light.position);

	// FRAGMENT
	m_current_material->set_vec3("light.position", light.position);
	m_current_material->set_vec3("light.diffuse", light.diffuse);
	m_current_material->set_vec3("light.specular", light.specular);
	m_current_material->set_vec3("light.ambient", light.ambient);
	m_current_material->set_float("light.intensity", light.intensity);
}

void Model::update_material_properties() const
{
	// FRAGMENT MATERIAL
	if (textures_cache.contains(aiTextureType_DIFFUSE))
	{
		Texture2D* diffuse_texture = textures_cache.at(aiTextureType_DIFFUSE);
		diffuse_texture->bind(0);
		m_current_material->set_int("material.diffuse", 0);
	}
	
	
	if (textures_cache.contains(aiTextureType_SPECULAR))
	{
		Texture2D* specular_texture = textures_cache.at(aiTextureType_SPECULAR);
		specular_texture->bind(1);
		m_current_material->set_int("material.specular", 1);
	}
	
	if (textures_cache.contains(aiTextureType_HEIGHT))
	{
		Texture2D* normal_texture = textures_cache.at(aiTextureType_HEIGHT);
		normal_texture->bind(2);
		m_current_material->set_int("material.normal", 2);
	}
	
	m_current_material->set_float("material.ambient", 0.5f);
	m_current_material->set_float("material.shininess", UserInterface::shininess);
	m_current_material->set_bool("materialSettings.useDiffuseTexture", EventManager::get_using_diffuse_texture());
	m_current_material->set_bool("materialSettings.useSpecularTexture", EventManager::get_using_specular_texture());
	m_current_material->set_bool("materialSettings.useNormalsTexture", EventManager::get_using_normal_maps());
	m_current_material->set_bool("material.gamma", UserInterface::is_gamma_enabled);
}

void Model::update_breakdown_shader() const
{
	aiTextureType texture_type = aiTextureType_DIFFUSE;
	if (UserInterface::is_diffuse_render_selected)
	{
		texture_type = aiTextureType_DIFFUSE;
	}
	else if (UserInterface::is_specular_selected)
	{
		texture_type = aiTextureType_SPECULAR;
	}
	else if (UserInterface::is_normal_map_selected)
	{
		texture_type = aiTextureType_HEIGHT;
	}

	m_current_material->set_int("textureSampler", 0);
	Texture2D* texture = textures_cache.contains(texture_type) ? textures_cache.at(texture_type) : ResourceManager::get_texture("None.jpg");
	texture->bind(0);
}

void Model::load_model(string const& path)
{
	// read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

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

			// Check if specular texture exists
			if (AI_SUCCESS == aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, &specular))
			{
				vertex.color = glm::vec4(specular.r, specular.g, specular.b, specular.a);
			}
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

	// 1. diffuse maps
	load_material_textures(material, aiTextureType_DIFFUSE);
	// 2. specular maps
	load_material_textures(material, aiTextureType_SPECULAR);
	// 3. normal maps
	load_material_textures(material, aiTextureType_HEIGHT);

	// return a mesh object created from the extracted mesh data
	return new Mesh(vertices, indices);
}

void Model::load_material_textures(aiMaterial* mat, aiTextureType type)
{
	//vector<string> textures;
	
	for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i)
	{
		aiString filename;
		mat->GetTexture(type, i, &filename);

		if (!textures_cache.contains(type)) // only one texture for each texture type at a time
		{
			string path = m_directory + '/' + filename.C_Str();
			string texture_file_name = string(filename.C_Str());
			Texture2D* texture = new Texture2D(path, texture_file_name, type, TextureUseType::Model);
			auto texture_name = texture_file_name.substr(0, texture_file_name.find('.'));
			auto texture_key = format("{}.{}",m_asset_name, texture_name);
			ResourceManager::add_texture(texture_key, texture);
			textures_cache[type] = texture;
		}
	}
}

void Model::set_current_material(const std::string& material_name)
{
	if (m_current_material != ResourceManager::get_material(material_name))
	{
		m_current_material = ResourceManager::get_material(material_name);
		for (const auto mesh : meshes)
		{
			mesh->set_material(m_current_material);
		}
	}
}
