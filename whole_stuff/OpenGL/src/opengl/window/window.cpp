
#include "pch.h"
#include "window.h"


namespace gl
{
    static int glfw_init()
    {
        if (!glfwInit())
        {
            printf("glfw init fail\n");
            exit(EXIT_FAILURE);
        }
        return 0;
    }

    static int glew_init()
    {
        if (glewInit() != GLEW_OK)
        {
            printf("GLEW INIT FAIL\n");
            glfwTerminate();
            exit(EXIT_FAILURE);
        }
        return 0;
    }


    // callback
    void error_callback(int error, const char* description)
    {
        fputs(description, stderr);
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


    void window_size_callback(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }


	Window::Window(const char* name, int width, int height)
	{
        // set glfw
        static int glfw = glfw_init();

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

        
        m_Window = glfwCreateWindow(width, height, name, NULL, NULL);
        if (!m_Window)
        {
            glfwTerminate();
            exit(EXIT_FAILURE);
        }
        glfwMakeContextCurrent(m_Window);

        // set callback functions
        glfwSetErrorCallback(error_callback);
        glfwSetKeyCallback(m_Window, key_callback);
        glfwSetCursorPosCallback(m_Window, mouse_callback);
        glfwSetScrollCallback(m_Window, scroll_callback);
        glfwSetWindowSizeCallback(m_Window, window_size_callback);


        //glfwSetCursorPos(window, lastX, lastY);
        //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSwapInterval(1);

        // set glew
        static int glew = glew_init();

        // veiwport
        glViewport(0, 0, width, height);
	}

    Window::~Window()
    {
        glfwDestroyWindow(m_Window);
    }

    bool Window::isOpen()
    {
        return !glfwWindowShouldClose(m_Window);
    }

    glm::ivec2 Window::GetSize()
    {
        int width, height;
        glfwGetWindowSize(m_Window, &width, &height);
        return glm::ivec2(width, height);
    }

    void Window::SwapBuffer()
    {
        glfwSwapBuffers(m_Window);
    }

    void Window::Resize(int width, int height)
    {
        glfwSetWindowSize(m_Window, width, height);
    }

}