
#include "pch.h"
#include "model.h"


namespace gl
{
    Model::Model(
        const Model& model, 
        const glm::vec3& pos,
        const glm::vec3& scale,
        const glm::vec3& rotation)
    {
        this->meshes = model.meshes;
        Position = pos;
        Scale = scale;
        Rotation = rotation;
    }

    // draws the model, and thus all its meshes
    void Model::Draw(Shader& shader)
    {
        glm::mat4 model(1.0f);
        model = glm::translate(model, Position);
        model = glm::scale(model, Scale);
        //model = glm::rotate(model, Rotation);
        shader.setUniMat4("model", model);

        for (unsigned int i = 0; i < meshes->size(); i++)
            (*meshes)[i].Draw(shader);
    }
    
}