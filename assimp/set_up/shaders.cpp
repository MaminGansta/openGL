
struct Shader
{
    uint32_t id;
    std::unordered_map<std::string, int> uniforms;
    bool invalid = false;

private:
    bool parse_shaders(char const* filepath, std::string& vertex, std::string& fragment)
    {
        enum Type { NONE = -1, VERTEX = 0, FRAGMENT = 1 } type = NONE;
        std::ifstream stream(filepath);

        if (!stream.is_open())
        {
            printf("file %s not exist", filepath);
            return false;
        }

        std::string line;
        std::stringstream shaders[2];

        while (std::getline(stream, line))
        {
            if (line.find("#shader") != std::string::npos)
            {
                if (line.find("vertex") != std::string::npos)
                    type = VERTEX;
                else if (line.find("fragment") != std::string::npos)
                    type = FRAGMENT;
            }
            else
                shaders[type] << line << '\n';
        }

        vertex = shaders[VERTEX].str();
        fragment = shaders[FRAGMENT].str();
        return true;
    }

    bool compile_shaders(const std::string& vertexSource, const std::string& fragmentSource)
    {
        // compile shaders
        GLint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        const char* cvertexSource = vertexSource.c_str();
        glShaderSource(vertexShader, 1, &cvertexSource, NULL);
        glCompileShader(vertexShader);
        // check the complilation result
        {
            GLint success;
            GLchar info[512];
            glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(vertexShader, 512, NULL, info);
                printf("%s", info);
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
            GLchar info[512];
            glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(fragmentShader, 512, NULL, info);
                printf("%s", info);
                return false;
            }
        }
        // Link shaders
        id = glCreateProgram();
        glAttachShader(id, vertexShader);
        glAttachShader(id, fragmentShader);
        glLinkProgram(id);
        // And check the retult
        {
            GLint success;
            GLchar info[512];
            glGetProgramiv(id, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(id, 512, NULL, info);
                printf("%s", info);
                return false;
            }
        }
        // delete both shaders
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return true;
    }

    int getUni(const std::string& name)
    {
        use();
        if (uniforms.find(name) != uniforms.end())
            return uniforms[name];

        int res = glGetUniformLocation(id, name.c_str());
#if _DEBUG
        if (res == -1)
            std::cout << "no such uniform: " << name << '\n';
#endif
        uniforms[name] = res;
        return res;
    }

public:

    Shader(const char* filepath)
    {
        std::string vertexSource, fragmentSource;
        invalid = !parse_shaders(filepath, vertexSource, fragmentSource);
        invalid = invalid ? true : !compile_shaders(vertexSource, fragmentSource);
    }

    void use() { glUseProgram(id); }

    // lone int 
    void setUni1i(const std::string& name, const int& val)
    {
        glcall(glUniform1i(getUni(name), val));
    }

    // float vec
    void setUni1f(const std::string& name, const float& val)
    {
        glcall(glUniform1f(getUni(name), val));
    }
    void setUni2f(const std::string& name, const glm::vec2& val)
    {
        glcall(glUniform2f(getUni(name), val.x, val.y));
    }
    void setUni3f(const std::string& name, const glm::vec3& val)
    {
        glcall(glUniform3f(getUni(name), val.x, val.y, val.z));
    }
    void setUni4f(const std::string& name, const glm::vec4& val)
    {
        glcall(glUniform4f(getUni(name), val.x, val.y, val.z, val.w));
    }

    // float matrices
    void setUniMat3(const std::string& name, const glm::mat3& val)
    {
        glUniformMatrix3fv(getUni(name), 1, GL_FALSE, glm::value_ptr(val));
    }
    void setUniMat4(const std::string& name, const glm::mat4& val)
    {
        glUniformMatrix4fv(getUni(name), 1, GL_FALSE, glm::value_ptr(val));
    }
};