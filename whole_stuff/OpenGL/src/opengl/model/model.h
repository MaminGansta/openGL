#pragma once

#include <vector>
#include <string>
#include <memory>

#include "glm.hpp"
#include "mesh.h"
#include "shader/shader.h"


namespace gl
{
    class Model
    {
    public:
        std::shared_ptr<std::vector<Mesh>> meshes;

        glm::vec3 Position;
        glm::vec3 Scale;
        glm::vec3 Rotation;

        Model() = default;

        Model(const Model&) = default;
        Model& operator= (const Model&) = default;

        Model(const Model& model,
              const glm::vec3& pos,
              const glm::vec3& scale = glm::vec3(1.0f),
              const glm::vec3& rotation = glm::vec3(0.0f));

        //Model(Model&&) = default;
        //Model& operator= (Model&&) = default;

        inline bool valid() { return meshes != nullptr; }
        
        // draws the model, and thus all its meshes
        void Draw(Shader& shader);
    };
}