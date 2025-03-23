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

#include "mesh.h"
#include "camera.h"
#include "light.h"

#include <string>
#include <vector>
#include <set>

class Model : public Asset
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
	Model(const std::string& path, const std::string& file_name, int index);
	~Model() override;

	void draw() const; // draws the model, and thus all its meshes
	void set_camera_and_light(Camera& camera, Light& light);
	void set_current_material(const std::string& material_name);
	void set_visible(bool visible) { m_is_visible = visible; }
	bool is_visible() const { return m_is_visible; }
private:
	// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	void load_model(std::string const& path);
	// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void process_node(aiNode* node, const aiScene* scene);
	Mesh* process_mesh(aiMesh* mesh, const aiScene* scene);
	// checks all material textures of a given type and loads the textures if they're not loaded yet.
	// the required info is returned as a Texture struct.
	std::vector<std::string> load_material_textures(aiMaterial* mat, aiTextureType type);
	void update_light_properties() const;
	void update_material_properties() const;
	void update_breakdown_shader() const;
	
	Camera* m_camera;
	Light* m_light;
	Material* m_current_material;

	bool m_is_visible;
};

#endif