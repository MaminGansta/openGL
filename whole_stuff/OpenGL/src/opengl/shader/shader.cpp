
#include "pch.h"
#include "shader.h"


namespace gl
{
        bool Shader::parse_shaders(const std::string& path, std::string& vertex, std::string& fragment, std::string& geometry)
        {
            enum Type { NONE = -1, VERTEX = 0, FRAGMENT = 1, GEOMETRY = 2 } type = NONE;
            std::ifstream stream(path);

            if (!stream.is_open())
            {
                printf("file \"%s\" don't exist\n", path.c_str());
                return false;
            }

            std::string line;
            std::stringstream shaders[3];

            while (std::getline(stream, line))
            {
                if (line.find("#shader") != std::string::npos)
                {
                    if (line.find("vertex") != std::string::npos)
                        type = VERTEX;
                    else if (line.find("fragment") != std::string::npos)
                        type = FRAGMENT;
                    else if (line.find("geometry") != std::string::npos)
                        type = GEOMETRY;
                }
                else if (type == NONE)
                    continue;
                else
                    shaders[type] << line << '\n';
            }

            vertex = shaders[VERTEX].str();
            fragment = shaders[FRAGMENT].str();
            geometry = shaders[GEOMETRY].str();
            return true;
        }

        bool Shader::compile_shaders(const std::string& vertexSource, const std::string& fragmentSource, const std::string& geometry)
        {
            // compile shaders
            GLint vertexShader = glCreateShader(GL_VERTEX_SHADER);
            const char* cvertexSource = vertexSource.c_str();
            glShaderSource(vertexShader, 1, &cvertexSource, NULL);
            glCompileShader(vertexShader);
            // check the complilation result
            {
                GLint success;
                glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
                if (!success)
                {
                    GLint maxLength = 0;
                    glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

                    // The maxLength includes the NULL character
                    std::string log;
                    log.resize(maxLength);
                    glGetShaderInfoLog(vertexShader, maxLength, &maxLength, (GLchar*)log.data());
                    glDeleteShader(vertexShader);
                    
                    printf("ERROR: %s\n%s\n", name.c_str(), log.c_str());
                    return false;
                }
            }
            // fragment shader
            GLint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
            const char* cfragmentSource = fragmentSource.c_str();
            glShaderSource(fragmentShader, 1, &cfragmentSource, NULL);
            glCompileShader(fragmentShader);
            // agen check the result
            {
                GLint success;
                glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
                if (!success)
                {
                    GLint maxLength = 0;
                    glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

                    // The maxLength includes the NULL character
                    std::string log;
                    log.resize(maxLength);
                    glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, (GLchar*)log.data());
                    glDeleteShader(fragmentShader);

                    printf("ERROR: %s\n%s\n", name.c_str(), log.c_str());
                    return false;
                }
            }
            // geometry shader
            GLint geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
            if (geometry.size())
            {
                const char* cgeometrySource = geometry.c_str();
                glShaderSource(geometryShader, 1, &cgeometrySource, NULL);
                glCompileShader(geometryShader);
                // agen check the result
                {
                    GLint success;
                    glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
                    if (!success)
                    {
                        GLint maxLength = 0;
                        glGetShaderiv(geometryShader, GL_INFO_LOG_LENGTH, &maxLength);

                        // The maxLength includes the NULL character
                        std::string log;
                        log.resize(maxLength);
                        glGetShaderInfoLog(geometryShader, maxLength, &maxLength, (GLchar*)log.data());
                        glDeleteShader(geometryShader);

                        printf("ERROR: %s\n%s\n", name.c_str(), log.c_str());
                        return false;
                    }
                }
            }

            // Link shaders
            id = glCreateProgram();
            glAttachShader(id, vertexShader);
            glAttachShader(id, fragmentShader);
            if (geometry.size())
                glAttachShader(id, geometryShader);
            glLinkProgram(id);
            // And check the retult
            {
                GLint success;
                glGetProgramiv(id, GL_COMPILE_STATUS, &success);
                if (!success)
                {
                    GLint maxLength = 0;
                    glGetShaderiv(geometryShader, GL_INFO_LOG_LENGTH, &maxLength);

                    // The maxLength includes the NULL character
                    std::string log;
                    log.resize(maxLength);

                    glGetProgramInfoLog(id, maxLength, NULL, (GLchar*)log.data());
                    printf("ERROR: %s\n%s\n", name.c_str(), log.c_str());
                    return false;
                }
            }
            // delete both shaders
            glDeleteShader(geometryShader);
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            return true;
        }

        int Shader::getUni(const std::string& name)
        {
            Use();
            if (uniforms.find(name) != uniforms.end())
                return uniforms[name];

            int res = glGetUniformLocation(id, name.c_str());

            if (res == -1)
                printf_s("Shader: %s doesn't have uniform: %s\n", this->name.c_str(), name.c_str());

            uniforms[name] = res;
            return res;
        }


        Shader::Shader(const std::string& path)
        {
            name = path.substr(path.find_last_of('/') + 1);

            std::string vertexSource, fragmentSource, geometry;
            invalid = !parse_shaders(path, vertexSource, fragmentSource, geometry);
            invalid = invalid ? true : !compile_shaders(vertexSource, fragmentSource, geometry);
        }

        void Shader::Use() { glUseProgram(id); }

        // lone int 
        void Shader::setUni1i(const std::string& name, const int& val)
        {
            glcall(glUniform1i(getUni(name), val));
        }

        // float vec
        void Shader::setUni1f(const std::string& name, const float& val)
        {
            glcall(glUniform1f(getUni(name), val));
        }
        void Shader::setUni2f(const std::string& name, const glm::vec2& val)
        {
            glcall(glUniform2f(getUni(name), val.x, val.y));
        }
        void Shader::setUni3f(const std::string& name, const glm::vec3& val)
        {
            glcall(glUniform3f(getUni(name), val.x, val.y, val.z));
        }
        void Shader::setUni4f(const std::string& name, const glm::vec4& val)
        {
            glcall(glUniform4f(getUni(name), val.x, val.y, val.z, val.w));
        }

        // float matrices
        void Shader::setUniMat3(const std::string& name, const glm::mat3& val)
        {
            glUniformMatrix3fv(getUni(name), 1, GL_FALSE, glm::value_ptr(val));
        }
        void Shader::setUniMat4(const std::string& name, const glm::mat4& val)
        {
            glUniformMatrix4fv(getUni(name), 1, GL_FALSE, glm::value_ptr(val));
        }

}