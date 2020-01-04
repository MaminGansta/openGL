
static void parse_shaders(char const* filepath, std::string& vertex, std::string& fragment)
{
    enum Type { NONE = -1, VERTEX = 0, FRAGMENT = 1 } type = NONE;
    std::ifstream stream(filepath);

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
}

bool make_program(char const* filepath, int& shaderProgram)
{
    // parse shaders
    std::string vertexSource, fragmentSource;
    parse_shaders(filepath, vertexSource, fragmentSource);

    // compile shaders
    GLint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* cvertexSource = vertexSource.c_str();
    glShaderSource(vertexShader, 1, &cvertexSource,  NULL);
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
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // And check the retult
    {
        GLint success;
        GLchar info[512];
        glGetProgramiv(shaderProgram, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shaderProgram, 512, NULL, info);
            printf("%s", info);
            return false;
        }
    }
    // delete both shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return true;
}