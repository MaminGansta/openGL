

/*

    // set callback functions
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

*/


#define PI 3.14159265359f

template <typename T>
int sgn(T num) { return (num > T(0)) - (num < T(0)); }


// error handler
#define ASSERT(x) if (!(x)) __debugbreak();
#define glcall(func) GLClearError();\
    func;\
    ASSERT(GLLogCall(#func, __FILE__, __LINE__))


static void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[openGL Error] >" << error << '\n'
            << file << ':' << line << '\n';
        return false;
    }
    return true;
}




// time
float last_time = 0;
float delta_time = 0;

// globals
char input[512];

float lastX = 400, lastY = 300;
float MoffsetX = 0, MoffsetY = 0;
float Mfov_offset = 0;
float sensetivity = 0.004f;

int window_h, window_w;

bool pressed(int code)
{
    return input[code];
}

bool clicked(int code)
{
    return input[code] == 1;
}

float mouse_offsetX()
{
    float temp = MoffsetX;
    MoffsetX = 0;
    return temp;
}

float mouse_offsetY()
{
    float temp = MoffsetY;
    MoffsetY = 0;
    return temp;
}

float fov_offset()
{
    float temp = Mfov_offset;
    Mfov_offset = 0;
    return temp;
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_W)
        input[GLFW_KEY_W] = action;
    if (key == GLFW_KEY_S)
        input[GLFW_KEY_S] = action;
    if (key == GLFW_KEY_A)
        input[GLFW_KEY_A] = action;
    if (key == GLFW_KEY_D)
        input[GLFW_KEY_D] = action;
    if (key == GLFW_KEY_D)
        input[GLFW_KEY_D] = action;

    if (key == GLFW_KEY_UP)
        input[GLFW_KEY_UP] = action;
    if (key == GLFW_KEY_DOWN)
        input[GLFW_KEY_DOWN] = action;
    if (key == GLFW_KEY_LEFT)
        input[GLFW_KEY_LEFT] = action;
    if (key == GLFW_KEY_RIGHT)
        input[GLFW_KEY_RIGHT] = action;

}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    MoffsetX = (xpos - lastX) * sensetivity;
    MoffsetY = (lastY - ypos) * sensetivity;

    lastX = xpos;
    lastY = ypos;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
        Mfov_offset -= yoffset;
}