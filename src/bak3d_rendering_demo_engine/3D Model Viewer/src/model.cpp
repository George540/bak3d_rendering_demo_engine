#include <assimp/postprocess.h>
#define STB_IMAGE_IMPLEMENTATION    
#include "Tools/stb_image.h"

#include <iostream>
#include <ostream>
#include <filesystem>
#include <glm/ext/matrix_transform.hpp>

#include "model.h"
#include "event_manager.h"
#include "renderer.h"

Model::Model(string const& path, Camera& cam, Light& light, int index) : m_camera(&cam), m_light(&light), m_combo_index(index)
{
	auto shader = new Shader(std::filesystem::absolute("shaders/ModelShader.vs").string().c_str(),
							 std::filesystem::absolute("shaders/ModelShader.fs").string().c_str());
	shader->set_index(0);
	m_shaders.push_back(shader);
	shader = nullptr;
	shader = new Shader(
		std::filesystem::absolute("shaders/DissectShader.vs").string().c_str(),
		std::filesystem::absolute("shaders/DissectShader.fs").string().c_str());
	shader->set_index(1);
	m_shaders.push_back(shader);
	load_model(path);

	std::cout << "Number of shaders compiled: " << m_shaders.size() << std::endl;

	if (m_shaders.empty())
	{
		throw std::runtime_error("ERROR::SHADER_COMPILATION_ERROR: No shaders loaded!");
	}

	set_current_shader(0);
}

Model::~Model()
{
	// Delete and free shaders in memory
	for (const auto s : m_shaders)
	{
		delete s;
	}
	m_shaders.clear();

	// Free texture data
	textures_loaded.clear();
	cout << "Texture data of model " << m_name << " have been cleared" << endl;
	
	// Safely dereference camera and light addresses from pointers
	m_camera = nullptr;
	m_light = nullptr;
	
	// Delete all mesh VAO data
	delete_mesh_vaos();
	meshes.clear();
	cout << "Model " << m_name <<" mesh data have been safely deleted" << endl;
}

void Model::draw() const
{
	for (auto& mesh : meshes)
	{
		m_current_shader->use();

		// Set MVP matrix
		glm::mat4 model = glm::mat4(1.0f);
		m_current_shader->set_mat4("projection", m_camera->get_projection_matrix());
		m_current_shader->set_mat4("view", m_camera->get_view_matrix());
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
		m_current_shader->set_mat4("model", model);

		if (m_current_shader->get_index() == 0
			&& Renderer::is_full_render_selected
			&& !Renderer::is_diffuse_render_selected
			&& !Renderer::is_specular_selected
			&& !Renderer::is_normal_map_selected)
		{
			update_light_properties();
			update_material_properties(mesh);
		}
		else
		{
			update_breakdown_shader();
		}

		mesh.draw(*m_current_shader);
	}
}

void Model::update_light_properties() const
{
	const auto light = m_light->get_light_properties();

	// VERTEX
	m_current_shader->set_vec3("viewPos", m_camera->get_camera_position());
	m_current_shader->set_vec3("lightPos", light.position);

	// FRAGMENT
	m_current_shader->set_vec3("light.position", light.position);
	m_current_shader->set_vec3("light.diffuse", light.diffuse);
	m_current_shader->set_vec3("light.specular", light.specular);
	m_current_shader->set_vec3("light.ambient", light.ambient);
	m_current_shader->set_float("light.intensity", light.intensity);
}

void Model::update_material_properties(const Mesh& mesh) const
{
	// FRAGMENT MATERIAL
	m_current_shader->set_int("material.diffuse", 0);
	m_current_shader->set_int("material.specular", 1);
	m_current_shader->set_int("material.normal", 2);
	m_current_shader->set_float("material.ambient", 0.5f);
	m_current_shader->set_float("material.shininess", Renderer::shininess);
	m_current_shader->set_bool("materialSettings.useDiffuseTexture", EventManager::get_using_diffuse_texture());
	m_current_shader->set_bool("materialSettings.useSpecularTexture", EventManager::get_using_specular_texture());
	m_current_shader->set_bool("materialSettings.useNormalsTexture", EventManager::get_using_normal_maps());
	m_current_shader->set_bool("material.gamma", Renderer::is_gamma_enabled);
}

void Model::update_breakdown_shader() const
{
	int texture_id = 0;
	if (Renderer::is_diffuse_render_selected)
	{
		texture_id = 0;
	}
	else if (Renderer::is_specular_selected)
	{
		texture_id = 1;
	}
	else if (Renderer::is_normal_map_selected)
	{
		texture_id = 2;
	}
	else
	{
		texture_id = 0;
	}
	m_current_shader->set_int("textureSampler", texture_id);
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
	directory = path.substr(0, path.find_last_of('/'));
	m_name = path.substr(path.find_last_of('/') + 1, path.size());
	std::cout << "Loading model with name: " << m_name << std::endl;

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

		meshes.push_back(process_mesh(mesh, scene));
	}

	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		process_node(node->mChildren[i], scene);
	}
}

Mesh Model::process_mesh(aiMesh* mesh, const aiScene* scene)
{
	// data to fill
	vector<vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture2D> textures;

	// walk through each of the mesh's vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		vertex vertex{};
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

			vertex.useDiffuseTexture = mat->GetTextureCount(aiTextureType_DIFFUSE) > 0 ? true : false;
			EventManager::is_using_diffuse_texture = vertex.useDiffuseTexture;

			// Check if specular texture exists
			if (AI_SUCCESS == aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, &specular))
			{
				vertex.color = glm::vec4(specular.r, specular.g, specular.b, specular.a);
			}

			vertex.useSpecularTexture = mat->GetTextureCount(aiTextureType_SPECULAR) > 0 ? true : false;
			EventManager::is_using_specular_texture = vertex.useSpecularTexture;

			// Check if normals texture exists
			vertex.useNormalsTexture = mat->GetTextureCount(aiTextureType_HEIGHT) > 0 ? true : false;
			EventManager::is_using_normals_texture = vertex.useNormalsTexture;
		}

		vertices.push_back(vertex);
	}

	// now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		auto face = mesh->mFaces[i];
		// If the face is made up of three vertices only, it's a triangle
		m_num_triangles += (face.mNumIndices == 3) ? 1 : 0;

		// retrieve all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < face.mNumIndices; ++j)
		{
			indices.push_back(face.mIndices[j]);
			GLuint v1 = face.mIndices[j];
			GLuint v2 = face.mIndices[(j + 1) % face.mNumIndices];
			m_unique_edges.insert(edge(v1, v2));
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
	return Mesh(vertices, indices, textures);
}

vector<Texture2D> Model::load_material_textures(aiMaterial* mat, aiTextureType type)
{
	vector<Texture2D> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i)
	{
		aiString filename;
		mat->GetTexture(type, i, &filename);
		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (auto& text_loaded : textures_loaded)
		{
			if (std::strcmp(text_loaded.get_file_name().data(), filename.C_Str()) == 0)
			{
				textures.push_back(text_loaded);
				skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{   // if texture hasn't been loaded already, load it
			string path = directory + '/' + filename.C_Str();
			Texture2D texture = Texture2D(path, type);
			textures.push_back(texture);
			textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
		}
	}
	return textures;
}

void Model::delete_mesh_vaos() const
{
	for (auto& mesh : meshes)
	{
		mesh.delete_vao_vbo();
	}
}
