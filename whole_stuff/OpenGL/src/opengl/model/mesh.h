#pragma once

#include "shader/shader.h"


namespace gl
{
    struct Vertex
    {
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

    enum class TextureType { DIFFUSE, SPECULAR, NORMAL, HEIGHT };

    struct Texture
    {
        unsigned int id;
        TextureType type;
    };

    class Mesh
    {
    public:
        /*  Mesh Data  */
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        std::vector<Texture> textures;
        uint32_t VAO;
        int shininess = 32;

        /*  Functions  */
        // constructor
        Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<Texture> textures);

        Mesh(const Mesh&) = default;
        Mesh& operator= (const Mesh&) = default;

        Mesh(Mesh&&) = default;
        Mesh& operator= (Mesh&&) = default;

        // render the mesh
        void Draw(Shader& shader);


    private:
        /*  Render data  */
        uint32_t VBO, EBO;

        /*  Functions    */
        // initializes all the buffer objects/arrays
        void SetupMesh();
    };
}