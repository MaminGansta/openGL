#pragma once

#include "shader/shader.h"



namespace gl
{
    struct Vertex {
        // position
        glm::vec3 Position;
        // normal
        glm::vec3 Normal;
        // texCoords
        glm::vec2 TexCoords;
        // tangent
        glm::vec3 Tangent;
        // bitangent
        glm::vec3 Bitangent;
    };

    enum TextureType { DIFFUSE, SPECULAR, NORMAL, HEIGHT };

    struct Texture {
        unsigned int id;
        TextureType type;
        std::string path;
    };

    class Mesh {
    public:
        /*  Mesh Data  */
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;
        unsigned int VAO;

        /*  Functions  */
        // constructor
        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
        // render the mesh
        void Draw(Shader& shader);


    private:
        /*  Render data  */
        unsigned int VBO, EBO;

        /*  Functions    */
        // initializes all the buffer objects/arrays
        void SetupMesh();
    };
}