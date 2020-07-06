#pragma once

#include "assimp/scene.h"

#include "mesh.h"
#include "shader/shader.h"

#include <vector>
#include "string"


namespace gl
{
    unsigned int TextureFromFile(const char* path, const std::string& directory);

    class Model
    {
    public:
        /*  Model Data */
        std::vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
        std::vector<Mesh> meshes;
        std::string directory;
        bool gammaCorrection;

        /*  Functions   */
        // constructor, expects a filepath to a 3D model.
        Model(std::string const& path, bool gamma = false);

        // draws the model, and thus all its meshes
        void Draw(Shader& shader);

    private:
        /*  Functions   */
        // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
        void LoadModel(std::string const& path);

        // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
        void ProcessNode(aiNode* node, const aiScene* scene);

        Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);

        // checks all material textures of a given type and loads the textures if they're not loaded yet.
        // the required info is returned as a Texture struct.
        void LoadMaterialTextures(std::vector<Texture>& textures, aiMaterial* mat, aiTextureType const& type, TextureType const& texture_type);
    };
}