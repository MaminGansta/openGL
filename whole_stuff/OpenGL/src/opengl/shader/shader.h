#pragma once

#include <string>
#include <cstdint>
#include <unordered_map>
#include "gl_assert/gl_assert.h"
#include "glm.hpp"

namespace gl
{
    struct Shader
    {
        uint32_t id;
        std::unordered_map<std::string, int> uniforms;
        bool invalid = false;

    private:
        bool parse_shaders(char const* filepath, std::string& vertex, std::string& fragment, std::string& geometry);
        bool compile_shaders(const std::string& vertexSource, const std::string& fragmentSource, const std::string& geometry);
        int getUni(const std::string& name);

    public:

        Shader(const char* filepath);

        void Use();

        // lone int 
        void setUni1i(const std::string& name, const int& val);

        // float vec
        void setUni1f(const std::string& name, const float& val);
        void setUni2f(const std::string& name, const glm::vec2& val);
        void setUni3f(const std::string& name, const glm::vec3& val);
        void setUni4f(const std::string& name, const glm::vec4& val);

        // float matrices
        void setUniMat3(const std::string& name, const glm::mat3& val);
        void setUniMat4(const std::string& name, const glm::mat4& val);

    };
}