// =====================================================================================
// This mesh class is referenced from https://learnopengl.com by Joey de Vries
// Only changes made were attribute naming format and more modern C++ syntax
// Joe de Vries on twitter: https://twitter.com/JoeyDeVriez
// =====================================================================================

#ifndef MODEL_H
#define MODEL_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "mesh.h"
#include "camera.h"
#include "light.h"

#include <string>
#include <vector>

unsigned int texture_from_file(const char* path, const string& directory);

class Model
{
public:
    // model data
    std::vector<texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    std::vector<Mesh> meshes; // a model is made out of one or more meshes
    std::string directory;
    std::string m_name;
    bool gamma_correction{};

    // constructor, expects a filepath to a 3D model.
    Model(string const& path, Camera& cam, Light& light);
    ~Model();

    void draw() const; // draws the model, and thus all its meshes
    void set_current_shader(int id) { m_current_shader = m_shaders.at(id); }
    void delete_mesh_vaos() const;
private:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void load_model(string const& path);
    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void process_node(aiNode* node, const aiScene* scene);
    Mesh process_mesh(aiMesh* mesh, const aiScene* scene);
    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    vector<texture> load_material_textures(aiMaterial* mat, aiTextureType type, string typeName);
    void update_light_properties() const;
    void update_material_properties(const Mesh& mesh) const;
    void update_breakdown_shader() const;

	vector<Shader*> m_shaders;
    Shader* m_current_shader{};
    Camera* m_camera;
    Light* m_light;
};

#endif