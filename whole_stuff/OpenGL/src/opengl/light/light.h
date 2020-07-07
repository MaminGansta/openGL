#pragma once

#include "shader/shader.h"
#include "glm.hpp"
#include <vector>

enum class LightType
{
    DirLight,
    PointLight,
    SpotLight
};

namespace gl
{
    struct Light
    {
        LightType type;

        // dir
        glm::vec3 direction;
        glm::vec3 position;

        // point
        float constant = 1.0f;
        float linear = 0.045f;
        float quadratic = 0.0075f;

        // spot
        float cutOff = glm::cos(glm::radians(12.5f));
        float outerCutOff = glm::cos(glm::radians(17.5f));

        glm::vec3 ambient = glm::vec3(0.1f);
        glm::vec3 diffuse = glm::vec3(0.5f);
        glm::vec3 specular = glm::vec3(0.8f);
        int shininess = 32;
    };


    inline Light CreateDirLight(const glm::vec3& dir)
        //const glm::vec3& ambient, 
        //const glm::vec3& diffuse, 
        //const glm::vec3& specular)
    {
        Light out;
        out.type = LightType::DirLight;
        out.direction = dir;
        return out;
    }

    inline Light CreatePointLihgt(const glm::vec3& pos)
    {
        Light out;
        out.type = LightType::PointLight;
        out.position = pos;
        return out;
    }

    inline Light CreateSpotLight(const glm::vec3& pos, const glm::vec3 dir)
    {
        Light out;
        out.type = LightType::SpotLight;
        out.position = pos;
        out.direction = dir;
        return out;
    }


    inline void ApplyLightToShader(const std::vector<Light>& lighters, Shader& shader)
    {
        shader.Use();
        char buffer[255];
        for (int i = 0; i < lighters.size(); i++)
        {
            const Light& lighter = lighters[i];
            
            sprintf_s(buffer, "lights[%d]", i);
            std::string lighter_id(buffer);

            shader.setUni1i(lighter_id + ".type", (int)lighter.type);
            shader.setUni3f(lighter_id + ".direction", lighter.direction);
            shader.setUni3f(lighter_id + ".position", lighter.position);

            shader.setUni1f(lighter_id + ".constant", lighter.constant);
            shader.setUni1f(lighter_id + ".linear", lighter.linear);
            shader.setUni1f(lighter_id + ".quadratic", lighter.quadratic);
            
            shader.setUni1f(lighter_id + ".cutOff", lighter.cutOff);
            shader.setUni1f(lighter_id + ".outerCutOff", lighter.outerCutOff);

            shader.setUni3f(lighter_id + ".ambient", lighter.ambient);
            shader.setUni3f(lighter_id + ".diffuse", lighter.diffuse);
            shader.setUni3f(lighter_id + ".specular", lighter.specular);
        }
        shader.setUni1i("nLights", lighters.size());
    }

}