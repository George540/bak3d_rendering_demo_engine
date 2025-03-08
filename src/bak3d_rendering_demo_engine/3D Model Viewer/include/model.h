// =====================================================================================
// This model class is referenced from https://learnopengl.com by Joey de Vries
// Only changes made were attribute naming format and more modern C++ syntax
// Joe de Vries on twitter: https://twitter.com/JoeyDeVriez
// =====================================================================================

#pragma once

#ifndef MODEL_H
#define MODEL_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "texture.h"
#include "mesh.h"
#include "camera.h"
#include "light.h"

#include <string>
#include <vector>
#include <set>

class Model
{
public:
	// model data
	std::vector<std::string> textures_loaded; // stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	std::vector<Mesh*> meshes; // a model is made out of one or more meshes
	int m_combo_index;
	bool gamma_correction{};

	// model stats
	int m_num_vertices;
	std::set<Edge> m_unique_edges; // with a set, an edge is ensured to be stored only once
	int m_num_faces;
	int m_num_triangles;

	// constructor, expects a filepath to a 3D model.

	Model() = default;
	Model(const string& path, const string& file_name, const string& model_name, int index);
	~Model();

	void draw() const; // draws the model, and thus all its meshes
	void set_toggle_shaders(Shader* model_shader, Shader* dissect_shader) { m_toggle_shaders[0] = model_shader; m_toggle_shaders[1] = dissect_shader; }
	void set_current_toggle_shader(GLuint shader_index) { m_current_shader_index = shader_index; }

	std::string get_directory() const { return m_directory; }
	std::string get_file_name() const { return m_file_name; }
	std::string get_model_name() const { return m_model_name; }
private:
	// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	void load_model(string const& path);
	// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void process_node(aiNode* node, const aiScene* scene);
	Mesh* process_mesh(aiMesh* mesh, const aiScene* scene);
	// checks all material textures of a given type and loads the textures if they're not loaded yet.
	// the required info is returned as a Texture struct.
	vector<std::string> load_material_textures(aiMaterial* mat, aiTextureType type);
	void update_light_properties() const;
	void update_material_properties() const;
	void update_breakdown_shader() const;

	Shader* m_toggle_shaders[2];
	GLuint m_current_shader_index;
	Camera* m_camera;
	Light* m_light;

	std::string m_directory;
	std::string m_file_name;
	std::string m_model_name;
};

#endif