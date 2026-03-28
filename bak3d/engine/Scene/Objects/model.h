/* ===========================================================================
The MIT License (MIT)

Copyright (c) 2022-2026 George Mavroeidis - GeoGraphics

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
=========================================================================== */

#pragma once

#ifndef MODEL_H
#define MODEL_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "camera.h"
#include "light.h"
#include "Loader/asset.h"
#include "Renderer/mesh.h"

#include <string>
#include <vector>
#include <set>

/*
 * An asset class that stores information about a model loaded from a file using assimp.
 */
class Model : public Asset
{
public:
	// model data
	std::unordered_map<aiTextureType, Texture2D*> textures_cache;
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

	bool has_texture_of_type(const aiTextureType& texture_type) const { return textures_cache.contains(texture_type); }
private:
	// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	void load_model(std::string const& path);
	// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void process_node(aiNode* node, const aiScene* scene);
	Mesh* process_mesh(aiMesh* mesh, const aiScene* scene);

	void load_material_textures(aiMaterial* mat, aiTextureType type);
	void update_light_properties() const;
	void update_material_properties() const;
	void update_breakdown_shader() const;
	
	Camera* m_camera;
	Light* m_light;
	Material* m_current_material;

	bool m_is_visible;
};

#endif