#pragma once

#include "glm.hpp"
#include "GLFW/glfw3.h"

namespace gl
{
    // time
    extern float last_time;
    extern float delta_time;

    // input
    extern char input[512];

    extern float lastX, lastY;
    extern float MoffsetX, MoffsetY;
    extern float Mfov_offset;
    extern float sensetivity;

    bool pressed(int code);
    bool clicked(int code);
    float mouse_offsetX();
    float mouse_offsetY();
    float fov_offset();


	struct Window
	{
        GLFWwindow* m_Window;
		
        Window(const char* name, int width = 800, int height = 600);
        ~Window();

        void Resize(int width, int height);
        bool isOpen();

        glm::ivec2 GetSize();

        void SwapBuffer();

	};

}