
#include "model_loader.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif 
#include "stb_image.h"


namespace gl
{
    static uint32_t TextureFromFile(const char* path, const std::string& directory);
    static uint32_t CreateTexturePlug();

    std::vector<std::pair<std::string, Model*>>  ModelLoader::stored_paths;
    std::vector<std::pair<std::string, Texture>> ModelLoader::loaded_textures;


    Model ModelLoader::LoadModel(std::string const& path, const glm::vec3& pos, const glm::vec3& scale, const glm::vec3& rotation)
    {
        Model model;
        model.Position = pos;
        model.Scale = scale;
        model.Rotation = rotation;

        for (auto& stored_path : stored_paths)
        {
            if (stored_path.first == path)
            {
                model.meshes = stored_path.second->meshes;
                return model;
            }
        }
        stored_paths.push_back(std::make_pair(path, &model));

        // read file via ASSIMP
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenNormals);
        // check for errors
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            printf("ERROR::ASSIMP:: %s \n", importer.GetErrorString());
            return model;
        }
        
        model.meshes = std::make_shared<std::vector<Mesh>>();
        model.meshes->reserve(scene->mNumMeshes);
        
        // process ASSIMP's root node recursively
        ProcessNode(model, scene->mRootNode, scene);
        return model;
    }

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void ModelLoader::ProcessNode(Model& model, aiNode* node, const aiScene* scene)
    {
        // process each mesh located at the current node
        for (int i = 0; i < node->mNumMeshes; i++)
        {
            // the node object only contains indices to index the actual objects in the scene. 
            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            model.meshes->push_back(ProcessMesh(mesh, scene));
        }

        // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for (int i = 0; i < node->mNumChildren; i++)
            ProcessNode(model, node->mChildren[i], scene);
    }

    Mesh ModelLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene)
    {
        // data to fill
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        std::vector<Texture> textures;

        vertices.reserve(mesh->mNumVertices);
        indices.reserve(mesh->mNumFaces);
        textures.reserve(5);

        // Walk through each of the mesh's vertices
        for (int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            // positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            // normals
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
            // texture coordinates
            if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
            {
                glm::vec2 vec;
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            // tangent
            if (mesh->mTangents)
            {
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = vector;
                // bitangent
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.Bitangent = vector;
            }

            vertices.push_back(vertex);
        }

        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for (int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for (int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        // process materials
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
        // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
        // Same applies to other texture as the following list summarizes:
        // diffuse: texture_diffuseN
        // specular: texture_specularN
        // normal: texture_normalN

        // 1. diffuse maps
        LoadMaterialTextures(textures, material, aiTextureType_DIFFUSE, TextureType::DIFFUSE); // "texture_diffuse");
        // 2. specular maps
        LoadMaterialTextures(textures, material, aiTextureType_SPECULAR, TextureType::SPECULAR);  // "texture_specular");
        // 3. normal maps
        LoadMaterialTextures(textures, material, aiTextureType_HEIGHT, TextureType::NORMAL);//"texture_normal");
        // 4. height maps
        LoadMaterialTextures(textures, material, aiTextureType_AMBIENT, TextureType::HEIGHT);//"texture_height");

        // return a mesh object created from the extracted mesh data
        return Mesh(vertices, indices, textures);
    }

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    void ModelLoader::LoadMaterialTextures(std::vector<Texture>& textures, aiMaterial* mat, aiTextureType type, TextureType texture_type)
    {
        // retrieve the directory path of the filepath
        const std::string& path = stored_paths.back().first;
        std::string directory = path.substr(0, path.find_last_of('/'));


        for (int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            bool skip = false;
            for (int j = 0; j < loaded_textures.size(); j++)
            {
                if (std::strcmp(loaded_textures[j].first.data(), str.C_Str()) == 0)
                {
                    textures.push_back(loaded_textures[j].second);
                    skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                    break;
                }
            }
            if (!skip)
            {   // if texture hasn't been loaded already, load it

                //stbi_set_flip_vertically_on_load(type == NORMAL);

                Texture texture;
                texture.id = TextureFromFile(str.C_Str(), directory);
                texture.type = texture_type;
                textures.push_back(texture);
                ModelLoader::loaded_textures.push_back(std::make_pair(str.C_Str(), texture));  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
            }
        }

        // create white 1x1 texture
        if (mat->GetTextureCount(type) == 0 && 
            (texture_type == TextureType::DIFFUSE || texture_type == TextureType::SPECULAR))
        {
            static int white_texture_plug = CreateTexturePlug();
            Texture texture;
            texture.id = white_texture_plug;
            texture.type = texture_type;
            textures.push_back(texture);
        }
    }


    static uint32_t TextureFromFile(const char* path, const std::string& directory)
    {
        std::string filename(path);
        filename = directory + '/' + filename;

        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else
        {
            printf("Texture failed to load at path: %s\n", path);
            stbi_image_free(data);
        }

        return textureID;
    }



    static uint32_t CreateTexturePlug()
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);

        uint8_t color[] = { 230, 230, 230, 255 };

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, new uint32_t(*((uint32_t*)color)));
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        return textureID;
    }
}