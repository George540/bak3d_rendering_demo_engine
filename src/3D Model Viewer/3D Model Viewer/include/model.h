#ifndef MODEL_H
#define MODEL_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "mesh.h"

#include <string>
#include <vector>

class Model
{
public:
    // model data 
    std::vector<texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    std::vector<Mesh> meshes; // a model is made out of one or more meshes
    std::string directory;
    bool gamma_correction;

    // constructor, expects a filepath to a 3D model.
    Model(string const& path, bool gamma = false);

    void draw(Shader& shader); // draws the model, and thus all its meshes
private:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void load_model(string const& path);
    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void process_node(aiNode* node, const aiScene* scene);
    Mesh process_mesh(aiMesh* mesh, const aiScene* scene);
    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    vector<texture> load_material_textures(aiMaterial* mat, aiTextureType type, string typeName);
};

#endif
